
agens disaster -c "CREATE EXTENSION IF NOT EXISTS postgis;"

agens disaster -f ./create_table.sql
agens disaster -f ./load_table.sql

agens disaster -f ./create_json.sql
agens disaster -f ./load_json.sql

agens disaster -f ./create_graph.sql
agens disaster -f ./load_graph.sql

agens disaster -f ./create_array.sql
agens disaster -f ./load_array.sql
