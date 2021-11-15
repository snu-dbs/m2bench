DATASET_PATH=
USERNAME=
PASSWORD= 



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

arangoimp --overwrite true --file "$DATASET_PATH/disaster/array/finedust_idx.csv" --type csv --collection "Finedust_idx" --server.username $USERNAME  --server.password "$PASSWORD" --create-collection true --threads 4 --server.database Disaster

