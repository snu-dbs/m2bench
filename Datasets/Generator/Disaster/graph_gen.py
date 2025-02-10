import pandas as pd
from shapely.geometry import Point, Polygon
import json
import numpy as np
import os
import time
import math
from multiprocessing import Process, Manager, Value, Lock, Pool
import sys


def callfunc2(df_partial, multipoly):
    result = []
    for idx, row in df_partial.iterrows():
        p1 = Point(row['longitude'], row['latitude'])
        for poly in multipoly:
            if p1.within(poly):
                nid, lat, lon = int(row['id']), row['latitude'], row['longitude']
                result.append([nid, lat, lon])
                break

    return result


def roadnetwork_gen(data_dirpath, outdir):
    os.system("sed '1,7d' " + data_dirpath + "USA-road-d.CAL.co | sed 's/ /,/g' > co.csv")
    os.system("sed '1,7d' " + data_dirpath + "USA-road-d.CAL.gr | sed 's/ /,/g' > gr.csv")

    df1 = pd.read_csv('co.csv',names=['type','id','longitude','latitude']) # node
    df2 = pd.read_csv('gr.csv',names=['type','from','to','distance']) # edge

    df1 = df1[['id','latitude','longitude']]
    df1['latitude'] /= 1000000
    df1['longitude'] /= 1000000

    with open(data_dirpath + "/ca.json", 'r') as json_file: # only consider CA
        json_data = json.load(json_file)

    ca_list = json_data['loc']['coordinates']
    multipoly = []
    for p1 in ca_list:
        poly = []
        for p2 in p1:
            for p3 in p2:
                tuple1 = (p3[0],p3[1])
                poly.append(tuple1)
        result = Polygon(poly)
        multipoly.append(result)

    site_id = 100000000         # Really big number. osm objects in california may not exceed this number.
    num_proc = 8
    size_df1 = len(df1.index)
    slice_df1 = int(math.ceil(size_df1 / num_proc))

    merged = []
    with Pool(processes=num_proc) as pool:
        m = Manager()
        procs2 = [pool.apply_async(callfunc2, (df1[(i*slice_df1):min(((i + 1) * slice_df1), size_df1)], multipoly)) for i in range(num_proc)]
        for p in procs2:
            res = p.get()
            merged.extend(res)

    result = pd.DataFrame(merged, columns=['roadnode_id', 'latitude', 'longitude'])
    result['site_id'] = result.index + site_id

    result.to_csv(outdir + 'original_roadnode.csv', index=False)  # This is for site data
    result.drop(columns=['latitude', 'longitude']).to_csv(outdir+'Roadnode.csv',index=False) #node

    df2 = df2[['from','to','distance']]

    from1 = []
    to1 = []
    distance = []
    ids = result['roadnode_id'].tolist()
    ids = set(ids)
    for idx, row in df2.iterrows():
        if row['from'] in ids and row['to'] in ids:
            from1.append(row['from'])
            to1.append(row['to'])
            distance.append(row['distance'])

    edge = pd.DataFrame({'_from':from1,'_to':to1,'distance':distance})
    edge.to_csv(outdir+'Road.csv',index=False) #edge

    os.system("rm co.csv gr.csv")

