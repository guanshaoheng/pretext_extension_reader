
'''
    Copyright 2024 Shaoheng Guan sg35@sanger.ac.uk, Wellcome Sanger Institute
'''

import os
import time
import PretextParserPython as pp          # export PYTHONPATH=/Users/sg35/pretext_parser/build_cmake/src:$PYTHONPATH
import matplotlib.pyplot as plt
import numpy as np
from typing import List
from utils import timer, check_and_mkdir  # export PYTHONPATH=/Users/sg35/pretext_parser/src/python:$PYTHONPATH


print(pp.__doc__)


class Genome_PY:
    @timer
    def __init__(
        self, 
        path_pretext:str
    ) -> None:
        self.genome_py = pp.Genome_PY(path_pretext)
        self.name = self.genome_py.genome.name
        self.dir_path = os.path.dirname(path_pretext)
        self.total_genome_len = self.genome_py.genome.total_genome_len
        self.n_textures = self.genome_py.genome.n_textures
        self.n_textures_1d = self.genome_py.genome.n_textures_1d
        self.n_pixels_1d = self.genome_py.genome.n_pixels_1d
        self.n_pixels_per_texture_1d = self.genome_py.genome.n_pixels_per_texture_1d
        self.number_of_original_contigs = self.genome_py.genome.number_of_original_contigs
    
    """
        todo(shaoheng): 修改此处保证在进行reduction之后的extension长度一致
    """
    @timer
    def get_graph_data(self, ):
        graph_data = self.genome_py.get_graph_data() 
        for i in range(len(graph_data)):
            assert len(graph_data[i] == self.n_pixels_1d), \
                f"length of graph data[{i}] should be ({self.n_pixels_1d}), but is ({len(graph_data[i])})"
        return graph_data
    
    @timer
    def get_graph_names(self, ):
        graph_names:List = self.genome_py.get_graph_names()
        return graph_names

    @timer
    def plot_graph(self, ):
        """
          显示读取的graph
        """
        fig_dir_path = os.path.join(self.dir_path, "figs/" + self.genome_py.genome.name)
        check_and_mkdir(self.dir_path + "/figs", fig_dir_path)

        graph_names = self.get_graph_names()
        graph_data = self.get_graph_data()
        for i in range(len(graph_names)):
            fig = plt.figure(figsize=(10, 3))
            plt.plot(range(len(graph_data[i])), graph_data[i], label = graph_names[i])
            plt.title(f"{self.name} ({self.n_pixels_1d}) pixels")
            plt.xlabel("Pixels")
            plt.ylabel("Extension signal")
            plt.legend()
            plt.tight_layout()
            plt.savefig(os.path.join(fig_dir_path, rf"graph_{i:d}_{graph_names[i]:s}.png"), dpi=300)
            plt.close("all")
        print(f"Figures are saved in: {fig_dir_path}")
    

def main():
    for i in range(len(pretext_file_paths)): 
        print('='*50 + "\n" + " "*20 + f"{i+1}/{len(pretext_file_paths)}")
        print(pretext_file_paths[i])
        obj = Genome_PY(pretext_file_paths[i])
        names = obj.get_graph_names()
        data = obj.get_graph_data()
        obj.plot_graph()
        del obj
        print('='*50)


if __name__ == "__main__":

    pretext_file_paths = [
        "/Users/sg35/auto-curation/data/data_auto_curation/pretexts/gfHerCora1_2_normal.pretext",
        "/Users/sg35/auto-curation/data/data_auto_curation/pretexts/daLamAmpl1_2_hr.pretext",
    ]
    start_time = time.time()
    main()
    print(f"\n\nFinished in ({(time.time() - start_time)/60.}) mins.")
    print("\n" + "="*50)
