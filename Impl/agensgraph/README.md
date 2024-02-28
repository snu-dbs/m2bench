## AgensGraph

Please make sure that you have already installed AgensGraph 2.1.3 on your machine.
Since M2Bench includes spatial queries, you need to install the PostGIS extension.
Please install PostGIS 2.5.0 to AgensGraph as well.

AgensGraph requires the `file_fdw` extension to import graph data. 
If you build AgensGraph from their source code, you might need to build stuff in the `contrib` directory of the AgensGraph source tree.
Please consult with the `contrib/README` file in the AgensGraph source tree.

Before running tasks, you need to load the generated data to each database system. 
In the `load_datasets` directory, you can find directories for each scenario in M2Bench.
These directories contain scripts for data loading and creating indexes.

We provide a script named `import.sh` in the `load_datasets` directory.
This script is configured to use the `5432` connection port and try to connect without a password. If your database instance has different configuration, please modify files in each scenario directory to match your configuration.

Before you run the script, please read the below:
- Please make sure that a user to run the command should have read/write permission to the `/tmp` directory.
- Make sure that you are able to call the `agens` command in anywhere.
- Create databases named `ecommerce`, `healthcare`, and `disaster` for each scenario. **Please make sure that each database is configured to use the `UTF8` encoding** (Otherwise, you may encounter importing error).

In order to import data, naviate to the `/Impl/agensgraph/load_datasets` directory and run the following command. 
```bash
$ bash import.sh
```


## Running Tasks

Navigate to `run_tasks` and run the following command.
It will execute the whole tasks (i.e., Task 0 to Task 16).

```bash
bash Run_all.sh
```