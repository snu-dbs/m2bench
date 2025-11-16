#!/bin/bash

SCIDB_CONTAINER="m2bench-scidb"

echo "========================================"
printf "\tHealthcare\n"
echo "========================================"

echo "==== Import data to MySQL ===="
mysql < ./healthcare/create_table.sql
mysql --local-infile < ./healthcare/load_table.sql

echo "==== Import data to MongoDB ===="
bash ./healthcare/load_json.sh

