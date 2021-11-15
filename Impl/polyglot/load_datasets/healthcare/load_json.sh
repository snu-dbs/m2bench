USERNAME=
DATASET_PATH=
mongoimport --db Healthcare  --collection drug  --username $USERNAME  --drop --file $DATASET_PATH/healthcare/json/drug.json  --jsonArray;

