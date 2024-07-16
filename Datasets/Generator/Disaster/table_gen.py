import os
import pandas as pd
import math
import random
import numpy as np
from geopy.distance import distance
from datetime import datetime
import time
import sys
from sklearn.neighbors import KDTree
from multiprocessing import Process, Manager, Value, Lock, Pool
import json

def earthquake_gen(data_dirpath, outdir):
    filelist = os.listdir(data_dirpath)

    result = None
    filelist.sort()
    for file in filelist:
        if file.endswith('.csv'):
            df = pd.read_csv(data_dirpath+file)
            result = pd.concat([result, df])

    result = result.sort_values(by=['time'],axis=0)
    result = result[result['place'].str.contains("CA", na=False)] # choose only CA
    df = result[['time','latitude','longitude','depth','mag']]
    df.columns = ['time','latitude','longitude','depth','magnitude']
    df = df.reset_index()
    df.index.name='earthquake_id'
    df = df.drop('index',axis=1)
    df['latitude'] = df['latitude'].map(lambda x: '{:.6f}'.format(x))
    df['longitude'] = df['longitude'].map(lambda x: '{:.6f}'.format(x))
    df.to_csv(outdir+'Earthquake.csv',sep=',',na_rep='NaN')

def ST_centroid(coordinatessss):
    lon = 0.0
    lat = 0.0
    nrow = 0
    for coordinatesss in coordinatessss:
        for coordinatess in coordinatesss:
            for coordinates in coordinatess:
                lon+=coordinates[0]
                lat+=coordinates[1]
                nrow += 1
    return [lon/nrow, lat/nrow]

def convert_shelter(outdir):
    mylist = []
    mydict = {}

    #print("1. Read Site.json and Extract coordinates")
    with open(outdir+"/../json/Site.json", 'r') as site_file:
        id = 0
        i = 0
        for line in site_file:
            site_json = json.loads(line)
            i += 1
            if 'properties' in site_json:
                if site_json['properties']['type'] == 'building':
                    mydict[id] = site_json['site_id']
                    mylist.append(ST_centroid(site_json['geometry']['coordinates']))
                    id += 1
            #progress = str(int(i / 1367834) * 10) + "% ..."
            #if (i % 1367834 == 0): print(progress)
    #print("Total number of objects:", i, "read")
    #print("Total number of buildings:", id, "read")

    #print("2. Build Kd Tree")
    building_coordinates = np.array(mylist)
    building_kdtree = KDTree(building_coordinates)

    #print("3. Find closest building for each shelter")

    unique_site = []
    with open(outdir+"Shelter.csv", 'w') as new_shelter_file:
        with open(outdir+"original_shelter.csv", 'r') as shelter_file:
            i = 0
            shelter_file.readline()
            new_shelter_file.write("shelter_id|site_id|capacity|name\n")
            for line in shelter_file:
                parsed_line = line.split("|")
                shelter_lon = float(parsed_line[1])
                shelter_lat = float(parsed_line[2])
                dist, ind = building_kdtree.query([[shelter_lon, shelter_lat]], 1)
                if str(mydict[ind[0][0]]) in unique_site:
                    continue
                unique_site.append(str(mydict[ind[0][0]]))
                new_shelter_file.write(parsed_line[0] + "|" + str(mydict[ind[0][0]]) + "|" +
                                       parsed_line[3] + "|" + parsed_line[4])

                i += 1

    shelter_file.close()
    site_file.close()
    new_shelter_file.close()

def shelter_gen(data_dirpath, outdir):
    data = pd.read_csv(data_dirpath + "National_Shelter_System_Facilities.csv", low_memory=False)
    df = None    
    if 'STATE' in data.columns:
        # the old dataset has some capitalized columns
        data = data[data['STATE'] == 'CA']
        df = data[['X', 'Y', 'EVACUATION_CAPACITY', 'SHELTER_NAME']]
    else:
        # the latest dataset has some lowercased columns
        data = data[data['state'] == 'CA']
        df = data[['X', 'Y', 'evacuation_capacity', 'shelter_name']]       # X and Y is capitalized

    df.columns = ['longitude', 'latitude', 'capacity', 'name']

    df = df.reset_index()
    df.index.name = 'shelter_id'
    df = df.drop('index', axis=1)
    df['latitude'] = df['latitude'].map(lambda x: '{:.6f}'.format(x))
    df['longitude'] = df['longitude'].map(lambda x: '{:.6f}'.format(x))
    df.to_csv(outdir+'original_shelter.csv', sep='|', na_rep='NaN')

    convert_shelter(outdir)

    os.system("rm "+outdir+"original_shelter.csv")


def init(l):
    global lock
    lock = l


def callfunc2(df_partial, v, div, mul):
    result = []
    for _, row in df_partial.iterrows():
        lat = row['CENTROID_Y']
        lon = row['CENTROID_X']
        area = row['TOTAL_A_KM']
        pol = row['UN_2020_E'] / div

        length = math.sqrt(area) / 2
        measure_spot = (lat, lon)

        _, min_lon, _ = distance(kilometers=length).destination(measure_spot, 270)
        min_lat, _, _ = distance(kilometers=length).destination(measure_spot, 180)
        max_lat, _, _ = distance(kilometers=length).destination(measure_spot, 0)
        _, max_lon, _ = distance(kilometers=length).destination(measure_spot, 90)

        if pol >= 1:
            pol = int(np.around(pol) * mul)
            for j in range(pol):
                lock.acquire()
                gid = v.value
                v.value += 1
                lock.release()

                timestamp = 1600182000  # 2020-09-16
                for k in range(168):  # 7 days x 24 hours
                    new_lon = random.uniform(min_lon, max_lon)
                    new_lat = random.uniform(min_lat, max_lat)
                    d = datetime.fromtimestamp(timestamp).isoformat() + ".000Z"
                    result.append([gid, '{:.6f}'.format(new_lon), '{:.6f}'.format(new_lat), d])
                    timestamp += 3600  # 1 hour

    return result

def gps_gen(data_dirpath, outdir):
    data = pd.read_csv(data_dirpath + "gpw_v4_admin_unit_center_points_population_estimates_rev11_usaca.csv")

    users = 50000
    div = 40000000 / users
    val = data[data['UN_2020_E'] / div >= 1]['UN_2020_E'].values
    val = val / div
    val = np.around(val)
    val_sum = np.sum(val)
    mul = int(users / val_sum)

    #############################
    gid = 0  # user id
    result = []

    num_proc = 8
    size_df1 = len(data.index)
    slice_df1 = int(math.ceil(size_df1 / num_proc))

    l = Lock()
    with Pool(processes=num_proc, initializer=init, initargs=(l, )) as pool:
        m = Manager()
        v = m.Value('i', gid)

        procs2 = [pool.apply_async(callfunc2, (data[(i*slice_df1):min(((i + 1) * slice_df1), size_df1)], v, div, mul)) for i in range(num_proc)]
        for p in procs2:
            result.extend(p.get())

    gid = v.value
    while gid < users:
        for i in range(len(data)):
            lat = data.loc[i]['CENTROID_Y']
            lon = data.loc[i]['CENTROID_X']
            area = data.loc[i]['TOTAL_A_KM']

            length = math.sqrt(area) / 2
            measure_spot = (lat, lon)

            _, min_lon, _ = distance(kilometers=length).destination(measure_spot, 270)
            min_lat, _, _ = distance(kilometers=length).destination(measure_spot, 180)
            max_lat, _, _ = distance(kilometers=length).destination(measure_spot, 0)
            _, max_lon, _ = distance(kilometers=length).destination(measure_spot, 90)

            timestamp = 1600182000  # 2020-09-16
            for k in range(168):  # 7 days x 24 hours
                new_lon = random.uniform(min_lon, max_lon)
                new_lat = random.uniform(min_lat, max_lat)
                d = datetime.fromtimestamp(timestamp).isoformat() + ".000Z"
                result.append([gid, '{:.6f}'.format(new_lon), '{:.6f}'.format(new_lat), d])
                timestamp += 3600  # 1 hour
            gid += 1
            if gid == users:
                break

    df = pd.DataFrame(result, columns=['id', 'longitude', 'latitude', 'time'])
    df.columns = ['user_id', 'longitude', 'latitude', 'time']
    df.index.name = 'gps_id'
    df.to_csv(outdir+'Gps.csv')
