import os
import pandas as pd
import json
import time
import math
import sys
import orjson

from multiprocessing import Process, Manager, Value, Lock, Pool

def p1(data_dirpath):
    os.system("ogr2ogr -f 'GeoJSON' california-points.geojson " + data_dirpath + "/california-latest.osm -sql \"SELECT * FROM points WHERE ST_IsValid(geometry)\" -dialect SQLite > /dev/null 2>&1")

def p2(data_dirpath):
    os.system("ogr2ogr -f 'GeoJSON' california-multipolygons.geojson " + data_dirpath + "/california-latest.osm -sql \"SELECT * FROM multipolygons WHERE ST_IsValid(geometry)\" -dialect SQLite > /dev/null 2>&1")
    
def ogr_background(data_dirpath):
    os.system('cp Disaster/osmconf.ini .')

    with Pool(processes=2) as pool:
        procs2 = []
        procs2.append(pool.apply_async(p1, (data_dirpath, )))
        procs2.append(pool.apply_async(p2, (data_dirpath, )))
        for p in procs2: p.get()

    os.system('rm osmconf.ini')


def site_gen(outdir):
    df = pd.read_csv(outdir+'/../property_graph/original_roadnode.csv')

    with open('Roadnode.json', 'w') as json_file:
        for idx, row in df.iterrows():
            final = dict()

            geometry = dict()
            geometry["type"] = "Point"
            geometry["coordinates"] = [row['longitude'], row['latitude']]

            properties = dict()
            properties["roadnode"] = "yes"

            final["geometry"] = geometry
            final["properties"] = properties
            final["site_id"] = int(row['site_id'])

            json.dump(final, json_file)
            json_file.write('\n')

    os.system("./Disaster/site_convert.sh")

    with open('site2.json', 'r') as site_file:
        with open(outdir+'Site.json', 'w') as json_file:
            site_id = 0
            for line in site_file:
                properties = dict()
                line = line.strip()
                line = line[:-1] if line[-1] == ',' else line
                site_json = orjson.loads(line)
                
                # a json from roadnode has site_id, so we need to preserve site_id in such case
                if 'site_id' not in site_json:
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

                # json.dump(site_json, json_file)
                res = orjson.dumps(site_json).decode('utf-8')
                json_file.write(res + '\n')

    os.system("rm site2.json Roadnode.json")
    os.system("rm "+outdir+"/../property_graph/original_roadnode.csv")
