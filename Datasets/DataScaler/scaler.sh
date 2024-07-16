
DATASET_PATH=../
SCHEMA_PATH=./
SF=$1

sed -i 's/\r$//g' ../ecommerce/property_graph/person_interestedIn_tag.csv

./build/scaler $SF $DATASET_PATH $SCHEMA_PATH
echo "3. Scale up Disaster Datasets ..."
echo "Scale GPS ..."
python ./disaster_scaler/gps.py $SF ../raw_datasets/gps/  ../disaster/table/

echo "Scale Finedust ..."
python ./disaster_scaler/finedust_idx.py $SF ../raw_datasets/finedust/  ../disaster/array/

echo "Disaster Done"





