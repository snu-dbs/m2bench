res = db._query(`
LET Z1 = 3
LET Z2 = 4
LET interval = 10800
   

LET lat_min = 34.01189870
LET arrayinfo_lat_grid_interval =  0.000172998

LET lon_min = -118.3450100223
LET arrayinfo_lon_grid_interval = 0.000216636

LET lat_max = arrayinfo_lat_grid_interval*522+lat_min
LET lon_max = arrayinfo_lon_grid_interval*522+lon_min


 

            
Let B = (For site in Site 
            Filter site.properties.type == "building"
            Filter site.properties.description == "school"

            Let multipolygon = site.geometry.coordinates
            
            For n1 in multipolygon
                    For n2 in n1
                        For n3 in n2 
                            
                            Collect site_id = site.site_id
                            AGGREGATE lon = AVERAGE(n3[0]), lat = AVERAGE(n3[1])
                            Return {site_id, lon, lat}
            
        )


Let A = (For finedust in Finedust_idx
            Filter finedust.timestamp >= Z1
            Filter finedust.timestamp <= Z2 
            Collect latitude = finedust.latitude, longitude = finedust.longitude
            Aggregate avg_pm10 = Avg(finedust.pm10)
            Return {latitude, longitude, avg_pm10})
 
Let C = (For b in B
            Filter b.lat >= lat_min 
            Filter b.lon >= lon_min 
            Filter b.lat <= lat_max
            Filter b.lon <= lon_max 
            For a in A 
                Filter (b.lat - 34.01189870)/arrayinfo_lat_grid_interval <= a.latitude + 1 
                Filter (b.lat - 34.01189870)/arrayinfo_lat_grid_interval >= a.latitude
                Filter (b.lon - (-118.3450100223))/arrayinfo_lon_grid_interval <= a.longitude + 1
                Filter (b.lon - (-118.3450100223))/arrayinfo_lon_grid_interval >= a.longitude 
                Return {lat:b.lat, lon:b.lon, pm10:a.avg_pm10})
        
Return(Length(C))
 `);


res2 = 	res.getExtra();

print (res)
print(res2['stats']['executionTime']);


