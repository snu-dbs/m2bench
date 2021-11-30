# Run M2Bench in AgensGraph 
- This document explain how to run M2Bench in ArangoDB

## Install AgensGraph 
- https://bitnine.net/documentations/quick-guide-2-0.html#installation
- We have tested version 2.2 AgensGraph

## LOAD Dataset into AgensGraph 
- Go to `load_dataset/[ecommerce|healthcare|disaster]` folder.
- open following scripts : `load_ecommerce.sh`, `load_healthcare.sh`, `load_disaster.sh`
- fill the DATASET_PATH with the **absolute path** to the m2bench datasets. ex) `DATASET_PATH=/home/userX/m2bench/Datasets`
- Run the scripts. ex) `./load_ecommerce.sh`


## Run M2Bench Tasks
- Go to `run_tasks` folder 
- Run `./run_all.sh`

