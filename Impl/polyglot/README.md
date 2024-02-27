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
We recommend to use the SciDB docker image.

Please make sure that you have configured each system.

### Data Loading

Before running tasks, you need to load the generated data to each database system. 
In the `load_datasets` directory, you can find directories for each scenario in M2Bench.
These directories contains scripts for data loading and creating indexes.

We provide a script named `import.sh` in the `load_datasets` directory.
This script is configured to use database configurations below. If your database instance has different configuration, please modify files in each scenario directory to match your configuration.
- MySQL: the connection port is `3306` and a password should not be required.
- MongoDB: the connection port is `27017` and a password should not be required.
- Neo4j: the connection port is `7687`. Authentication is required when you run the script.
- SciDB: the connection port to SciDB Shim is `8080`.

Before you run the script, please read the below:
- Please install the Graph Data Science library (GDS) v1.6.0 for Neo4j.
- Please make sure that the user to run the command should have read/write permission to `/var/lib/neo4j/import` and `/tmp`.
- Make sure that you are able to call `mysql` (for MySQL), `mongoimport` (for MongoDB), `cypher-shell` (for Neo4J), and `iquery` (for SciDB).
- If you do not have `iquery`, the script will import data to SciDB inside the docker conatiner. You will be asked for the name of the conainter in this case.

In order to import data, naviate to the `/Impl/polyglot/load_datasets` directory and run the following command. 
```bash
$ bash import.sh
```

## Running Tasks

### Prerequsite

Please install the below software to build Polyglot persistence.

- Cmake >= 3.11.x 
- [Neo4j client](https://neo4j-client.net/)
- [MySQL C++ connector for MySQL 8.0.25](https://dev.mysql.com/downloads/)
- [MongoDB C++ connector 3.6.2](http://mongocxx.org/mongocxx-v3/installation/)

### Build Polyglot

Before you build Polyglot persistence, you need to set connection configurations correctly.
Please navigate to the `include/Connection` directory and update each `mongodb_connector.h`, `mysql_connector.h`, and `neo4j_connector.h` file to meet your database connection information.
Next, please navigate to the `src/Scenarios/PolyglotDB` directory and modify `SCIDB_HOST_XXX` variables in each `Ecommerce_Tasks.h`, `Healthcare_Tasks.h`, and `Disaster_Tasks.h`.

Now you can build Polyglot persistence with the following commands.

```bash
mkdir build
cd build
cmake ..
make
```

### Run Polyglot

The following command runs the whole tasks (i.e., Task 0 to Task 16).

```bash
./m2bench
```

Since M2Bench data are generated from real-world datasets that is updated sometimes, you may have to check if the input argument of a task is valid.
For example, the task 11 requires an input argument `earthquake_id` and the default value `41862` may not return anything.
At the point we write this documentation, the id of the same earthquake is `43744` and if you change the value of the argument to this value, you can find appropriate results.
We attach the row of the earthquake for your reference.
```csv
43744,2020-09-16T00:06:39.040Z,35.592167,-117.597500,4.26,1.58
```

