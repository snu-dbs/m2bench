res = db._query(`
LET Z1 = "2020-09-17T00:00:00.000Z"
LET Z2 = "2020-09-17T01:00:00.000Z"

LET A =(FOR a IN (FOR s IN Shelter
                    FOR sc IN Site_centroid FILTER s.site_id == sc.site_id && sc.properties.type == "building" && sc.properties.description == "hospital"
                    RETURN {shelter_id: s.shelter_id, shelter_geom: sc.centroid})
            FOR fGps IN (FOR g IN Gps FILTER g.time >= Z1  && g.time < Z2 RETURN {gps_id: g.gps_id, lat: g.latitude, lon: g.longitude})
                FILTER GEO_DISTANCE(a.shelter_geom, [fGps.lon, fGps.lat]) <= 5000
                COLLECT shelter_id = a.shelter_id, shelter_geom = a.shelter_geom WITH COUNT into numGps
                SORT numGps DESC
                LIMIT 1
	RETURN {shelter_id: shelter_id, shelter_geom: shelter_geom})

LET B = (FOR s in Site
            FILTER s.properties.type == "roadnode"
            SORT GEO_DISTANCE(A[0].shelter_geom, s.geometry) ASC
            LIMIT 1
        RETURN {roadnode_id: s.site_id})

LET C = (FOR fBuilding IN (FOR sc IN Site_centroid
                            FILTER sc.properties.type == "building" && sc.properties.description == "school" && GEO_DISTANCE(A[0].shelter_geom, sc.centroid)<=1000
                            RETURN sc)
    LET temp = (FOR s in Site FILTER s.properties.type == "roadnode"
                SORT GEO_DISTANCE(fBuilding.centroid, s.geometry) ASC
                LIMIT 1
                RETURN s)
    RETURN {building_id: fBuilding.site_id, roadnode_id: temp[0].site_id})

LET shelter_node = (FOR n IN Roadnode FILTER n.site_id == B[0].roadnode_id RETURN {shelter_id: A[0].shelter_id, node: n._id})
LET building_nodes = (FOR n IN Roadnode FOR c IN C FILTER n.site_id == c.roadnode_id RETURN {building_id: c.building_id, node: n._id})

LET D = (FOR b IN building_nodes
            LET path = (FOR v, e IN OUTBOUND SHORTEST_PATH shelter_node[0].node TO b.node Road OPTIONS {weightAttribute: 'distance'}
                        RETURN e.distance)
            LET path_cost = SUM(path)
            SORT path_cost, b.building_id
            LIMIT 5
        RETURN {building_id: b.building_id})

RETURN length(D) `);

res2 = res.getExtra();
print(res)
print ( res2['stats']['executionTime'] )
