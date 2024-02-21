\c healthcare
  
COPY Drug (data) FROM program 'sed -e ''s/\\/\\\\/g'' /tmp/m2bench/healthcare/json/drug.json';
