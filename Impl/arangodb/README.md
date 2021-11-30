# Run M2Bench in ArangoDB 
- This document explain how to run M2Bench in ArangoDB

## Install ArangoDB 
- https://www.arangodb.com/docs/stable/getting-started-installation.html
- We have tested the version 3.7 ArangoDB 

## LOAD Dataset into ArangoDB 
- Go to `load_dataset` folder
- Open `load_all.sh` file  and fill **DATASET_PATH**, **USERNAME** and **PASSWORD** fields.
- **DATASET_PATH** is the path to the m2bench datasets (ex. ~/m2bench/Datasets )
- **USERNAME** is the username to access ArangoDB.
- **PASSWORD** is the password to access ArangoDB.
- Run `./load_all.sh`

## Run M2Bench Tasks
- Go to `run_tasks` folder 
- Open `run_all.sh` file and fill **USERNAME** and **PASSWORD** fields.
- **USERNAME** is the username to access ArangoDB.
- **PASSWORD** is the password to access ArangoDB.
- Run `./run_all.sh`

