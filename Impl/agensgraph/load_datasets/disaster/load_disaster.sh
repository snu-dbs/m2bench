DATASET_PATH=

NODE="'$DATASET_PATH/disaster/property_graph/Roadnode.csv'"
EDGE="'$DATASET_PATH/disaster/property_graph/Road.csv'"


agens -f ./create_table.sql
agens -f ./load_table.sql


agens -f ./create_json.sql
agens -f ./load_json.sql


agens -v n=$NODE =v e=$EDGE -f ./create_graph.sql
agens -f ./load_graph.sql

agens -f ./create_array.sql
agens -f ./load_array.sql
