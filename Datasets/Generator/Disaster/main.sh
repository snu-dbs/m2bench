python ./Disaster/main.py

# site centroid
echo "Generating site_centroid"

SITE_DIR=../disaster/json/
let "t=$(nproc)/2"
PREFIX='_Site_split_'
split -n l/$t $SITE_DIR/Site.json $PREFIX

for item in $(ls $PREFIX*); do
    python ./Disaster/site_centroid_gen.py $item $item".json" &
done

wait
cat $PREFIX*.json > $SITE_DIR/Site_centroid.json
