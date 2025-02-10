sed '1,5d;$d' california-points.geojson | sed '$d' | sed 's/,\n/\n/g' > 1.json

sed '1,5d;$d' california-multipolygons.geojson | sed '$d' > 3.json

sed -n "3000001, \$p" 3.json | sed 's/,\n/\n/g' > 3_2.json &
sed -n '1,3000001p' 3.json | sed 's/,\n/\n/g' | sed -n '1,3000000p' > 3_3.json &

wait

cat 1.json 3_2.json 3_3.json Roadnode.json > site2.json

rm 1.json 3.json 3_2.json 3_3.json *.geojson
