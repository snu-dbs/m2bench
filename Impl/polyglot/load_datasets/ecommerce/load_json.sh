USERNAME=
DATASET_PATH=
mongoimport --db Ecommerce  --collection Order  --username $USERNAME  --drop --file $DATASET_PATH/ecommerce/json/order.json
mongoimport --db Ecommerce  --collection Review  --username $USERNAME  --drop --file $DATASET_PATH/ecommerce/json/review.json

