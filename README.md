# Incremental Maximal Clique Enumeration for Hybrid Edge Changes with Vertex Scope Constraints
This is C++ implement (with python code to generate data) of the paper "Incremental Maximal Clique Enumeration for Hybrid Edge Changes with Vertex Scope Constraints", ICDE 2022.
## Data generation
All data are stored into the *DATA/final_file* folder. 
The real-world networks can be freely accessed online:https://snap.stanford.edu/data/ or http://networkrepository.com/index.php. 
The synthetic graphs are generated using the graph generator tool *NetworkX* and *GTGraph*.
Run the jupyter file to generate add_minus/mix/degree related data.
## Data preparation
Put data to be used into folder *max_clique/clean/*.
And modify data and batch size in file *GraphMining/main.cpp*, at the same time, choose algorithm to be used.
## Compile and compute
using the following commmand to compile the code with linux:
    ./run.sh
when you start to compute, you can run:
    nohup ./a.out >Graph_self.log 2>&1 &
## citation
If you use this repository in your research, please cite the following paper:
>todo
