DATASET_PATH=../../../Datasets

mongosh Disaster --eval "db.Site.createIndex({ geometry: '2dsphere' })"
mongoimport --db Disaster --collection Site --file $DATASET_PATH/disaster/json/Site.json --numInsertionWorkers $(($(nproc) / 2))

mongosh Disaster --eval "db.Site_centroid.createIndex({ centroid: '2dsphere' })"
mongoimport --db Disaster --collection Site_centroid --file $DATASET_PATH/disaster/json/Site_centroid.json --numInsertionWorkers $(($(nproc) / 2))

mongosh Disaster ./disaster/create_json_index.js
