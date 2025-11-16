#!/bin/bash

SCIDB_CONTAINER="m2bench-scidb"

echo "========================================"
printf "\tDisaster & Safety\n"
echo "========================================"

echo "==== Import data to MySQL ===="
mysql < ./disaster/create_table.sql
mysql --local-infile < ./disaster/load_table.sql

echo "==== Import data to MongoDB ===="
bash ./disaster/load_json.sh

echo "==== Import data to SciDB ===="
echo "You can ignore errors during the import process if the final arrays are created."
SCIDB_DATA=$(pwd)/../../../Datasets/disaster/array/
if command -v iquery &> /dev/null
then
    mkdir -p /tmp/m2bench
    tail -n +2 $SCIDB_DATA/Finedust_idx.csv > /tmp/m2bench/Finedust_idx.csv
    bash ./disaster/load_array.sh

    iquery -aq "load_library('linear_algebra')"
    iquery -aq "load_library('dense_linear_algebra')"
else
    # echo -n "iquery not found. Please type the name of the SciDB container: "
    # read SCIDB_CONTAINER
    echo "iquery not found. Using default SciDB container: $SCIDB_CONTAINER"
    
    sudo docker exec -it $SCIDB_CONTAINER mkdir -p /tmp/m2bench
    sudo docker cp $SCIDB_DATA/Finedust_idx.csv $SCIDB_CONTAINER:/tmp/m2bench/Finedust_idx_with_header.csv
    sudo docker exec -it $SCIDB_CONTAINER bash -c "tail -n +2 /tmp/m2bench/Finedust_idx_with_header.csv > /tmp/m2bench/Finedust_idx.csv"
    sudo docker cp ./disaster/load_array.sh $SCIDB_CONTAINER:/tmp/m2bench
    sudo docker exec -it $SCIDB_CONTAINER bash /tmp/m2bench/load_array.sh

    sudo docker exec -it $SCIDB_CONTAINER iquery -aq "load_library('linear_algebra')"
    sudo docker exec -it $SCIDB_CONTAINER iquery -aq "load_library('dense_linear_algebra')"
fi

