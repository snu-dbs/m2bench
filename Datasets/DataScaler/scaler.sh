
DATASET_PATH=../
SCHEMA_PATH=./
SF=$1
./build/scaler $SF $DATASET_PATH $SCHEMA_PATH
echo "3. Scale up Ecommerce Datasets ..."
echo "Scale GPS ..."
python ./disaster_scaler/gps.py $SF ../raw_datasets/gps_dataset/  ../disaster/table/

echo "Scale Finedust ..."
python ./disaster_scaler/finedust_idx.py $SF ../raw_datasets/finedust_dataset/  ../disaster/array/

echo "Disaster Done"





