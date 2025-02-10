\c healthcare;

COPY Drug (data) 
FROM PROGRAM 'sed -e ''s/\\/\\\\/g'' /tmp/m2bench/healthcare/json/drug.json';
