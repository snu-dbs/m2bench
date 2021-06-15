//
// Created by mxmdb on 21. 5. 3..
//

void T12(){



}

void T13(){



}

/**
 *  [Task16] Fine Dust Backtesting ([D, A]=> D).
 *  For a given timestamp Z, hindcast the pm10 values of the schools. (The Z is teh number between min, max of the timestamp dimension.)
 *
 *  Z1 = (Z/TimeInterval)*TimeInterval
 *  Z2 = {(Z+TimeInterval-1)/TimeInterval}*TimeInterval
 *
 *  A: SELECT latitude, longitude, avg(pm10) FROM Finedust
 *      WHERE timestamp >= Z1  and timestamp <= Z2 group by lat, lon
 *
 *  B: SELECT Map.properties.osm_id AS id, location,
 *      FROM Map, A
 *      WHERE
 *          WithIN(Box(latitude, longitude, latitude+e1, longitude+e2), ST_Centroid(Map.geometry))
 *          Map.properties.building = 'school' //Document
 *
 */


void T16(){

    /* step 0 */
//    db.T16_temp.drop()
//    db._create("T16_temp")
//    db._create("T16_temp")
//    db.T16_temp.ensureIndex({type:"persistent", fields:["lat", "lon"]})
//    db.T16_temp.ensureIndex({type:"persistent", fields:["lat"]})
//    db.T16_temp.ensureIndex({type:"persistent", fields:["lon"]})



    /* step 1 */
//    For t in T16_temp
//    Remove t in T16_temp



    /* step 2 */
//    LET Z1 = 1600182000+3*10800
//    LET Z2 = 1600182000+4*10800
//
//    LET lat_min = 34.01189870
//    LET arrayinfo_lat_grid_interval =  0.000172998
//
//    LET lon_min = -118.3450100223
//    LET arrayinfo_lon_grid_interval = 0.000216636
//
//    LET lat_max = arrayinfo_lat_grid_interval*522+lat_min
//    LET lon_max = arrayinfo_lon_grid_interval*522+lon_min
//
//
//    Let B = (For map in Map
//    Filter map.properties.building == "school"
//    Let multipolygon = map.geometry.coordinates
//
//    Let ZIP =
//    (For n1 in multipolygon
//    For n2 in n1
//    For n3 in n2
//    Let lon = n3[0]
//    Let lat = n3[1]
//    Return {lon, lat}
//    )
//
//    Let Centroid = (For zip in ZIP
//    Collect
//    AGGREGATE lon = AVERAGE(zip.lon), lat = AVERAGE(zip.lat)
//    Return {lon, lat}
//    )
//
//    Return{lat: Centroid[0].lat,lon: Centroid[0].lon}
//    )
//    FOR b IN B
//    INSERT { lat: b.lat , lon: b.lon } INTO T16_temp



    /* step 3 */
//    LET Z1 = 1600182000+3*10800
//    LET Z2 = 1600182000+4*10800
//
//
//    LET lat_min = 34.01189870
//    LET arrayinfo_lat_grid_interval =  0.000172998
//
//    LET lon_min = -118.3450100223
//    LET arrayinfo_lon_grid_interval = 0.000216636
//
//    LET lat_max = arrayinfo_lat_grid_interval*522+lat_min
//    LET lon_max = arrayinfo_lon_grid_interval*522+lon_min
//
//    Let A = (For finedust in Finedust
//    Filter finedust.timestamp >= Z1
//    Filter finedust.timestamp <= Z2
//    Collect latitude = finedust.latitude, longitude = finedust.longitude
//    Aggregate avg_pm10 = Avg(finedust.pm10)
//    Return {latitude, longitude, avg_pm10})
//
//    Let C = (For b in T16_temp
//    Filter b.lat >= lat_min
//    Filter b.lon >= lon_min
//    Filter b.lat <= lat_max
//    Filter b.lon <= lon_max
//    For a in A
//    Filter b.lat-0.000172998 <= a.latitude
//    Filter b.lat >= a.latitude
//    Filter b.lon-0.000216636 <= a.longitude
//    Filter b.lon >= a.longitude
//    Return {lat:b.lat, lon:b.lon, pm10:a.avg_pm10})
//
//    Return(Length(C))



}