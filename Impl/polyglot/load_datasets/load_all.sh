#!/bin/bash

SCIDB_CONTAINER="m2bench-scidb"

bash load_ecommerce.sh
bash load_healthcare.sh
bash load_disaster.sh
