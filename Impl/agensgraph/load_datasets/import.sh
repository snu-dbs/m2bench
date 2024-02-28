DATA_PATH=$(pwd)/../../../Datasets/
mkdir -p /tmp/m2bench/
ln -s $DATA_PATH/ecommerce /tmp/m2bench/ 2>&1 >& /dev/null
ln -s $DATA_PATH/healthcare /tmp/m2bench/ 2>&1 >& /dev/null 
ln -s $DATA_PATH/disaster /tmp/m2bench/ 2>&1 >& /dev/null

echo "==== E-commerce ===="
cd ecommerce
bash load_ecommerce.sh
cd ..

echo "==== Healthcare ===="
cd healthcare
bash load_healthcare.sh
cd ..

echo "==== Disaster & Safety ===="
cd disaster
bash load_disaster.sh
cd ..