import os
from geopy.distance import distance
import pandas as pd
import sys

def finedust_gen(data_dirpath, outdir):

    filelist = os.listdir(data_dirpath)

    measure_spot = (34.057076, -118.290731) # place where the lidar h/w installed (the center of the circle area)
    distance_per_point = 28.8 / 1000 # 28.8 meters

    interval = int(648000/(61*1-1))

    _, min_lon, _ = distance(kilometers=distance_per_point * 174).destination(measure_spot, 270)
    min_lat, _, _ = distance(kilometers=distance_per_point * 174).destination(measure_spot, 180)
    max_lat, _, _ = distance(kilometers=distance_per_point * 174).destination(measure_spot, 0)
    _, max_lon, _ = distance(kilometers=distance_per_point * 174).destination(measure_spot, 90)

    lat = []
    lon = []
    for i in range(522):
        for j in range(522):
            lat.append("{:.6f}".format(min_lat + i*0.000172998)) # 28.8m interval in latitude
            lon.append("{:.6f}".format(min_lon + j*0.000216636)) # 28.8m interval in longitude

    time = 1600182000 # 2020/09/16 00:00:00
    result = None
    filelist.sort()
    i = 0
    prev = None
    for file in filelist:
        if file.endswith('.csv'):
            df = pd.read_csv(data_dirpath+file)
            df['latitude'] = lat
            df['longitude'] = lon
            df = df.dropna(axis=0)
            while time <= 1600182000 + 10800*i:
                df['timestamp'] = time
                ratio = (1600182000 + 10800*i - time)/float(10800)
                if prev is not None:
                    df['pm10'] = df['pm10']*(1-ratio) + prev['pm10']*ratio
                    df['pm2.5'] = df['pm2.5']*(1-ratio) + prev['pm2.5']*ratio
                df = df[['timestamp','latitude','longitude','pm10','pm2.5']]
                if result is None:
                    result = df
                else:
                    result = pd.concat([result,df])
                time += interval
            prev = df
            i += 1

    result['pm10'] = result['pm10'].map(lambda x: '{:.2f}'.format(x))
    result['pm2.5'] = result['pm2.5'].map(lambda x: '{:.2f}'.format(x))
    result.to_csv(outdir+'Finedust.csv',index=False, header=True)

def finedust_idx_gen(data_dirpath, outdir):

    filelist = os.listdir(data_dirpath)

    measure_spot = (34.057076, -118.290731) # place where the lidar h/w installed (the center of the circle area)
    distance_per_point = 28.8 / 1000 # 28.8 meters

    interval = int(648000/(61*1-1))

    _, min_lon, _ = distance(kilometers=distance_per_point * 174).destination(measure_spot, 270)
    min_lat, _, _ = distance(kilometers=distance_per_point * 174).destination(measure_spot, 180)
    max_lat, _, _ = distance(kilometers=distance_per_point * 174).destination(measure_spot, 0)
    _, max_lon, _ = distance(kilometers=distance_per_point * 174).destination(measure_spot, 90)

    lat = []
    lon = []
    for i in range(522):
        for j in range(522):
            lat.append(i)  # 28.8m interval in latitude
            lon.append(j)  # 28.8m interval in longitude

    time = 1600182000  # 2020/09/16 00:00:00
    time_idx = 0
    result = None
    filelist.sort()
    i = 0
    prev = None
    for file in filelist:
        if file.endswith('.csv'):
            df = pd.read_csv(data_dirpath + file)
            df['latitude'] = lat
            df['longitude'] = lon
            df = df.dropna(axis=0)
            while time <= 1600182000 + 10800 * i:
                df['timestamp'] = time_idx
                ratio = (1600182000 + 10800 * i - time) / float(10800)
                if prev is not None:
                    df['pm10'] = df['pm10'] * (1 - ratio) + prev['pm10'] * ratio
                    df['pm2.5'] = df['pm2.5'] * (1 - ratio) + prev['pm2.5'] * ratio
                df = df[['timestamp', 'latitude', 'longitude', 'pm10', 'pm2.5']]
                if result is None:
                    result = df
                else:
                    result = pd.concat([result, df])
                time += interval
                time_idx += 1
            prev = df
            i += 1

    result['pm10'] = result['pm10'].map(lambda x: '{:.2f}'.format(x))
    result['pm2.5'] = result['pm2.5'].map(lambda x: '{:.2f}'.format(x))
    result.loc[:, 'timestamp'] = result['timestamp'].apply(int)
    result.loc[:, 'latitude'] = result['latitude'].apply(int)
    result.loc[:, 'longitude'] = result['longitude'].apply(int)
    result.to_csv(outdir+'Finedust_idx.csv', index=False, header=True)
