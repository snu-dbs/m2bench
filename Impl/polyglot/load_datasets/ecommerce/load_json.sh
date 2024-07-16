DATASET_PATH=../../../Datasets
mongoimport --db Ecommerce  --collection Order --drop --file $DATASET_PATH/ecommerce/json/order.json
mongoimport --db Ecommerce  --collection Review --drop --file $DATASET_PATH/ecommerce/json/review.json

mongo Ecommerce ./ecommerce/create_json_index.js