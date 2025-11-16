#!/bin/bash

SCIDB_CONTAINER="m2bench-scidb"

echo "========================================"
printf "\tE-commerce\n"
echo "========================================"

echo "==== Import data to MySQL ===="
mysql < ./ecommerce/create_table.sql
mysql --local-infile < ./ecommerce/load_table.sql

mysql < ./ecommerce/create_graph.sql
mysql --local-infile < ./ecommerce/load_graph.sql

echo "==== Import data to MongoDB ===="
bash ./ecommerce/load_json.sh

