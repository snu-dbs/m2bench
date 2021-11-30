# Run M2Bench in AgensGraph 
- This document explain how to run M2Bench in ArangoDB

## Install AgensGraph 
- https://bitnine.net/documentations/quick-guide-2-0.html#installation
- We have tested version 2.2 AgensGraph

## LOAD Dataset into AgensGraph 
- Go to `load_dataset/[ecommerce|healthcare|disaster]` folder
- Run following scripts to load datasets in each scenario: `load_ecommerce.sh`, `load_healthcare.sh`, `load_disaster.sh`

## Run M2Bench Tasks
- Go to `run_tasks` folder 
- Run `./run_all.sh`

