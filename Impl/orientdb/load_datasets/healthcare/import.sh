# create working directory
cp -r $PWD/healthcare/table $PWD/healthcare/_table
cp -r $PWD/healthcare/json $PWD/healthcare/_json
cp -r $PWD/healthcare/property_graph $PWD/healthcare/_property_graph

# change path of load configurations to absolute path
# table
sed -i "s+__TBAPATH+$PWD+" ./healthcare/_table/load_patient.json
sed -i "s+__TBAPATH+$PWD+" ./healthcare/_table/load_prescription.json
sed -i "s+__TBAPATH+$PWD+" ./healthcare/_table/load_diagnosis.json

# json
sed -i "s+__TBAPATH+$PWD+" ./healthcare/_json/load_drug.json

# graph
sed -i "s+__TBAPATH+$PWD+" ./healthcare/_property_graph/load_disease.json
sed -i "s+__TBAPATH+$PWD+" ./healthcare/_property_graph/load_isa.json

# convert json to a list of documents
# drug
echo "[" > $PWD/../../../Datasets/healthcare/json/_drug_list.json
sed '$!s/$/,/' $PWD/../../../Datasets/healthcare/json/drug.json >> $PWD/../../../Datasets/healthcare/json/_drug_list.json
echo "]" >> $PWD/../../../Datasets/healthcare/json/_drug_list.json

# import
# table
sudo /opt/orientdb/bin/oetl.sh $PWD/healthcare/_table/load_patient.json
sudo /opt/orientdb/bin/oetl.sh $PWD/healthcare/_table/load_prescription.json
sudo /opt/orientdb/bin/oetl.sh $PWD/healthcare/_table/load_diagnosis.json

# json
sudo /opt/orientdb/bin/oetl.sh $PWD/healthcare/_json/load_drug.json

# graph
sudo /opt/orientdb/bin/oetl.sh $PWD/healthcare/_property_graph/load_disease.json
sudo /opt/orientdb/bin/oetl.sh $PWD/healthcare/_property_graph/load_isa.json

# remove working directory
rm -rf $PWD/healthcare/_table $PWD/healthcare/_json $PWD/healthcare/_property_graph

# remove temp json
rm $PWD/../../../Datasets/healthcare/json/_drug_list.json
