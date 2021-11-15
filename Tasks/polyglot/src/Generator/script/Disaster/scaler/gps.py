import pandas as pd
import math
import random
import numpy as np
from geopy.distance import distance
from datetime import datetime
import time
import sys

data = pd.read_csv("./real_dataset/gpw_v4_admin_unit_center_points_population_estimates_rev11_usaca.csv")

users = 50000*int(sys.argv[1])
div = 40000000/users
val = data[data['UN_2020_E']/div>=1]['UN_2020_E'].values
val = val/div
val = np.around(val)
val_sum = np.sum(val)
mul = int(users/val_sum)

gid = 0 # user id
result = []
for i in range(len(data)):
    lat = data.loc[i]['CENTROID_Y']
    lon = data.loc[i]['CENTROID_X']
    area = data.loc[i]['TOTAL_A_KM']
    pol = data.loc[i]['UN_2020_E']/div
    
    length = math.sqrt(area)/2
    measure_spot = (lat,lon)
    
    _, min_lon, _ = distance(kilometers=length).destination(measure_spot, 270)
    min_lat, _, _ = distance(kilometers=length).destination(measure_spot, 180)
    max_lat, _, _ = distance(kilometers=length).destination(measure_spot, 0)
    _, max_lon, _ = distance(kilometers=length).destination(measure_spot, 90)

    if pol >= 1:
        pol = int(np.around(pol)*mul)
        for j in range(pol):
            timestamp = 1600182000 # 2020-09-16
            for k in range(168):# 7 days x 24 hours
                new_lon = random.uniform(min_lon, max_lon)
                new_lat = random.uniform(min_lat, max_lat)
                d = datetime.fromtimestamp(timestamp).isoformat() + ".000Z"
                result.append([gid, new_lon, new_lat, d])
                timestamp += 3600 # 1 hour
            gid += 1

while gid < users:
    for i in range(len(data)):
        lat = data.loc[i]['CENTROID_Y']
        lon = data.loc[i]['CENTROID_X']
        area = data.loc[i]['TOTAL_A_KM']
        
        length = math.sqrt(area)/2
        measure_spot = (lat,lon)
    
        _, min_lon, _ = distance(kilometers=length).destination(measure_spot, 270)
        min_lat, _, _ = distance(kilometers=length).destination(measure_spot, 180)
        max_lat, _, _ = distance(kilometers=length).destination(measure_spot, 0)
        _, max_lon, _ = distance(kilometers=length).destination(measure_spot, 90)

        timestamp = 1600182000 # 2020-09-16
        for k in range(168):# 7 days x 24 hours
            new_lon = random.uniform(min_lon, max_lon)
            new_lat = random.uniform(min_lat, max_lat)
            d = datetime.fromtimestamp(timestamp).isoformat() + ".000Z"
            result.append([gid, new_lon, new_lat, d])
            timestamp += 3600 # 1 hour
        gid += 1
        if gid == users:
            break

df = pd.DataFrame(result,columns=['id','longitude','latitude','time'])
df.columns = ['user_id','longitude','latitude','time']
df.index.name='gps_id'
df['latitude'] = df['latitude'].map(lambda x: '{:.6f}'.format(x))
df['longitude'] = df['longitude'].map(lambda x: '{:.6f}'.format(x))
df.to_csv('Gps.csv')
