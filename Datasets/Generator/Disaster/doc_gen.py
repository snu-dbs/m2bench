import os
import pandas as pd
import json

def site_gen(data_dirpath, outdir):
    os.system('cp Disaster/osmconf.ini .')

    os.system("ogr2ogr -f 'GeoJSON' california-points.geojson " + data_dirpath + "california-latest.osm points")
    os.system("ogr2ogr -f 'GeoJSON' california-multipolygons.geojson " + data_dirpath + "california-latest.osm multipolygons")

    os.system('rm osmconf.ini')

    os.system("./Disaster/site_convert.sh")

    df = pd.read_csv(outdir+'/../property_graph/original_roadnode.csv')

    with open('Roadnode.json', 'w') as json_file:
        for idx, row in df.iterrows():
            final = dict()

            geometry = dict()
            geometry["type"] = "Point"
            geometry["coordinates"] = ['{:.6f}'.format(row['longitude']), '{:.6f}'.format(row['latitude'])]

            properties = dict()
            properties["roadnode"] = "yes"

            final["geometry"] = geometry
            final["properties"] = properties

            json.dump(final, json_file)
            json_file.write('\n')

    os.system("cat site.json Roadnode.json > site2.json")

    with open('site2.json', 'r') as site_file:
        with open(outdir+'Site.json', 'w') as json_file:
            site_id = 0
            for line in site_file:
                properties = dict()
                line = line.strip()
                line = line[:-1] if line[-1] == ',' else line
                site_json = json.loads(line)
                site_json['site_id'] = site_id
                site_id += 1
                try:
                    if site_json['properties']['building'] is not None:
                        properties['type'] = 'building'
                        properties['description'] = site_json['properties']['building']
                except:
                    pass
                try:
                    if site_json['properties']['roadnode'] is not None:
                        properties['type'] = 'roadnode'
                        properties['description'] = site_json['properties']['roadnode']
                except:
                    pass
                try:
                    if site_json['properties']['name'] is not None:
                        properties['name'] = site_json['properties']['name']
                except:
                    pass
                try:
                    if site_json['properties']['address'] is not None:
                        properties['address'] = site_json['properties']['address']
                except:
                    pass
                try:
                    properties['type']
                except:
                    properties['type'] = 'etc'
                site_json['properties'] = properties

                json.dump(site_json, json_file)
                json_file.write('\n')

    os.system("rm site2.json site.json Roadnode.json")
    os.system("rm "+outdir+"/../property_graph/original_roadnode.csv")
