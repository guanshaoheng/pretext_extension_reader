# Pretext extension reader


## Usage

Please put file `PretextParserPython.cpython-311-darwin.so` and `genome_class.py` in same directory.

Usage:
```python
from genome_class import Genome_PY

pretext_file_path:str = ...
genome_py = Genome_PY(pretext_file_path)
graph_name_list = genome_py.get_graph_names()
graph_data_list = genome_py.get_graph_data()


```