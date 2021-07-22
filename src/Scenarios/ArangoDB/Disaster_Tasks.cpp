//
// Created by mxmdb on 21. 5. 3..
//

void T10(){

//    LET Z1 = "2020-06-01T00:00:00.000Z"
//    LET Z2 = "2020-06-01T02:00:00.000Z"
//
//    LET A = (FOR e IN Earthquake
//    FILTER e.time >= Z1  && e.time <= Z2
//    RETURN {eqk_id: e.earthquake_id, eqk_lat: e.latitude, eqk_lon: e.longitude} )
//
//    LET B = (FOR s IN Site FILTER s.properties.type == "roadnode"
//    FOR a IN A
//    FILTER GEO_DISTANCE([a.eqk_lon, a.eqk_lat], s.geometry) <= 5000
//    RETURN {site_id: s.site_id} )
//
//    LET C = (FOR b IN B
//    FOR node IN Roadnode
//    FILTER node.site_id == b.site_id
//    RETURN node)
//
//    LET D = (FOR c IN C
//    FOR v, e IN OUTBOUND c Road
//    RETURN {c, v, e})
//
//    RETURN length(D)

}

void T11(){

//    FOR eqk IN Earthquake FILTER eqk.earthquake_id == 41862
//        LET X_latitude = eqk.latitude
//        LET X_longitude = eqk.longitude
//        LET X_time = eqk.time
//
//
//    LET A = (FOR g IN Gps
//    FILTER GEO_DISTANCE([X_longitude, X_latitude], [g.longitude, g.latitude]) <= 10000 && g.time >= X_time && g.time <= DATE_ADD(X_time, "PT1H")
//    RETURN {id: g.gps_id, lat: g.latitude, lon: g.longitude} )
//
//    LET B = (FOR a IN A
//    LET temp = (FOR s in Site FILTER s.properties.type == "roadnode"
//    SORT GEO_DISTANCE([a.lon, a.lat], s.geometry) ASC
//    LIMIT 1
//    RETURN s)
//    RETURN {gps_id: a.id, closest_node_site_id: temp[0].site_id})
//
//    LET gps_nodes = (FOR n IN Roadnode
//    FOR b IN B
//    FILTER n.site_id == b.closest_node_site_id
//    RETURN {gps_id: b.gps_id, node: n})
//
//    LET C = (FOR sh IN Shelter
//    FOR s IN Site
//    FILTER sh.site_id == s.site_id && GEO_DISTANCE([X_longitude, X_latitude], s.geometry) <= 15000
//    RETURN {shelter_id: sh.shelter_id, geom: s.geometry })
//
//    LET D = (FOR c IN C
//    LET temp = (FOR s in Site FILTER s.properties.type == "roadnode"
//    SORT GEO_DISTANCE(c.geom, s.geometry) ASC
//    LIMIT 1
//    RETURN s)
//    RETURN {shelter_id: c.shelter_id, closest_node_site_id: temp[0].site_id}
//    )
//
//    LET shelter_nodes = (FOR n IN Roadnode
//    FOR d IN D
//    FILTER n.site_id == d.closest_node_site_id
//    RETURN {shelter_id: d.shelter_id, node: n})
//
//    LET E = (FOR src IN gps_nodes
//                  FOR dst IN shelter_nodes
//                      LET path = (FOR v, e IN OUTBOUND SHORTEST_PATH src.node TO dst.node Road
//                                  OPTIONS {weightAttribute: 'distance'}
//                                  RETURN e.distance )
//     RETURN {gps_id: src.gps_id, shelter_id: dst.shelter_id, cost: SUM(path)} )
//
//    FOR i IN E RETURN i

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
 *      WHERE ShortestPath(RoadNode, startNode:B.node_id, endNode:C.node_id) <= 2km //Relational
 */
void T12(){

    //    db.T12_temp.drop()
    //    db._create("T12_temp")

    /* STEP 1 */
//    LET Z1 = "2020-09-17T00:00:00.000Z"
//    LET Z2 = "2020-09-17T01:00:00.000Z"
//
//    LET A = (FOR s IN Shelter
//            FOR site IN Site FILTER s.site_id == site.site_id
//            RETURN {shelter_id: s.shelter_id, site: site})
//
//    LET B = (FOR a IN A
//                FOR g IN (FOR g IN Gps FILTER g.time >= Z1  && g.time <= Z2 RETURN {gps_id: g.gps_id, lat: g.latitude, lon: g.longitude})
//                FILTER GEO_DISTANCE(a.site.geometry, [g.lon, g.lat]) <= 5000
//                COLLECT shelter_id = a.shelter_id, shelter_site = a.site WITH COUNT into numGps
//                SORT numGps DESC
//                LIMIT 1
//            RETURN {shelter_id, shelter_site})
//
//    LET C = (FOR site in Site
//                FILTER site.properties.type == "roadnode"
//                SORT GEO_DISTANCE(B[0].shelter_site.geometry, site.geometry) ASC
//                LIMIT 1
//                RETURN site)
//
//    FOR n IN Roadnode
//        FILTER n.site_id == C[0].site_id
//        INSERT {shelter_id: B[0].shelter_id, geom: B[0].shelter_site.geometry, closest_roadnode: n} INTO T12_temp

    /* STEP 2 */
//    LET shelter = T12_temp[0]
//
//    LET D = (FOR s IN Site
//                FILTER s.properties.type == "building" && GEO_DISTANCE(shelter.geom, s.geometry) <=1000
//                RETURN s)
//
//    LET E = (FOR d IN D
//                LET temp = (FOR s in Site FILTER s.properties.type == "roadnode"
//                                SORT GEO_DISTANCE(d.geometry, s.geometry) ASC
//                                LIMIT 1
//                                RETURN s)
//                RETURN {building_id: d.site_id, closest_node_site_id: temp[0].site_id})
//
//    LET buildings = (FOR n IN Roadnode
//                        FOR e IN E
//                        FILTER n.site_id == e.closest_node_site_id
//                        RETURN {building_id: e.building_id, closest_roadnode: n})
//
//    LET F = (FOR b IN buildings
//                LET path = ( FOR v, e IN OUTBOUND SHORTEST_PATH shelter.closest_roadnode TO b.closest_roadnode Road
//                                RETURN e.distance)
//                LET path_cost = SUM(path)
//                //FILTER path_cost > 0
//                SORT path_cost
//                LIMIT 5
//    RETURN {shelter_id: T12_temp[0].shelter_id, building_id: b.building_id, cost: path_cost})
//
//    FOR i IN F return i


//    For t in T12_temp REMOVE t in T12_temp
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
 
  LET A = (
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

 
 LET B = (
    FOR cell IN A
        COLLECT latitude = cell.latitude, longitude = cell.longitude INTO g
        
        RETURN {
            latitude: latitude,
            longitude: longitude,
            pm10_avg : (SUM(g[*].cell.pm10_sum) / SUM(g[*].cell.pm10_count)) 
        }
    
 )


LET Ct1 = (
    RETURN MAX(FOR cell in B RETURN cell.pm10_avg)
)

LET Ct2 = (
    FOR cell IN B
        FILTER cell.pm10_avg == Ct1[0]
        RETURN cell
)

LET Ct3src = (
    FOR site IN Site
        FILTER site.properties.type == 'roadnode'
        SORT GEO_DISTANCE([CLON, CLAT], site.geometry) ASC
        LIMIT 1
        RETURN site
)

LET Ct4dst = (
    FOR site IN Site
        FILTER site.properties.type == 'roadnode'
        SORT GEO_DISTANCE([-118.34501002237936 + (Ct2[0].longitude * 0.000216636), 34.011898718557454 + (Ct2[0].latitude * 0.000172998)], site.geometry) ASC
        LIMIT 1
        RETURN site
)

LET Ct5src = (
    FOR node IN Roadnode
        FILTER node.site_id == Ct3src[0].site_id
        RETURN node
)

LET Ct6dst = (
    FOR node IN Roadnode
        FILTER node.site_id == Ct4dst[0].site_id
        RETURN node
)

LET C = (
    FOR v, e IN OUTBOUND SHORTEST_PATH Ct5src[0] TO Ct6dst[0] GRAPH 'Road_Network' OPTIONS {weightAttribute: 'distance'}
        RETURN {v, e}
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



// }