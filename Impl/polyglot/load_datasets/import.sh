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

