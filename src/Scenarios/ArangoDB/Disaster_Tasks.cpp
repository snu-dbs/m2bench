//
// Created by mxmdb on 21. 5. 3..
//

/**
 * [Task 10] Road Network Filtering ([R, D, G]=> G)
 * For the earthquakes which occurred between time Z1 and Z2, find the road network subgraph within 5km from the earthquakes' location.
 *
 * A = SELECT n1, r, n2 AS subgraph FROM Earthquake, Site, RoadNode
 *     WHERE (n1:RoadNode) - [r:Road] -> (n2:RoadNode) AND ST_Distance(Site.geometry, Earthquake.coordinates) <= 5km
 *     AND Earthquake.time >= Z1 AND Earthquake.time < Z2 AND RoadNode.site_id = Site.site_id //Graph
*/
void T10(){

//    LET Z1 = "2020-06-01T00:00:00.000Z"
//    LET Z2 = "2020-06-01T02:00:00.000Z"
//
//    LET A = (FOR e IN Earthquake
//    FILTER e.time >= Z1  && e.time < Z2
//    RETURN {eqk_id: e.earthquake_id, eqk_lat: e.latitude, eqk_lon: e.longitude} )
//
//    LET B = (FOR s IN Site FILTER s.properties.type == "roadnode"
//    FOR a IN A
//    FILTER GEO_DISTANCE([a.eqk_lon, a.eqk_lat], s.geometry) <= 5000
//    RETURN {site_id: s.site_id} )
//
//    LET C = ( FOR c IN (FOR b IN B
//                        FOR node IN Roadnode
//    FILTER node.site_id == b.site_id RETURN node)
//    FOR v, e IN OUTBOUND c Road
//    RETURN {c, v, e})
//
//    RETURN length(C)

}

/**
  *  [Task 11] Closest Shelter ([R, D, G]=> R)
  *  For a given earthquake information X, find the cost of the shortest path for each GPS coordinate and Shelter pair.
  *  (GPS coordinates are limited by 1 hour and 10km from the X. Shelters are limited by 15km from the X.)
  *
  *  A = SELECT GPS.gps_id, ST_ClosestObject(Site, roadnode, GPS.coordinates) AS roadnode_id FROM GPS, Site, RoadNode
  *      WHERE GPS.time >= X.time AND GPS.time < X.time + 1 hour AND ST_Distance(GPS.coordinates, X.coordinates) <= 10km
  *      AND RoadNode.site_id = Site.site_id //Relational
  *
  *  B = SELECT t.shelter_id, ST_ClosestObject(Site, roadnode, ST_Centroid(t.geometry)) AS roadnode_id
  *      FROM RoadNode, Site, (SELECT Shelter.shelter_id, Site.geometry FROM Site, Shelter WHERE ST_Distance(ST_Centroid(Site.geometry), X.coordinates) <= 15km AND Shelter.site_id = Site.site_id) AS t
  *      WHERE RoadNode.site_id = Site.site_id  //Relational
  *
  *  C = SELECT A.gps_id, B.shelter_id, ShortestPath(RoadNode, startNode:A.roadnode_id, endNode:B.roadnode_id) AS cost
  *      FROM A, B, RoadNode //Relational
  */
void T11(){

//    FOR eqk IN Earthquake FILTER eqk.earthquake_id == 41862
//        LET X_latitude = eqk.latitude
//        LET X_longitude = eqk.longitude
//        LET X_time = eqk.time
//
//    LET A = (FOR fGps IN (FOR g IN Gps
//    FILTER GEO_DISTANCE([X_longitude, X_latitude], [g.longitude, g.latitude]) <= 10000 && g.time >= X_time && g.time < DATE_ADD(X_time, "PT1H")
//    RETURN {id: g.gps_id, lat: g.latitude, lon: g.longitude} )
//    LET temp = (FOR s in Site FILTER s.properties.type == "roadnode"
//    SORT GEO_DISTANCE([fGps.lon, fGps.lat], s.geometry) ASC
//    LIMIT 1
//    RETURN s)
//    RETURN {gps_id: fGps.id, roadnode_id: temp[0].site_id})
//
//
//    LET B = (FOR fShelter IN (FOR sh IN Shelter
//                              FOR s IN Site
//    FILTER sh.site_id == s.site_id && GEO_DISTANCE([X_longitude, X_latitude], s.geometry) <= 15000
//    RETURN {shelter_id: sh.shelter_id, geom: s.geometry })
//    LET temp = (FOR s in Site FILTER s.properties.type == "roadnode"
//    SORT GEO_DISTANCE(fShelter.geom, s.geometry) ASC
//    LIMIT 1
//    RETURN s)
//    RETURN {shelter_id: fShelter.shelter_id, roadnode_id: temp[0].site_id}
//    )
//
//    LET gps_nodes = (FOR n IN Roadnode
//    FOR a IN A
//    FILTER n.site_id == a.roadnode_id
//    RETURN {gps_id: a.gps_id, node: n._id})
//
//    LET shelter_nodes = (FOR n IN Roadnode
//    FOR b IN B
//    FILTER n.site_id == b.roadnode_id
//    RETURN {shelter_id: b.shelter_id, node: n._id})
//
//    LET C = (FOR src IN gps_nodes
//    FOR dst IN shelter_nodes
//    LET path = (FOR v, e IN OUTBOUND SHORTEST_PATH src.node TO dst.node Road
//            OPTIONS {weightAttribute: 'distance'}
//    RETURN e.distance )
//    RETURN {gps_id: src.gps_id, shelter_id: dst.shelter_id, cost: SUM(path)} )
//
//    FOR doc IN C
//	    COLLECT WITH COUNT INTO length
//	    RETURN length

}

/**
 * [Task 12] New Shelter ([R, D, G]=> R)
 * For the shelter of which the number of GPS coordinates are the most within 5km from the shelter between time Z1 and Z2,
 * find five closest buildings from the shelter. The buildings are limited by 1km from the shelter.
 *
 * A = SELECT Shelter.shelter_id, Site.geometry
 *     FROM Shelter, Site, (SELECT Shelter.shelter_id, COUNT(GPS.gps_id) AS cnt FROM Shelter, GPS, Site
 *                          WHERE ST_Distance(GPS.coordinates, ST_Centroid(Site.geometry)) <= 5km AND Site.site_id = Shelter.site_id
 *                          AND GPS.time >= Z1 AND GPS.time < Z2 GROUP BY Shelter.id ORDER BY cnt DESC LIMIT 1) AS t
 *     WHERE Shelter.shelter_id = t.shelter_id AND Site.site_id = Shelter.site_id  //Relational
 *
 * B = SELECT A.shelter_id, ST_ClosestObject(Site, roadnode, ST_centroid(A.geometry)) AS roadnode_id
 *     FROM A, RoadNode, Site
 *     WHERE Site.site_id = RoadNode.site_id  //Relational
 *
 * C = SELECT t.site_id, ST_ClosestObject(Site, roadnode, ST_Centroid(t.geometry)) AS roadnode_id
 *     FROM Site, RoadNode, (SELECT Site.site_id, Site.geometry FROM Site, A
 *                           WHERE ST_Distance(Site.geometry, ST_centroid(A.geometry)) <= 1km AND Site.properties.type = 'building') AS t
 *     WHERE RoadNode.site_id = Site.site_id  //Relational
 *
 * D = SELECT C.site_id, ShortestPath(RoadNode, startNode:B.roadnode_id, endNode:C.roadnode_id) AS cost
 *     FROM B, C, RoadNode
 *     ORDER BY cost LIMIT 5  //Relational
 */
void T12(){

//    LET Z1 = "2020-09-17T00:00:00.000Z"
//    LET Z2 = "2020-09-17T01:00:00.000Z"
//
//    LET A = (FOR a IN (FOR s IN Shelter FOR site IN Site FILTER s.site_id == site.site_id && site.properties.description == "hospital"
//    RETURN {shelter_id: s.shelter_id, site: site})
//    FOR g IN (FOR g IN Gps FILTER g.time >= Z1  && g.time < Z2 RETURN {gps_id: g.gps_id, lat: g.latitude, lon: g.longitude})
//    FILTER GEO_DISTANCE(a.site.geometry, [g.lon, g.lat]) <= 5000
//    COLLECT shelter_id = a.shelter_id, shelter_site = a.site WITH COUNT into numGps
//    SORT numGps DESC
//    LIMIT 1
//    RETURN {shelter_id: shelter_id, shelter_site_id: shelter_site.site_id})
//
//    LET shelter_geom = (FOR s IN Site
//    FILTER s.site_id == A[0].shelter_site_id
//    RETURN s.geometry)
//
//    LET B = (FOR site in Site
//    FILTER site.properties.type == "roadnode"
//    SORT GEO_DISTANCE(shelter_geom[0], site.geometry) ASC
//    LIMIT 1
//    RETURN {roadnode_id: site.site_id})
//
//    LET C = (FOR d IN (FOR s IN Site
//    FILTER s.properties.type == "building" && s.properties.description == "school" && GEO_DISTANCE(shelter_geom[0], s.geometry) <=1000
//    RETURN s)
//    LET temp = (FOR s in Site FILTER s.properties.type == "roadnode"
//    SORT GEO_DISTANCE(d.geometry, s.geometry) ASC
//    LIMIT 1
//    RETURN s)
//    RETURN {building_id: d.site_id, roadnode_id: temp[0].site_id})
//
//    LET target_shelter = (FOR n IN Roadnode
//    FILTER n.site_id == B[0].roadnode_id
//    RETURN {shelter_id: A[0].shelter_id, node: n._id})
//
//    LET buildings = (FOR n IN Roadnode
//    FOR c IN C
//    FILTER n.site_id == c.roadnode_id
//    RETURN {building_id: c.building_id, node: n._id})
//
//
//    LET D = (FOR b IN buildings
//    LET path = (FOR v, e IN OUTBOUND SHORTEST_PATH target_shelter[0].node TO b.node Road
//            OPTIONS {weightAttribute: 'distance'}
//    RETURN e.distance )
//    LET path_cost = SUM(path)
//    SORT path_cost, b.building_id
//    LIMIT 5
//    RETURN {building_id: b.building_id})
//
//    RETURN length(D)

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
[Query]
LET A=(FOR eq IN Earthquake
    FILTER eq.magnitude >= 4.5
    FOR site IN Site
	    FILTER site.properties.type == 'building'

	    LET dist = GEO_DISTANCE([eq.longitude, eq.latitude], site.geometry)
	    FILTER dist <= 30000
	    
	    COLLECT description = site.properties.description
	        WITH COUNT INTO cnt
	    
	    RETURN {description, cnt})

RETURN Length(A)
 */

void T13(){

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
 [Query]
 LET Z1 = 5
 LET Z2 = 10
  
 LET AB = (
    FOR cell IN Finedust_idx
        FILTER (Z1 <= cell.timestamp) AND (cell.timestamp <= Z2)
        
        LET AVG = (
            FOR wcell IN Finedust_idx 
                FILTER (wcell.timestamp == cell.timestamp) AND
                    ((cell.latitude - 2) <= wcell.latitude) AND
                    (wcell.latitude <= (2 + cell.latitude)) AND
                    ((cell.longitude - 2) <= wcell.longitude) AND
                    (wcell.longitude <= (2 + cell.longitude))
                RETURN wcell.pm10
            )

        RETURN {
            timestamp: cell.timestamp,
            latitude: cell.latitude,
            longitude: cell.longitude,
            date: FLOOR(cell.timestamp / 8),
            pm10_avg: AVERAGE(AVG)
        }
        
)

LET Ct2 = (
    FOR doc IN AB 
        COLLECT date = doc.date
        AGGREGATE pm10_max = MAX(doc.pm10_avg)
        RETURN {date: date, pm10_max: pm10_max}
)

LET C = (
    FOR t1 IN AB
        FOR t2 IN Ct2
            FILTER t1.pm10_avg == t2.pm10_max AND t1.date == t2.date
            RETURN {m: t2.pm10_max, date: t1.date, latitude: t1.latitude, longitude: t1.longitude, timestamp: t1.timestamp}
)

LET D = (
    FOR c IN C
        LET NEAR = ( 
            FOR site IN Site
                FILTER site.properties.type == 'building'
                SORT GEO_DISTANCE([-118.34501002237936 + (c.longitude * 0.000216636), 34.011898718557454 + (c.latitude * 0.000172998)], site.geometry) ASC
                LIMIT 1
                RETURN site
        )
        
        SORT c.date ASC
        
        RETURN {
            date: c.date,
            timestamp: c.timestamp,
            site_id: NEAR[0].site_id
        }
)

RETURN Length(D)
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
LET Z1 = 5
LET Z2 = 10
LET CLON = -118.0614431
LET CLAT = 34.068509

  LET AB1 = (
    FOR cell IN Finedust_idx
        FILTER (Z1 <= cell.timestamp) AND (cell.timestamp <= Z2)
        LET win = (
            FOR wcell IN Finedust_idx 
                FILTER (cell.timestamp == wcell.timestamp) AND
                    ((cell.latitude - 2) <= wcell.latitude) AND
                    (wcell.latitude <= (2 + cell.latitude)) AND
                    ((cell.longitude - 2) <= wcell.longitude) AND
                    (wcell.longitude <= (2 + cell.longitude))
                RETURN wcell.pm10
            )
            
        RETURN {
            latitude: cell.latitude,
            longitude: cell.longitude,
            pm10_sum: SUM(win[*]),
            pm10_count: COUNT(win[*])
        }
    
)
 
 LET AB = (
    FOR cell IN AB1
        COLLECT latitude = cell.latitude, longitude = cell.longitude INTO g
        
        RETURN {
			coordinates: [-118.34501002237936 + (longitude * 0.000216636), 34.011898718557454 + (latitude * 0.000172998)],
            pm10_avg : (SUM(g[*].cell.pm10_sum) / SUM(g[*].cell.pm10_count)) 
        }
    
 )

LET C = (
	FOR cell IN AB
		SORT cell.pm10_avg DESC
		
		LET s1 = (
			FOR site IN Site
    	        FILTER site.properties.type == 'roadnode'
        	    SORT GEO_DISTANCE([CLON, CLAT], site.geometry) ASC
                LIMIT 1
                RETURN site
		)
		LET src = (
			FOR node IN Roadnode
				FILTER s1[0].site_id == node.site_id
				LIMIT 1
				RETURN node
		)
		LET s2 = (
			FOR site IN Site
    	        FILTER site.properties.type == 'roadnode'
    	        SORT GEO_DISTANCE([cell.coordinates[0], cell.coordinates[1]], site.geometry) ASC
    	        LIMIT 1
                RETURN site
		)
		LET dst = (
			FOR node IN Roadnode
				FILTER s2[0].site_id == node.site_id
				LIMIT 1
				RETURN node
		)
		
		LET path = (
			FOR v, e IN OUTBOUND SHORTEST_PATH src[0] TO dst[0] GRAPH 'Road_Network' OPTIONS {weightAttribute: 'distance'}
        RETURN {v, e}
		)
		
		LIMIT 1
		RETURN path
)

RETURN Length(C)
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


// void T16(){


//  LET Z1 = 1600182000+3*10800
//  LET Z2 = 1600182000+4*10800
//  LET interval = 10800
   

// LET lat_min = 34.01189870
// LET arrayinfo_lat_grid_interval =  0.000172998

// LET lon_min = -118.3450100223
// LET arrayinfo_lon_grid_interval = 0.000216636

// LET lat_max = arrayinfo_lat_grid_interval*522+lat_min
// LET lon_max = arrayinfo_lon_grid_interval*522+lon_min


 

            
// Let B = (For site in Site 
//             Filter site.properties.type == "building"
//             Filter site.properties.description == "school"

//             Let multipolygon = site.geometry.coordinates
            
//             For n1 in multipolygon
//                     For n2 in n1
//                         For n3 in n2 
                            
//                             Collect site_id = site.site_id
//                             AGGREGATE lon = AVERAGE(n3[0]), lat = AVERAGE(n3[1])
//                             Return {site_id, lon, lat}
            
//         )


// Let A = (For finedust in Finedust_idx
//             Filter finedust.timestamp >= (Z1 - 1600182000)/interval
//             Filter finedust.timestamp <= (Z2 - 1600182000)/interval
//             Collect latitude = finedust.latitude, longitude = finedust.longitude
//             Aggregate avg_pm10 = Avg(finedust.pm10)
//             Return {latitude, longitude, avg_pm10})
 
// Let C = (For b in B
//             Filter b.lat >= lat_min 
//             Filter b.lon >= lon_min 
//             Filter b.lat <= lat_max
//             Filter b.lon <= lon_max 
//             For a in A 
//                 Filter (b.lat - 34.01189870)/arrayinfo_lat_grid_interval <= a.latitude + 1 
//                 Filter (b.lat - 34.01189870)/arrayinfo_lat_grid_interval >= a.latitude
//                 Filter (b.lon - (-118.3450100223))/arrayinfo_lon_grid_interval <= a.longitude + 1
//                 Filter (b.lon - (-118.3450100223))/arrayinfo_lon_grid_interval >= a.longitude 
//                 Return {lat:b.lat, lon:b.lon, pm10:a.avg_pm10})
        
// Return(Length(C))

/// The below is deprecated 

//  LET Z1 = 1600182000+3*10800
//  LET Z2 = 1600182000+4*10800
 
   

// LET lat_min = 34.01189870
// LET arrayinfo_lat_grid_interval =  0.000172998

// LET lon_min = -118.3450100223
// LET arrayinfo_lon_grid_interval = 0.000216636

// LET lat_max = arrayinfo_lat_grid_interval*522+lat_min
// LET lon_max = arrayinfo_lon_grid_interval*522+lon_min


 

            
// Let B = (For site in Site 
//             Filter site.properties.type == "building"
//             Filter site.properties.description == "school"

//             Let multipolygon = site.geometry.coordinates
            
//             For n1 in multipolygon
//                     For n2 in n1
//                         For n3 in n2 
                            
//                             Collect site_id = site.site_id
//                             AGGREGATE lon = AVERAGE(n3[0]), lat = AVERAGE(n3[1])
//                             Return {site_id, lon, lat}
            
//         )


// Let A = (For finedust in Finedust
//             Filter finedust.timestamp >= Z1
//             Filter finedust.timestamp <= Z2
//             Collect latitude = finedust.latitude, longitude = finedust.longitude
//             Aggregate avg_pm10 = Avg(finedust.pm10)
//             Return {latitude, longitude, avg_pm10})
 
// Let C = (For b in B
//             Filter b.lat >= lat_min 
//             Filter b.lon >= lon_min 
//             Filter b.lat <= lat_max
//             Filter b.lon <= lon_max 
//             For a in A 
//                 Filter b.lat-0.000172998 <= a.latitude 
//                 Filter b.lat >= a.latitude
//                 Filter b.lon-0.000216636 <= a.longitude
//                 Filter b.lon >= a.longitude 
//                 Return {lat:b.lat, lon:b.lon, pm10:a.avg_pm10})
        
// Return(Length(C))



// }