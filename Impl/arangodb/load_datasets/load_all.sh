DATASET_PATH=../../../Datasets
USERNAME=
PASSWORD=

./ecommerce/load_ecommerce.sh $DATASET_PATH $USERNAME $PASSWORD
./healthcare/load_healthcare.sh $DATASET_PATH $USERNAME $PASSWORD
./disaster/load_disaster.sh $DATASET_PATH $USERNAME $PASSWORD
