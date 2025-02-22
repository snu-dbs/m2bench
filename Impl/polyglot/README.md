## Polyglot Persistence

Polyglot persistence consists of MySQL, MongoDB, and SciDB to support each data model with their native database system.
Please make sure that you have already installed each DBMS on your machine with the appropriate versions.

- MySQL 9.2.0
- MongoDB 8.0.4
- SciDB 19.11.5

SciDB community edition that we can use freely is no longer provided by Paradigm4.
Thus, we provide [a copy of SciDB 19.11.5](https://github.com/snu-dbs/scidb-19.11.5.f8334b60/).
Alternatively, you can use [a docker image](https://hub.docker.com/layers/rvernica/scidb/19.11-xenial/images/sha256-1e2cedc9bd6a4df47de03aa10fe2eae1b94b88fe2a5767d94aa286572f3cebdf?context=explore) shipping with pre-installed SciDB.
We recommend using the SciDB docker image.

Please make sure that you have configured each system.

### Data Loading

Before running tasks, you need to load the generated data to each database system.
In the `load_datasets` directory, you can find directories for each scenario in M2Bench.
These directories contain scripts for data loading and creating indexes.

We provide a script named `load_all.sh` in the `load_datasets` directory.
This script is configured to use the database configurations below.
If your database instance has a different configuration, please modify files in each scenario directory to match your configuration.

- MySQL: the connection port is `3306` and a password should not be required.
- MongoDB: the connection port is `27017` and a password should not be required.
- SciDB: the connection port to SciDB Shim is `8080`.

Before you run the script, please read the below:

- Make sure that you are able to call `mysql` (for MySQL), `mongoimport` (for MongoDB), and `iquery` (for SciDB).
- If you do not have `iquery`, the script will import data to SciDB inside the docker container.
  You will be asked for the name of the container in this case.
  In such a case, you need permission to run the `docker` command.
- If you are going to run SciDB inside a docker container, please allocate enough shared memory size (i.e., `/dev/shm`) when you create a container.

In order to import data, navigate to the `/Impl/polyglot/load_datasets` directory and run the following command.

```bash
$ bash load_all.sh
```

## Running Tasks

### Prerequisite

Please install the software below to build Polyglot persistence.

- Cmake >= 3.15.x
- [MySQL C++ connector for MySQL v9.2.0](https://dev.mysql.com/downloads/connector/cpp/)
- [MongoDB C++ connector v4.0](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/get-started/download-and-install/)

### Build Polyglot

Before you build Polyglot persistence, you need to set connection configurations correctly.
Please navigate to the `include/Connection` directory and update each `mongodb_connector.h`, `mysql_connector.h`, and `neo4j_connector.h` file to meet your database connection information.
Next, please navigate to the `src/Polyglot` directory and modify `SCIDB_HOST_XXX` variables in each `Ecommerce_Tasks.cpp`, `Healthcare_Tasks.cpp`, and `Disaster_Tasks.cpp`.

Now, you can build Polyglot persistence with the following commands.

```bash
mkdir build
cd build
cmake ..
make
```

### Run Polyglot

The following command runs the tasks required for TileDuck experiments (i.e., Task 0, Task 2, Task 9, Task 14 to Task 16).
Please note that the first time, you might see some SciDB errors during task execution (caused by `remove()` operations).
These errors would be disappeared the next time.

```bash
./m2bench
```
