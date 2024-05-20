/*
    Copyright 2024 Shaoheng Guan sg35@sanger.ac.uk, Wellcome Sanger Institute
*/
#ifndef SRC_INCLUDE_PRETEXT_PARSING_PYBIND_PRETEXT_AGP_H_
#include "pybind_pretext_agp.h"
#endif  // #ifndef SRC_INCLUDE_PRETEXT_PARSING_PYBIND_PRETEXT_AGP_H_


// "/Users/sg35/auto-curation/log/learning_notes/hic_curation/6 xgPatPell1_1/xgPatPell1_1.pretext.agp";
// "/Users/sg35/auto-curation/log/learning_notes/hic_curation/7 ilHypFluo3_2/ilHypFluo3_2_hr.pretext.agp_1";
// "/Users/sg35/auto-curation/log/learning_notes/hic_curation/3 iyExelsch1_1/iyExeIsch1_1.pretext.agp";
// "/Users/sg35/auto-curation/log/learning_notes/hic_curation/3 iyExelsch1_1/iyExeIsch1_1.pretext.agp_1";

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::stringstream ss;
        ss << "Usage: ./func <path/to/.agp file>\nBut used as:\n";
        ForLoop(argc) {
            ss << argv[index] << " ";
        }
        ss << "\n";
        throw std::runtime_error(ss.str());
    }

    // Genome_agp* genome_agp = get_genome_agp(argv[1]);
    // MapState_agp* curated_map_state = get_map_state_agp(
    //     genome_agp, 32, 2048, 1024);
    glfw_init();
    Genome_PY* genome_py = new Genome_PY(
        "/Users/sg35/auto-curation/data/data_auto_curation/pretexts/sHydCol1_3_hr.pretext");
    glfwTerminate();
    delete genome_py;

    return 0;
}