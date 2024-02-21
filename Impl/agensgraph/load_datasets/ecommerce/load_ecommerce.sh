agens ecommerce -c "CREATE EXTENSION postgis;"

agens ecommerce -f ./create_table.sql
agens ecommerce -f ./load_table.sql

agens ecommerce -f ./create_json.sql
agens ecommerce -f ./load_json.sql

agens ecommerce -f ./create_graph.sql
agens ecommerce -f ./load_graph.sql
