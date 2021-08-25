import pandas as pd
from shapely.geometry import Point, Polygon
import json
import numpy as np
import os

def roadnetwork_gen(data_dirpath, outdir):

    os.system("sed '1,7d' " + data_dirpath + "USA-road-d.CAL.co | sed 's/ /,/g' > co.csv")
    os.system("sed '1,7d' " + data_dirpath + "USA-road-d.CAL.gr | sed 's/ /,/g' > gr.csv")

    df1 = pd.read_csv('co.csv',names=['type','id','longitude','latitude']) # node
    df2 = pd.read_csv('gr.csv',names=['type','from','to','distance']) # edge

    df1 = df1[['id','latitude','longitude']]
    df1['latitude'] /= 1000000
    df1['longitude'] /= 1000000

    with open("ca.json", 'r') as json_file: # only consider CA
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

    site_id = 9646403
    nid = []
    sid = []
    latitude = []
    longitude = []
    for idx, row in df1.iterrows():
        p1 = Point(row['longitude'], row['latitude'])
        for poly in multipoly:
            if p1.within(poly):
                nid.append(row['id'])
                sid.append(site_id)
                latitude.append(row['latitude'])
                longitude.append(row['longitude'])
                site_id += 1
                break

    result = pd.DataFrame({'roadnode_id':nid, 'site_id':sid})
    result = result.astype({'roadnode_id':np.int64})
    result.to_csv(outdir+'Roadnode.csv',index=False) #node

    result = pd.DataFrame({'roadnode_id': nid, 'site_id': sid, 'latitude':latitude, 'longitude':longitude})
    result = result.astype({'roadnode_id': np.int64})
    result.to_csv(outdir + 'original_roadnode.csv', index=False)  # This is for site data

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

    edge = pd.DataFrame({'from':from1,'to':to1,'distance':distance})
    edge.to_csv(outdir+'Road.csv',index=False) #edge

    os.system("rm co.csv gr.csv")
