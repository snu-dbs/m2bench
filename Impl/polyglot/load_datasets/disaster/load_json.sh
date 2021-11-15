USERNAME=
DATASET_PATH=
mongoimport --db Disaster  --collection Site  --username $USERNAME --file $DATASET_PATH/disaster/json/original_site.json --drop --bypassDocumentValidation
mongo removeInvalidDoc.js

