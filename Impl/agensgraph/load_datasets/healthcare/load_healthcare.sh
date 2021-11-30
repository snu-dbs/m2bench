DATASET_PATH=

NODE="'$DATASET_PATH/healthcare/property_graph/Disease_network_nodes.csv'"
EDGE="'$DATASET_PATH/healthcare/property_graph/Disease_network_edges.csv'"

agens -f ./create_table.sql
agens -f ./load_table.sql


agens -f ./create_json.sql
agens -f ./load_json.sql


agens -v n=$NODE -v  e=$EDGE -f  ./create_graph.sql
agens -f ./load_graph.sql
