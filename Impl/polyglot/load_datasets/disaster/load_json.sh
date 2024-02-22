DATASET_PATH=../../../Datasets
mongo Disaster --eval "db.dropDatabase()" 
mongo Disaster --eval "db.Site.createIndex({geometry:'2dsphere'})" 

mongoimport --db Disaster --collection Site --file $DATASET_PATH/disaster/json/Site.json --drop --numInsertionWorkers $(($(nproc)/2))