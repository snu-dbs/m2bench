//
// Created by mxmdb on 21. 5. 3..
//

void T10(){

//    LET Z1 = "2020-05-01T00:00:00.000Z"
//    LET Z2 = "2020-07-01T00:00:00.000Z"
//
//    LET A = (FOR e IN Earthquake
//                FILTER e.time >= Z1  && e.time < Z2
//                RETURN {earthquake_id: e._id, latitude: e.latitude, longitude: e.longitude} )
//
//
//    LET B = (FOR node IN Roadnode
//                FOR a IN A
//                    LET dist = GEO_DISTANCE([a.longitude, a.latitude], [node.longitude, node.latitude])
//                    FILTER dist <= 15000
//                    RETURN node  )
//
//
//    LET C = (FOR b IN B
//                FOR v, e IN OUTBOUND b Road
//                RETURN {b, v, e})
//
//    RETURN length(C)

}

void T11(){


///* Parameter setting */
//    LET X_id = 41865
//    FOR e IN Earthquake
//        FILTER e.earthquake_id == X_id
//        LET X_latitude = e.latitude
//        LET X_longitude = e.longitude
//        LET X_time = e.time
//
///* Queries */
//    LET A = (FOR g IN Gps
//               LET dist = GEO_DISTANCE([X_longitude, X_latitude], [g.longitude, g.latitude])
//                FILTER dist <= 20000 && g.time >= X_time && g.time <= DATE_ADD(X_time, "PT1H")
//                RETURN {id: g.gps_id, latitude: g.latitude, longitude: g.longitude} )
//
//    LET B = (FOR a IN A
//                LET temp = (FOR node in Roadnode
//                SORT GEO_DISTANCE([a.longitude, a.latitude], [node.longitude, node.latitude]) ASC
//                LIMIT 1
//                RETURN node)
//            RETURN {gps_id: a.id, roadnode_id: temp[0]._id}
//    )  // GPS
//
//
//    LET C = (FOR s IN Shelter
//                LET dist = GEO_DISTANCE([X_longitude, X_latitude], [s.longitude, s.latitude])
//                FILTER dist <= 15000
//                RETURN {id: s.shelter_id, latitude: s.latitude, longitude: s.longitude} )
//
//    LET D = (FOR c IN C
//                LET temp = (FOR node in Roadnode
//                SORT GEO_DISTANCE([c.longitude, c.latitude], [node.longitude, node.latitude]) ASC
//                LIMIT 1
//                RETURN node)
//            RETURN {shelter_id: c.id, roadnode_id: temp[0]._id}
//    )  // Shelter
//
//    LET E = (FOR b IN B
//                FOR d IN D
//                LET path = ( FOR vertex, edge IN ANY SHORTEST_PATH b.roadnode_id TO d.roadnode_id Road
//                RETURN edge.distance)
//                RETURN {gps_id: b.gps_id, shelter_id: d.shelter_id, cost: SUM(path)})
//
//    RETURN length(E)
}

/**
 * [Task 12] New Shelter candidates (R, D, G => Relational)
 *  For the shelter of which the number of GPS coordinates are the most within 5km from the shelter between time Z1 and Z2,
 *  find buildings for new shelters located within 2km on foot from the shelter. (The buildings are limited by 5km from the shelter.)
 *
 *  A:  SELECT Shelter.id, Shelter.location, COUNT(GPS.id) AS cnt
 *          FROM Shelter, GPS
 *          WHERE ST_Distance(GPS.location, Shelter.location) <= 5km
 *              AND GPS.time >= Z1 AND GPS.time <= Z2
 *          GROUP BY Shelter.id, Shelter.location ORDER BY cnt DESC LIMIT 1
 *
 *  B:  SELECT A.id, ST_ClosestObject(RoadNode, node, A.location) AS node_id
 *          FROM A,RoadNode //Relational
 *
 *  C: SELECT Map.properties.osm_id AS id, Map.geometry, ST_ClosestObject(RoadNode, node, ST_Centroid(Map.geometry)) AS node_id
 *      FROM A, Map, RoadNode
 *      WHERE ST_Distance(Map.geometry, A.location) <= 5km
 *          AND Exists(Map.properties.building) //Relational
 *
 *  D: SELECT C.id
 *      FROM B, C, RoadNode
 *      WHERE ShortestPath(RoadNode, startNode:B.node_id, endNode:C.node_id) <= 2km //Relationa
 */
void T12(){

    //    db.T12_temp.drop()
    //    db._create("T12_temp")

    /*step 1*/
//    LET Z1 = "2020-09-17T00:00:00.000Z"
//    LET Z2 = "2020-09-17T01:00:00.000Z"

//    LET A1 = (FOR gps IN Gps
//    FILTER gps.time >= Z1  && gps.time <= Z2
//    RETURN gps )

//    LET A2 = (FOR shelter IN Shelter
//    FOR a IN A1
//    FILTER GEO_DISTANCE([shelter.longitude, shelter.latitude], [a.longitude, a.latitude]) <= 5000
//    COLLECT shelter_id = shelter.shelter_id, latitude = shelter.latitude, longitude= shelter.longitude WITH COUNT into cnt
//    SORT cnt DESC
//    LIMIT 1
//    RETURN {shelter_id, latitude, longitude, cnt}
//    )
//
//    LET B = (FOR node in Roadnode
//    SORT GEO_DISTANCE([A2[0].longitude, A2[0].latitude], [node.longitude, node.latitude]) ASC
//    LIMIT 1
//    RETURN {shelter_id: A2[0].shelter_id, roadnode_id: node._id, latitude: node.latitude, longitude: node.longitude}
//    )
//
//    FOR b IN B
//    INSERT {shelter_id: B[0].shelter_id, shelter_roadnode_id: B[0].roadnode_id, shelter_lat: B[0].latitude, shelter_lon: B[0].longitude  } INTO T12_temp


    /*step 2 */
//    LET shelter_lon = T12_temp[0].shelter_lon
//    LET shelter_lat = T12_temp[0].shelter_lat
//
//    LET C = (FOR map IN Map
//    FILTER map.properties.building != null
//
//    Let multipolygon = map.geometry.coordinates
//    Let ZIP =
//        (For n1 in multipolygon
//        For n2 in n1
//        For n3 in n2
//        Let lon = n3[0]
//        Let lat = n3[1]
//        Return {lon, lat} )
//    Let Centroid = (For zip in ZIP
//                        Collect
//                        AGGREGATE lon = AVERAGE(zip.lon), lat = AVERAGE(zip.lat)
//                        Return {lon, lat} )
//    FILTER GEO_DISTANCE([shelter_lon, shelter_lat], [Centroid[0].lon, Centroid[0].lat]) <= 5000
//    Return{map_id: map.map_id, latitude: Centroid[0].lat, longitude: Centroid[0].lon}
//    )
//
//    FOR c IN C
//        INSERT {map_id: c.map_id, centroid_lat: c.latitude, centroid_long: c.longitude} INTO T12_temp

    /* step 3 */
//    LET shelter = T12_temp[0]
//    LET D = (FOR t IN T12_temp
//                FILTER t.centroid_lat != null
//                LET temp = (FOR map in Map
//                                FILTER map.properties.roadnode == "yes"
//                                SORT GEO_DISTANCE([t.centroid_long, t.centroid_lat], map.geometry) ASC
//                                LIMIT 1
//                                RETURN map )
//    RETURN {building_map_id: t.map_id, closest_node_map_id: temp[0].map_id} )

// (match each closest_to_building_node_map_id to roadnode_id for graph query)
//    LET E = (FOR node IN Roadnode
//                FOR d IN D
//                FILTER node.map_id == d.closest_node_map_id
//                RETURN node )
//
//    LET F = (FOR e IN E
//                LET path = ( FOR vertex, edge IN ANY SHORTEST_PATH e._id TO shelter.shelter_roadnode_id Road
//                                RETURN edge.distance)
//                FILTER SUM(path) < 2000
//                RETURN e)
//
//    RETURN length(F)


//    For t in T12_temp
//        Remove t in T12_temp
}


/*
 * [Task 13] Damage Statistics.
 *
 * For the earthquakes of which magnitude is greater than 4.5, find the building statistics.
 * The buildings are limited by 30km from the earthquake location. (Relational, Document) -> Document
 *
 * A =  SELECT Site.properties.description, COUNT(*) 
 *      FROM Earthquake, Site WHERE ST_Distance(Site.geometry, Earthquake.coordinates) <= 30km 
 *          AND Site.properties.type = 'building' 
 *          AND Earthquake.magnitude >= 4.5 
 *      GROUP BY Site.properties.description // Document
 *
 * 
 * [Query]
 * 
 
[OLD]
FOR map IN Map
    // Exists(Map.properties.building)
    FILTER map.properties.building != null  
    
    FOR eq IN Earthquake
        // Earthquake.magnitude >= 4.5
	    FILTER eq.magnitude >= 4.5
	    
	    // ST_Distance(Map.geometry,Earthquake.location) <= 30km
	    LET dist = GEO_DISTANCE([eq.longitude, eq.latitude], map.geometry)
	    FILTER dist <= 30000
	    
	    // GROUP BY Map.properties.building
	    COLLECT building = map.properties.building
	        WITH COUNT INTO cnt


[New]
FOR site IN Site
    FILTER site.properties.type == 'building'
    FOR eq IN Earthquake
	    FILTER eq.magnitude >= 4.5

	    LET dist = GEO_DISTANCE([eq.longitude, eq.latitude], site.geometry, "wgs84")
	    FILTER dist <= 30000
	    
	    COLLECT description = site.properties.description
	        WITH COUNT INTO cnt
	    
	    RETURN {description, cnt}
	    
 */


/* 
 * [Task 14] Sources of Fine Dust.
 *
 * Analyze fine dust hotspot by date between time Z1 and Z2.
 * Print the nearest building with time of the hotspot.
 * Use window aggregation with a size of 5. (Document, Array) -> Document
 *
 * A =  SELECT date, timestamp, latitude, longitude, AVG(pm10) AS pm10_avg 
 *      FROM FineDust 
 *      WHERE timestamp >= Z1 
 *          AND timestamp <= Z2 
 *      WINDOW 1, 5, 5 // Array
 * B =  REDIMENSION(A, <pm10_avg: float>[date=0:*, timestamp=0:*, latitude=0:*, longitude=0:*]) // Array
 * C =  SELECT t1.date, t1.timestamp, (t1.latitude, t1.longitude) AS coordinates 
 *      FROM B AS t1, (
 *          SELECT date, MAX(pm10_avg) AS pm10_max 
 *          FROM B GROUP BY date
 *      ) AS t2 
 *      WHERE t1.pm10_avg = t2.pm10_max 
 *          AND t1.date = t2.date // Array 
 * D =  SELECT C.date, C.timestamp, ST_ClosestObject(Site, building, C.coordinates) AS site_id 
 *      FROM C, Site 
 *      ORDER BY C.date ASC // Document
 * 
 * 
 * [Query]
  
 LET Z1 = 1
 LET Z2 = 1
  
 LET AB = (
    FOR cell IN Finedust
        // WHERE timestamp >= Z1 AND timestamp <= Z2
        FILTER ((Z1 * 10800) + 1600182000 <= cell.timestamp) AND
            (cell.timestamp <= (Z2 * 10800) + 1600182000)
        
        // SELECT AVG(pm10) AS pm10_avg, WINDOW 1, 5, 5 
        LET AVG = (
            FOR wcell IN Finedust 
                // I specify 2.5 rather than 2 or 3 due to the floating point op.
                FILTER (wcell.timestamp == cell.timestamp) AND
                    ((cell.latitude - (2.5 * 0.000172998)) < wcell.latitude) AND
                    (wcell.latitude < ((2.5 * 0.000172998) + cell.latitude)) AND
                    ((cell.longitude - (2.5 * 0.000216636)) < wcell.longitude) AND 
                    (wcell.longitude < ((2.5 * 0.000216636) + cell.longitude))
                RETURN wcell.pm10
            )
        LIMIT 522*52
        RETURN {
            timestamp: cell.timestamp,
            latitude: cell.latitude,
            longitude: cell.longitude,
            date: FLOOR((cell.timestamp - 1600182000) / (10800 * 8)),       // as int idx
            pm10_avg: AVERAGE(AVG)
        }
        
)

// (SELECT date, MAX(pm10_avg) AS pm10_max FROM B GROUPBY date) (C subquery)
LET Ct2 = (
    FOR doc IN AB 
        COLLECT date = doc.date
        AGGREGATE pm10_max = MAX(doc.pm10_avg)
        RETURN {date: date, pm10_max: pm10_max}
)

LET C = (
    FOR t1 IN AB
        FOR t2 IN Ct2
            // WHERE t1.pm10_avg = t2.pm10_max AND t1.date = t2.date
            FILTER t1.pm10_avg == t2.pm10_max AND t1.date == t2.date
            // t1.date, t1.location, t1.timestamp
            RETURN {date: t1.date, latitude: t1.latitude, longitude: t1.longitude, timestamp: t1.timestamp}
)

LET D = (
    FOR c IN C
        // ST_ClosestObject(Map,building, C.location)
        LET NEAR = ( 
            FOR map IN Map
                FILTER map.properties.building != NULL
                SORT GEO_DISTANCE([c.longitude, c.latitude], map.geometry) ASC
                LIMIT 1
                RETURN map
        )
        
        // ORDER BY C.date ASC
        SORT c.date ASC
        
        // C.date, C.timestamp, ST_ClosestObject(Map,building, C.location) AS osm_id
        RETURN {
            date: c.date,
            timestamp: c.timestamp,
            osm_id: NEAR[0]
        }
)

 */


/* 
 * [Task15] Fine Dust Cleaning Vehicles.
 *
 * Recommend the route from the current coordinates by analyzing the hotspot between time Z1 and Z2.
 * Use window aggregation with a size of 5. (Graph, Document, Array) -> Relational
 *
 * A =  SELECT (latitude, longitude) AS coordinates, AVG(pm10) AS pm10_avg 
 *      FROM FineDust 
 *      WHERE timestamp >= Z1 
 *          AND timestamp <= Z2 
 *      WINDOW *, 5, 5 // Array
 * B =  SELECT ShortestPath(RoadNode, startNode: ST_ClosestObject(Site, roadnode, current_coordinates), endNode: ST_ClosestObject(Site, roadnode, A.coordinates)) 
 *      FROM A, RoadNode, Site 
 *      WHERE Site.site_id = RoadNode.site_id 
 *      ORDER BY A.pm10_avg DESC
 *      LIMIT 1 // Relational
 * 
 * 
 * [Query]
 * 
 
 LET Z1 = 3
 LET Z2 = 3
  
 LET A = (
    FOR cell IN Finedust
        // WHERE timestamp >= Z1 AND timestamp <= Z2
        FILTER cell.pm10 > 2000
        FILTER ((Z1 * 10800) + 1600182000 <= cell.timestamp) AND
            (cell.timestamp <= (Z2 * 10800) + 1600182000)
        
        // SELECT AVG(pm10) AS pm10_avg, WINDOW *, 5, 5 
        LET AVG = (
            FOR wcell IN Finedust 
                // I specify 2.5 rather than 2 or 3 due to the floating point op.
                FILTER ((Z1 * 10800) + 1600182000 <= wcell.timestamp) AND
                    (wcell.timestamp <= (Z2 * 10800) + 1600182000) AND
                    ((cell.latitude - (2.5 * 0.000172998)) < wcell.latitude) AND
                    (wcell.latitude < ((2.5 * 0.000172998) + cell.latitude)) AND
                    ((cell.longitude - (2.5 * 0.000216636)) < wcell.longitude) AND 
                    (wcell.longitude < ((2.5 * 0.000216636) + cell.longitude))
                RETURN wcell.pm10
            )
        LIMIT 522
        RETURN {
            timestamp: cell.timestamp,
            latitude: cell.latitude,
            longitude: cell.longitude,
            date: FLOOR((cell.timestamp - 1600182000) / (10800 * 8)),       // as int idx
            pm10_avg: AVERAGE(AVG)
        }
        
)

LET B = (
    FOR a IN A
        // WHERE A.pm10_avg >= 80
        FILTER a.pm10_avg >= 2000
        // ST_ClosestObject(RoadNode, node, A.location)
        LET map = (
            FOR node IN Roadnode
                LET map = (FOR map IN Map FILTER map.id == node.map_id RETURN map)
                SORT GEO_DISTANCE([a.longitude, a.latitude], map[0].geometry) ASC
                LIMIT 1
                RETURN map
        )
        RETURN map
            
)

RETURN B
 */

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