# M2Bench: A Database Benchmark for Multi-Model Workloads
- M2Bench is a  new benchmark program capable of evaluating a multi-model DBMS. 
- M2Bench supports several important data models including relational,document-oriented, property graph and array models.
- M2Bench consists of multi-model  workloads that are inspired by real-world problems. 
- Each task of the workloads mimics a real-world problem that involves at least two different models  of  data.

## How to Run M2Bench
- Generate datasets of M2Bench following the instructions in the `Datasets` folder.
- Choose DBMS X [agensgraph|arangodb|orientdb|polyglot]. 
- Load the datasets into DBMS following the instructions in the `Impl/X/load_datasets' folder.  
- Run tasks of M2Bench following the instruction in the 'Imp/X/run_tasks' folder.  
- All tasks in M2Bench is explained in the 'Tasks' folder. 

```
[Directory Description]

m2bench
├── Datasets  [How to generate the datasets of m2bench from the realworld datasets.]
│   ├── ecommerce
│   ├── healthcare
│   ├── disaster
│   └── README.md
├── Tasks  [Description of  all tasks of m2bench]
|   └── alltasks.md
├── Impl  [Implementation of all tasks on various DBMSs]
│   ├── agensgraph  
│   │   ├── load_datasets [How to load datasets into DBMS]
│   │   └── run_tasks [How to run tasks]
│   ├── arangodb
│   │   ├── load_datasets 
│   │   └── run_tasks
│   ├── orientdb
│   │   ├── load_datasets 
│   │   └── run_tasks
│   └── polyglot [polyglot persistence]
│       ├── load_datasets
│       └── run_tasks
├── README.md
└── LICENSE
``` 

