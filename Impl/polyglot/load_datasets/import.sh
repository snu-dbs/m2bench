#!/bin/bash

# E-commerce
echo "========================================"
printf "\tE-commerce\n"
echo "========================================"

echo "==== Import data to MySQL ===="
mysql < ./ecommerce/create_table.sql
mysql --local-infile < ./ecommerce/load_data.sql

echo "==== Import data to MongoDB ===="
bash ./ecommerce/load_json.sh

echo "==== Import data to Neo4J ===="
# make symbolic links to the data files
NEO4J_DATA=$(pwd)/../../../Datasets/ecommerce/property_graph/
for f in $(ls $NEO4J_DATA); do
    ln -s $NEO4J_DATA/$f /var/lib/neo4j/import > /dev/null 2>&1
done
# import data
cypher-shell --fail-at-end -f ./ecommerce/load_graph.cypher


# Healthcare
echo "========================================"
printf "\tHealthcare\n"
echo "========================================"

echo "==== Import data to MySQL ===="
mysql < ./healthcare/create_table.sql
mysql --local-infile < ./healthcare/load_data.sql

echo "==== Import data to MongoDB ===="
bash ./healthcare/load_json.sh

echo "==== Import data to Neo4J ===="
# make symbolic links to the data files
NEO4J_DATA=$(pwd)/../../../Datasets/healthcare/property_graph/
for f in $(ls $NEO4J_DATA); do
    ln -s $NEO4J_DATA/$f /var/lib/neo4j/import > /dev/null 2>&1
done

# import node
cypher-shell --fail-at-end -f ./healthcare/load_graph.cypher


# Disaster & Safety
echo "========================================"
printf "\tDisaster & Safety\n"
echo "========================================"

echo "==== Import data to MySQL ===="
mysql < ./disaster/create_table.sql
mysql --local-infile < ./disaster/load_data.sql

echo "==== Import data to MongoDB ===="
bash ./disaster/load_json.sh

echo "==== Import data to Neo4J ===="
# make symbolic links to the data files
NEO4J_DATA=$(pwd)/../../../Datasets/disaster/property_graph/
for f in $(ls $NEO4J_DATA); do
    ln -s $NEO4J_DATA/$f /var/lib/neo4j/import > /dev/null 2>&1
done
# import data
cypher-shell --fail-at-end -f ./disaster/load_graph.cypher

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
    echo -n "iquery not found. Please type the name of the SciDB container: "
    read SCIDB_CONTAINER
    docker exec -it $SCIDB_CONTAINER mkdir -p /tmp/m2bench
    docker cp $SCIDB_DATA/Finedust_idx.csv $SCIDB_CONTAINER:/tmp/m2bench/Finedust_idx_with_header.csv
    docker exec -it $SCIDB_CONTAINER bash -c "tail -n +2 /tmp/m2bench/Finedust_idx_with_header.csv > /tmp/m2bench/Finedust_idx.csv"
    docker cp ./disaster/load_array.sh $SCIDB_CONTAINER:/tmp/m2bench
    docker exec -it $SCIDB_CONTAINER bash /tmp/m2bench/load_array.sh

    docker exec -it $SCIDB_CONTAINER iquery -aq "load_library('linear_algebra')"
    docker exec -it $SCIDB_CONTAINER iquery -aq "load_library('dense_linear_algebra')"
fi