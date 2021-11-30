DATASET_PATH=$1
USERNAME=$2
PASSWORD=$3



# LOAD TABLE

arangoimp --overwrite true  --file "$DATASET_PATH/disaster/table/Earthquake.csv" --type csv --collection "Earthquake" --server.username $USERNAME  --server.password "$PASSWORD" --create-collection true --threads 4 --server.database Disaster

arangoimp --overwrite true --file "$DATASET_PATH/disaster/table/Gps.csv" --type csv --collection "Gps" --server.username $USERNAME  --server.password "$PASSWORD" --create-collection true --threads 4 --server.database Disaster

arangoimp --overwrite true --file "$DATASET_PATH/disaster/table/Shelter.csv" --type csv --separator "|" --collection "Shelter" --server.username $USERNAME  --server.password "$PASSWORD" --create-collection true --threads 4 --server.database Disaster


#LOAD JSON

arangoimp --overwrite true --file "$DATASET_PATH/disaster/json/Site.json" --type json --collection "Site" --server.username $USERNAME  --server.password "$PASSWORD" --create-collection true --threads 4 --server.database Disaster

#LOAD GRAPH

arangoimp --overwrite true --file "$DATASET_PATH/disaster/property_graph/Roadnode.csv" --type csv --translate "roadnode_id=_key" --collection "Roadnode" --server.username $USERNAME  --server.password "$PASSWORD" --create-collection true --threads 4 --server.database Disaster

arangoimp --overwrite true --file "$DATASET_PATH/disaster/property_graph/Road.csv" --type csv --translate "from=_from" --translate "to=_to" --collection "Road" --from-collection-prefix Roadnode --to-collection-prefix Roadnode --server.username $USERNAME  --server.password "$PASSWORD" --create-collection true --create-collection-type edge --threads 4 --server.database Disaster

#LOAD ARRAY

arangoimp --overwrite true --file "$DATASET_PATH/disaster/array/Finedust_idx.csv" --type csv --collection "Finedust_idx" --server.username $USERNAME  --server.password "$PASSWORD" --create-collection true --threads 4 --server.database Disaster


arangosh --server.database Disaster --server.username $USERNAME --server.password $PASSWORD --javascript.execute  $DATASET_PATH/../Impl/arangodb/load_datasets/disaster/create_index.js

