DATASET_PATH=
USERNAME=
PASSWORD=

# LOAD TABLE

arangoimp --overwrite true --file "$DATASET_PATH/healthcare/table/Diagnosis.csv" --type csv --collection "Diagnosis" --server.username $USERNAME  --server.password "$PASSWORD" --create-collection true --threads 4 --server.database Healthcare

arangoimp --overwrite true --file "$DATASET_PATH/healthcare/table/Patient.csv" --type csv --collection "Patient" --server.username $USERNAME  --server.password "$PASSWORD" --create-collection true --threads 4 --server.database Healthcare

arangoimp --overwrite true --file "$DATASET_PATH/healthcare/table/Prescription.csv" --type csv --collection "Prescription" --server.username $USERNAME  --server.password "$PASSWORD" --create-collection true --threads 4 --server.database Healthcare


# LOAD JSON

arangoimp --overwrite true --file "$DATASET_PATH/healthcare/json/drug.json" --batch-size 536870912 --type json --collection "Drug" --server.username $USERNAME  --server.password "$PASSWORD" --create-collection true --threads 4 --server.database Healthcare


# LOAD GRAPH


arangoimp --overwrite true --file "$DATASET_PATH/healthcare/property_graph/Disease_network_nodes.csv" --type csv --translate "disease_id=_key" --collection "Disease_network_nodes" --server.username $USERNAME  --server.password "$PASSWORD" --create-collection true --threads 4 --server.database Healthcare

arangoimp --overwrite true --file "$DATASET_PATH/healthcare/property_graph/Disease_network_edges.csv" --type csv --translate "source_id=_from" --translate "destination_id=_to" --collection "Disease_network_edges" --from-collection-prefix Disease_network_nodes --to-collection-prefix Disease_network_nodes --server.username $USERNAME  --server.password "$PASSWORD" --create-collection true --create-collection-type edge --threads 4 --server.database Healthcare

