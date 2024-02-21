DATASET_PATH=../../../Datasets
mongoimport --db Disaster  --collection Site  --username $USERNAME --file $DATASET_PATH/disaster/json/Site.json --drop --bypassDocumentValidation
mongo removeInvalidDoc.js

