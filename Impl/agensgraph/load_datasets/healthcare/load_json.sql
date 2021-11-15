\c healthcare
  
COPY Drug (data) FROM program 'sed -e ''s/\\/\\\\/g'' /home/mxmdb/m2bench/data/healthcare/json/drug.json';
