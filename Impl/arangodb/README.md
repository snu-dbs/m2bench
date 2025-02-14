# Run M2Bench in ArangoDB 
- This document explains how to run M2Bench in ArangoDB
- If you have set **USERNAME** to `root` and **PASSWORD** to `m2bench`, you can use the scripts as-is. Otherwise, update them accordingly.

## Load Dataset into ArangoDB 
1. Go to `load_dataset` folder
2. Open `load_all.sh` file and fill **DATASET_PATH**, **USERNAME** and **PASSWORD** fields.
    - **DATASET_PATH** is the path to the m2bench datasets (`/Datasets`).
    - **USERNAME** is the username to access ArangoDB.
    - **PASSWORD** is the password to access ArangoDB.
6. Run `./load_all.sh`

## Run M2Bench Tasks
1. Go to `run_tasks` folder 
2. Open `run_all.sh` file and fill **USERNAME** and **PASSWORD** fields.
    - **USERNAME** is the username to access ArangoDB.
    - **PASSWORD** is the password to access ArangoDB.
3. Run `./run_all.sh`

If you encounter a timeout error, please open the `run_all.sh` file and increase the value of the `--server.request-timeout` option.
