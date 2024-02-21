DATASET_PATH=../../../Datasets
mongoimport --db Healthcare  --collection drug --drop --file $DATASET_PATH/healthcare/json/drug.json  --jsonArray;
