# [M2Bench: A Database Benchmark for Multi-Model Analytic Workloads](https://www.vldb.org/pvldb/vol16/p747-moon.pdf)
- M2Bench is a new benchmark program capable of evaluating a multi-model DBMS. 
- M2Bench supports several important data models including **relational, document-oriented, property graph and array models**.
- M2Bench consists of multi-model workloads that are inspired by real-world problems. 
- Each task of the workloads mimics a real-world problem that involves at least two different models of data.


## Before Getting Started

- If you need to get a support during reproducing this work, feel free to contact us.
- We highly recommend to use the described database systems with Ubuntu 18.04 that we used for the publication.


### DBMS Versions

DBMS versions used for the experiment in the paper are as follows.

- Polyglot: MySQL 8.0.25, MongoDB 4.2.6, Neo4j 4.0.3, and SciDB 19.11.5.
- ArangoDB 3.7.12
- AgensGraph 2.1.3 (based on PostgreSQL 10.4)


## How to Run M2Bench

In order to run M2Bench, please follow the below steps.

1. Generate datasets of M2Bench following the instructions in `Datasets/README.md`.
2. Choose DBMS X `[agensgraph|arangodb|polyglot]`. 
3. Load the datasets into DBMS X and run tasks, following the instructions in `Impl/X/README.md`.


## Directory Overview

- All datasets in M2Bench are explained in the `Datasets` folder. 
- Data loading scripts and query implementations are stored in the `Impl` directory.
- All tasks in M2Bench are explained in the `/Tasks/alltasks.md` file. 


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