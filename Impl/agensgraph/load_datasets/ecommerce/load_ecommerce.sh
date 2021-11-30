DATASET_PATH=



PERSON="'$DATASET_PATH/ecommerce/property_graph/person_node.csv'"
PFP="'$DATASET_PATH/ecommerce/property_graph/person_follows_person.csv'"
HASH="'$DATASET_PATH/ecommerce/property_graph/hashtag_node_SF1.csv'"
PFH="'$DATASET_PATH/ecommerce/property_graph/person_interestedIn_tag.csv'"

agens -f ./create_table.sql
agens -f ./load_table.sql


agens -f ./create_json.sql
agens -f ./load_json.sql


agens -v p=$PERSON -v pfp=$PFP -v  h=$HASH -v  pih=$PFH -f ./create_graph.sql
agens -f ./load_graph.sql
