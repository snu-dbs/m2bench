\c healthcare
  
\COPY Drug (data) FROM program 'sed -e ''s/\\/\\\\/g'' ../../../../Datasets/healthcare/json/drug.json';
