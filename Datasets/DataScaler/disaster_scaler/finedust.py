import os
from geopy.distance import distance
import pandas as pd
import sys


#print 'Number of arguments:', len(sys.argv), 'arguments.'
#print 'Argument List:', str(sys.argv)


if len(sys.argv) != 3 :
    print "Run: python finedust.py [ScaleFactor]  [path_to_finedust_raw_dataset]"
    print "ex) python finedust.py 2 ~/m2bench/Datasets/raw_datasets/finedust_dataset/"
    exit(0); 

path2finedust = str(sys.argv[2])
filelist = os.listdir(path2finedust)

measure_spot = (34.057076, -118.290731) # place where the lidar h/w installed (the center of the circle area)
distance_per_point = 28.8 / 1000 # 28.8 meters

interval = int(648000/(61*int(sys.argv[1])-1))

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
        df = pd.read_csv(path2finedust+file)
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
result.to_csv('Finedust.csv',index=False)
