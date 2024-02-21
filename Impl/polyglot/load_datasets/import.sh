#! /bin/bash

# FIXME:
SF=$1

# E-commerce
echo "==== E-commerce ===="
echo "Import data to MySQL"
mysql < ./ecommerce/create_table.sql
mysql --local-infile < ./ecommerce/load_data.sql

echo "Import data to MongoDB"
bash ./ecommerce/load_json.sh

echo "Import data to Neo4J"
# make symbolic links to the data files
NEO4J_DATA=$(pwd)/../../../Datasets/ecommerce/property_graph/
for f in $(ls $NEO4J_DATA); do
    ln -s $NEO4J_DATA/$f /var/lib/neo4j/import
done
# import data
cypher-shell -f ./ecommerce/load_graph.cypher


# Healthcare
echo "==== Healthcare ===="
echo "Import data to MySQL"
mysql < ./healthcare/create_table.sql
mysql --local-infile < ./healthcare/load_data.sql

echo "Import data to MongoDB"
bash ./ecommerce/load_json.sh

echo "Import data to Neo4J"
# make symbolic links to the data files
NEO4J_DATA=$(pwd)/../../../Datasets/healthcare/property_graph/
for f in $(ls $NEO4J_DATA); do
    ln -s $NEO4J_DATA/$f /var/lib/neo4j/import
done
# import data
cypher-shell -f ./healthcare/load_graph.cypher



# Disaster & Safety
echo "==== Disaster & Safety ===="
echo "Import data to MySQL"
mysql < ./disaster/create_table.sql
mysql --local-infile < ./disaster/load_data.sql

echo "Import data to MongoDB"
bash ./disaster/load_json.sh

echo "Import data to Neo4J"
# make symbolic links to the data files
NEO4J_DATA=$(pwd)/../../../Datasets/disaster/property_graph/
for f in $(ls $NEO4J_DATA); do
    ln -s $NEO4J_DATA/$f /var/lib/neo4j/import
done
# import data
cypher-shell -f ./disaster/load_graph.cypher

echo "Import data to SciDB"
# TODO: Check
SCIDB_DATA=$(pwd)/../../../Datasets/disaster/array/
if command -v iquery &> /dev/null
then
    mkdir -p /tmp/m2bench
    ln -s $SCIDB_DATA/Finedust_idx.csv /tmp/m2bench
    bash load_array.sh
else
    echo "iquery not found. Please type the name of the SciDB container: "
    read SCIDB_CONTAINER
    docker exec -it $SCIDB_CONTAINER mkdir -p /tmp/m2bench
    docker cp $SCIDB_DATA/Finedust_idx.csv $SCIDB_CONTAINER:/tmp/m2bench
    docker cp disaster/load_array.sh $SCIDB_CONTAINER:~/
    docker exec -it $SCIDB_CONTAINER bash ~/load_array.sh
fi