import sys
import geopandas as gpd
import json

from multiprocessing import Process, Manager, Value, Lock, Pool
from math import ceil
from shapely.geometry import shape

def bulk_geometry_to_centroid(geometry_list):
    df = gpd.GeoDataFrame({'geometry': [shape(i) for i in geometry_list]}, crs='EPSG:4326')
    df['centroid'] = df.to_crs("+proj=cea").centroid.to_crs(df.crs)
    df['geometry'] = df['centroid']
    del df['centroid']

    json_list = json.loads(df.to_json(na='drop'))['features']
    return [x['geometry'] for x in json_list]

def write_only_centroid(out_f, original_json_list, centroid_json_list):
    for i, x in enumerate(original_json_list):
        x['centroid'] = centroid_json_list[i]
        del x['geometry']

        out_f.write(json.dumps(x) + '\n')

def site_centroid_gen(input_filename, output_filename):
    in_f = open(input_filename, 'r')
    out_f = open(output_filename, 'w')

    buf = []
    buf_size = 10000
    for _, line in enumerate(in_f):
        buf.append(json.loads(line))
        if len(buf) < buf_size:
            continue

        geometry_list = [x['geometry'] for x in buf]
        centroid_json_list = bulk_geometry_to_centroid(geometry_list)
        write_only_centroid(out_f, buf, centroid_json_list)
        buf = []
    
    if len(buf) > 0:
        geometry_list = [x['geometry'] for x in buf]
        centroid_json_list = bulk_geometry_to_centroid(geometry_list)
        write_only_centroid(out_f, buf, centroid_json_list)
        buf = []

    out_f.close()
    in_f.close()
    return

if __name__ == '__main__':
    filename, outname = sys.argv[1], sys.argv[2]
    site_centroid_gen(filename, outname)
