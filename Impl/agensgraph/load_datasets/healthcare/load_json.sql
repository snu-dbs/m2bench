\c healthcare
  
COPY Drug (data) FROM program 'sed -e ''s/\\/\\\\/g'' /healthcare/json/drug.json';
