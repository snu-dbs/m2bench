## Polyglot Persistence

**This documentation is under construction.**

Polyglot persistence consists of MySQL, MongoDB, Neo4J, and SciDB to support each data model with their native database system.
Please make sure that you have already installed each DBMS on your machine with appropriate versions.
- MySQL 8.0.25
- MongoDB 4.2.6
- Neo4j 4.0.3
- SciDB 19.11.5

SciDB community edition that we can use freely is no longer provided by Paradigm4.
Thus, we provide [a copy of SciDB 19.11.5](#).
Alternatively, you can use [a docker image](https://hub.docker.com/layers/rvernica/scidb/19.11-xenial/images/sha256-1e2cedc9bd6a4df47de03aa10fe2eae1b94b88fe2a5767d94aa286572f3cebdf?context=explore) shipping with pre-installed SciDB.

Before running tasks, you need to load the generated data to each database system. 
In the `load_datasets` directory, you can find directories for each scenario in M2Bench.
These directories contains scripts for data loading and creating indexes.

We provide a script named `import.sh` in the `load_datasets` directory.
This script is configured to use database configurations below. If your database instance has different configuration, please modify files in each scenario directory to match your configuration.
- MySQL: the connection port is `3306` and a password should not be required.
- MongoDB: the connection port is `27017` and a password should not be required.
- Neo4J: the connection port is `7687`. Authentication is required when you run the script.
- SciDB: the connection port to SciDB Shim is `8080`.

In order to import data, you simply need to do the followings:
```bash
$ bash import.sh
```
