agens healthcare -c "CREATE EXTENSION postgis;"

agens healthcare -f ./create_table.sql
agens healthcare -f ./load_table.sql

agens healthcare -f ./create_json.sql
agens healthcare -f ./load_json.sql

agens healthcare -f ./create_graph.sql
agens healthcare -f ./load_graph.sql
