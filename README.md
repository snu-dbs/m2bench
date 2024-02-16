# [M2Bench: A Database Benchmark for Multi-Model Analytic Workloads](https://www.vldb.org/pvldb/vol16/p747-moon.pdf)
- M2Bench is a new benchmark program capable of evaluating a multi-model DBMS. 
- M2Bench supports several important data models including **relational, document-oriented, property graph and array models**.
- M2Bench consists of multi-model workloads that are inspired by real-world problems. 
- Each task of the workloads mimics a real-world problem that involves at least two different models of data.

## Datasets
- All datasets in M2Bench are explained in the `Datasets` folder. 

## Workloads (Tasks)
- All tasks in M2Bench are explained in the `Tasks` folder. 

## How to Run M2Bench
1. Generate datasets of M2Bench following the instructions in the `Datasets` folder.
2. Choose DBMS X `[agensgraph|arangodb|orientdb|polyglot]`. 
3. Load the datasets into DBMS X following the instructions in the `Impl/X/load_datasets` folder.  
4. Run tasks of M2Bench following the instruction in the `Imp/X/run_tasks` folder.  


## Directory Overview
```
m2bench
├── Datasets                # How to generate the datasets of M2Bench from the realworld datasets.
│   ├── ecommerce          
│   ├── healthcare
│   ├── disaster
│   └── README.md
├── Tasks                   # Description of all tasks of M2Bench
|   └── alltasks.md
├── Impl                    # Implementation of tasks of M2Bench on various DBMSs
│   ├── agensgraph  
│   │   ├── load_datasets   # How to load datasets into DBMS
│   │   └── run_tasks       # How to run tasks
│   ├── arangodb
│   │   ├── load_datasets 
│   │   └── run_tasks
│   ├── orientdb
│   │   ├── load_datasets 
│   │   └── run_tasks
│   └── polyglot            # polyglot persistence
│       ├── load_datasets
│       └── run_tasks
├── README.md
└── LICENSE
``` 

