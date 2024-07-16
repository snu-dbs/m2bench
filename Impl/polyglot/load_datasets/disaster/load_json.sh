DATASET_PATH=../../../Datasets
mongo Disaster --eval "db.dropDatabase()" 

mongo Disaster --eval "db.Site.createIndex({geometry:'2dsphere'})" 
mongoimport --db Disaster --collection Site --file $DATASET_PATH/disaster/json/Site.json --numInsertionWorkers $(($(nproc)/2))

mongo Disaster --eval "db.Site_centroid.createIndex({centroid:'2dsphere'})" 
mongoimport --db Disaster --collection Site_centroid --file $DATASET_PATH/disaster/json/Site_centroid.json --numInsertionWorkers $(($(nproc)/2))

mongo Disaster ./disaster/create_json_index.js