/*
    Copyright 2024 Shaoheng Guan sg35@sanger.ac.uk, Wellcome Sanger Institute
*/

#ifndef SRC_INCLUDE_PRETEXT_PARSING_PYBIND_PRETEXT_AGP_H_
#define SRC_INCLUDE_PRETEXT_PARSING_PYBIND_PRETEXT_AGP_H_

#ifndef SRC_INCLUDE_PRETEXT_PARSING_PRETEXT_PARSER_H_
#include "pretext_parser.h"
#endif  // SRC_INCLUDE_PRETEXT_PARSING_PRETEXT_PARSER_H_


#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // 如果不包含这个 在交换vector或者数组报错
#include <pybind11/numpy.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <utility>

#define DISTANCE(x, y) x >= y ? x-y : y-x

typedef std::pair<u32, u32> u32_pair;
typedef std::priority_queue<
    u32_pair,
    std::vector<u32_pair>,
    std::function<bool(const u32_pair&, const u32_pair&)>> min_heap;


namespace py = pybind11;


template<typename T>
py::array_t<T> transform_arr2numpy_1d(
    u32 len,
    T* arr);

template<typename T>
py::array_t<T> transform_arr2numpy_2d(
    std::vector<u32> shape,
    T** arr);


class Genome_PY{
public:
    GenomeData* genome;
    explicit Genome_PY(std::string pretext_file_path) {
        genome = load_file(pretext_file_path.c_str(), 0);  // use the original resolution
    }
    ~Genome_PY() {
        delete genome;
    }
    py::list get_graph_names();
    py::array_t<u32> get_graph_data();
};

#endif  // SRC_INCLUDE_PRETEXT_PARSING_PYBIND_PRETEXT_AGP_H_

