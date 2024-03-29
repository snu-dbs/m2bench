iquery -aq "remove(Finedust_temp)"
iquery -aq "remove(Finedust)"
iquery -aq "create array Finedust_temp <timestamp:int64, latitude:int64, longitude:int64, pm10:double, pm2_5:double>[row_num=0:*,?,0]"
iquery -anq "load(Finedust_temp, '/tmp/m2bench/Finedust_idx.csv', -2, 'csv')"
iquery -anq "store(redimension(Finedust_temp, < pm10:double, pm2_5:double >[timestamp=0:*,61,0, latitude=0:*,522,0, longitude=0:*,522,0]), Finedust)"
