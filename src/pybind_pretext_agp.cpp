/*
    Copyright 2024 Shaoheng Guan sg35@sanger.ac.uk, Wellcome Sanger Institute
*/

#ifndef SRC_INCLUDE_PRETEXT_PARSING_PYBIND_PRETEXT_AGP_H_
#include "pybind_pretext_agp.h"
#endif  // #ifndef SRC_INCLUDE_PRETEXT_PARSING_PYBIND_PRETEXT_AGP_H_

template<typename T>
py::array_t<T> transform_arr2numpy_1d(
    u32 len,
    T* arr) {
    py::array_t<T> arr_numpy(len);
    T* ptr_buffer = reinterpret_cast<T*>(arr_numpy.request().ptr);
    /* 1d数组 */
    std::copy(arr, arr+len, ptr_buffer);
    return arr_numpy;
}

template<typename T>
py::array_t<T> transform_arr2numpy_2d(
    std::vector<u32> shape,
    T** arr) {
    py::array_t<T> arr_numpy(shape);
    T* ptr_buffer = static_cast<T*>(arr_numpy.request().ptr);
    for (unsigned int col = 0; col < shape[0]; col ++) {
        std::copy(arr[col], arr[col] + shape[1], ptr_buffer+col*shape[1]);
    }
    return arr_numpy;
}


py::list Genome_PY::get_graph_names() {
    py::list names(genome->graphs.size());
    ForLoop(genome->graphs.size()) {
        names[index] = genome->graphs[index].name;
    }
    return names;
}

py::array_t<u32> Genome_PY::get_graph_data() {
    std::vector<u32> shape = {static_cast<u32>(genome->graphs.size()), genome->n_pixels_1d};
    py::array_t<u32> graph_data(shape);

    ForLoop(genome->graphs.size()) {
        ForLoop1(genome->graphs[index].data.size()) {
            graph_data.mutable_at(index, index1) = genome->graphs[index].data[index1];
        }
    }
    return graph_data;
}


PYBIND11_MODULE(PretextParserPython, m) {
    m.doc() =
        "\n=====================================================\n"
        "               Pretext_Parser to Python\n"
        "\n"
        "                         v0.0"
        "\n=====================================================\n";

    m.def("glfw_init", glfw_init);
    m.def("glfw_terminate", glfwTerminate);

    py::class_<Graph>(m, "Graph")
        .def(py::init<u32>())
        .def_readwrite("name", &Graph::name)
        .def_readwrite("data", &Graph::data);

    py::class_<OriginalContig>(m, "OriginalContig")
        .def(py::init<>())  // 初始化函数
        .def_readwrite("name", &OriginalContig::name)
        .def_readwrite("n_fragments", &OriginalContig::n_fragments)
        .def_readwrite("pad", &OriginalContig::pad)
        .def_readwrite("contig_fracs", &OriginalContig::contig_fracs)
        .def_readwrite("contig_map_pixels", &OriginalContig::contig_map_pixels);

    py::class_<Fragment>(m, "Fragment")
        .def(py::init<>())  // 初始化函数
        .def_readwrite("meta_data_flags", &Fragment::meta_data_flags)
        .def_readwrite("original_contig_id", &Fragment::original_contig_id)
        .def_readwrite("length", &Fragment::length)
        .def_readwrite("start_coord", &Fragment::start_coord)
        .def_readwrite("scaff_id", &Fragment::scaff_id)
        .def_readwrite("pad", &Fragment::pad)
        .def_readwrite("inverted_flag", &Fragment::inverted_flag);

    py::class_<Map_State>(m, "Map_State")
        .def(py::init<>())  // 初始化函数
        .def_readwrite("contigs_ids", &Map_State::contigs_ids)
        .def_readwrite("original_contigs_ids", &Map_State::original_contigs_ids)
        .def_readwrite("scaff_ids", &Map_State::scaff_ids)
        .def_readwrite("contig_local_coords", &Map_State::contig_local_coords)
        .def_readwrite("meta_data_flags", &Map_State::meta_data_flags);

    py::class_<GenomeData>(m, "GenomeData")
        .def(py::init<std::string, u32, u32, u32, u32, u32>())
        .def_readwrite("name", &GenomeData::name)
        .def_readwrite("file_path", &GenomeData::file_path)
        .def_readwrite("total_genome_len", &GenomeData::total_genome_len)
        .def_readwrite("number_of_original_contigs", &GenomeData::number_of_original_contigs)
        .def_readwrite("number_of_fragments", &GenomeData::number_of_fragments)
        .def_readwrite("n_pixels_per_texture_1d", &GenomeData::n_pixels_per_texture_1d)
        .def_readwrite("n_textures_1d", &GenomeData::n_textures_1d)
        .def_readwrite("n_textures", &GenomeData::n_textures)
        .def_readwrite("n_pixels_1d", &GenomeData::n_pixels_1d)
        .def_readwrite("mip_map_levels", &GenomeData::mip_map_levels)
        .def_readwrite("n_bytes_per_texture", &GenomeData::n_bytes_per_texture);
        // .def_readwrite("graphs", &GenomeData::graphs);

    py::class_<Genome_PY>(m, "Genome_PY").def(py::init<std::string>())
        .def_readonly("genome", &Genome_PY::genome)
        .def("get_graph_names", &Genome_PY::get_graph_names)
        .def("get_graph_data", &Genome_PY::get_graph_data);
}

