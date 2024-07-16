# create working directory
cp -r $PWD/ecommerce/table $PWD/ecommerce/_table
cp -r $PWD/ecommerce/json $PWD/ecommerce/_json
cp -r $PWD/ecommerce/property_graph $PWD/ecommerce/_property_graph

# change path of load configurations to absolute path
# table
sed -i "s+__TBAPATH+$PWD+" $PWD/ecommerce/_table/load_customer.json
sed -i "s+__TBAPATH+$PWD+" $PWD/ecommerce/_table/load_product.json
sed -i "s+__TBAPATH+$PWD+" $PWD/ecommerce/_table/load_brand.json

# json
sed -i "s+__TBAPATH+$PWD+" $PWD/ecommerce/_json/load_order.json
sed -i "s+__TBAPATH+$PWD+" $PWD/ecommerce/_json/load_review.json

# graph
sed -i "s+__TBAPATH+$PWD+" $PWD/ecommerce/_property_graph/load_person.json
sed -i "s+__TBAPATH+$PWD+" $PWD/ecommerce/_property_graph/load_hashtag.json

sed -i "s+__TBAPATH+$PWD+" $PWD/ecommerce/_property_graph/load_follows.json
sed -i "s+__TBAPATH+$PWD+" $PWD/ecommerce/_property_graph/load_interestedin.json

# convert json to a list of documents
# order
echo "[" > $PWD/../../../Datasets/ecommerce/json/_order_list.json
sed '$!s/$/,/' $PWD/../../../Datasets/ecommerce/json/order.json >> $PWD/../../../Datasets/ecommerce/json/_order_list.json
echo "]" >> $PWD/../../../Datasets/ecommerce/json/_order_list.json
# review
echo "[" > $PWD/../../../Datasets/ecommerce/json/_review_list.json
sed '$!s/$/,/' $PWD/../../../Datasets/ecommerce/json/review.json >> $PWD/../../../Datasets/ecommerce/json/_review_list.json
echo "]" >> $PWD/../../../Datasets/ecommerce/json/_review_list.json

# import
# table
sudo /opt/orientdb/bin/oetl.sh $PWD/ecommerce/_table/load_customer.json
sudo /opt/orientdb/bin/oetl.sh $PWD/ecommerce/_table/load_product.json
sudo /opt/orientdb/bin/oetl.sh $PWD/ecommerce/_table/load_brand.json

# json
sudo /opt/orientdb/bin/oetl.sh $PWD/ecommerce/_json/load_order.json
sudo /opt/orientdb/bin/oetl.sh $PWD/ecommerce/_json/load_review.json

# graph
sudo /opt/orientdb/bin/oetl.sh $PWD/ecommerce/_property_graph/load_person.json
sudo /opt/orientdb/bin/oetl.sh $PWD/ecommerce/_property_graph/load_hashtag.json

sudo /opt/orientdb/bin/oetl.sh $PWD/ecommerce/_property_graph/load_follows.json
sudo /opt/orientdb/bin/oetl.sh $PWD/ecommerce/_property_graph/load_interestedin.json

# remove working directory
rm -rf $PWD/ecommerce/_table $PWD/ecommerce/_json $PWD/ecommerce/_property_graph

# remove temp json
rm $PWD/../../../Datasets/ecommerce/json/_order_list.json
rm $PWD/../../../Datasets/ecommerce/json/_review_list.json
