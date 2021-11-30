# Run M2Bench in ArangoDB 
- This document explain how to run m2bench in ArangoDB

## Install ArangoDB 
- https://www.arangodb.com/docs/stable/getting-started-installation.html
- We have tested **ArangoDB v 3.7 **

## LOAD Dataset into ArangoDB 
- Go to `load_dataset` folder
- open `load_all.sh` file  and fill the **DATASET_PATH**, **USERNAME** and **PASSWORD**.
- **DATASET_PATH** is the path to the m2bench datasets (ex. ~/m2bench/Datasets )
- **USERNAME** is the username to access ArangoDB.
- **PASSWORD** is the password to access ArangoDB.
- Run `./load_all.sh`
