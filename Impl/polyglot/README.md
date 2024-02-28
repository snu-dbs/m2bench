## Polyglot Persistence

Polyglot persistence consists of MySQL, MongoDB, Neo4J, and SciDB to support each data model with their native database system.
Please make sure that you have already installed each DBMS on your machine with appropriate versions.
- MySQL 8.0.25
- MongoDB 4.2.6
- Neo4j 4.0.3
- SciDB 19.11.5

SciDB community edition that we can use freely is no longer provided by Paradigm4.
Thus, we provide [a copy of SciDB 19.11.5](https://github.com/snu-dbs/scidb-19.11.5.f8334b60/).
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
- Please make sure that a user to run the command should have read/write permission to `/var/lib/neo4j/import` and `/tmp`.
- Make sure that you are able to call `mysql` (for MySQL), `mongoimport` (for MongoDB), `cypher-shell` (for Neo4J), and `iquery` (for SciDB).
- If you do not have `iquery`, the script will import data to SciDB inside the docker conatiner. You will be asked for the name of the conainter in this case. In such case, you need a permission to run the `docker` command.
- If you are going to run SciDB inside a docker container, please allocate enough shared memory size (i.e., `/dev/shm`) when you create a container. 

In order to import data, naviate to the `/Impl/polyglot/load_datasets` directory and run the following command. 
```bash
$ bash import.sh
```

## Running Tasks

### Prerequsite

Please install the below software to build Polyglot persistence.

- Cmake >= 3.11.x 
- [Neo4j client v5.0.3 by majensen](https://github.com/majensen/libneo4j-client) (not [the original repository](https://github.com/cleishm/libneo4j-client))
- [MySQL C++ connector for MySQL v8.0.25](https://dev.mysql.com/downloads/)
- [MongoDB C Driver & BSON Library v1.17.4](https://mongoc.org/libmongoc/current/installing.html)
- [MongoDB C++ connector v3.6.2](http://mongocxx.org/mongocxx-v3/installation/)

The Neo4j client might need a more configuration to build.
The below script is to build the project from the source code.

```bash
$ apt install -y libedit-dev autoconf libtool pkg-config libssl-dev
$ wget https://github.com/majensen/libneo4j-client/archive/refs/tags/v5.0.3.tar.gz
$ tar zxvf v5.0.3.tar.gz
$ cd libneo4j-client-5.0.3
$ ./autogen.sh
$ vim configure.ac    # Remove  "-Wno-error=stringop-truncation -Wno-unknown-warning-option" in the "GCC_FLAGS". You can use another text editor.
$ ./configure --disable-tools --without-tls --disable-werror
$ make 
# make install if you need it
```


### Build Polyglot

Before you build Polyglot persistence, you need to set connection configurations correctly.
Please navigate to the `include/Connection` directory and update each `mongodb_connector.h`, `mysql_connector.h`, and `neo4j_connector.h` file to meet your database connection information.
Next, please navigate to the `src/Polyglot` directory and modify `SCIDB_HOST_XXX` variables in each `Ecommerce_Tasks.cpp`, `Healthcare_Tasks.cpp`, and `Disaster_Tasks.cpp`.

Now you can build Polyglot persistence with the following commands.

```bash
mkdir build
cd build
cmake ..
make
```

### Run Polyglot

The following command runs the whole tasks (i.e., Task 0 to Task 16).
Please note that the first time, you might see some SciDB errors during task execution (caused by `remove()` operations).
These errors would be disappeared the next time.

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

