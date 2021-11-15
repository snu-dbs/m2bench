res1 = db._query(`

FOR eqk IN Earthquake FILTER eqk.earthquake_id == 41862
	LET X_latitude = eqk.latitude
        LET X_longitude = eqk.longitude
        LET X_time = eqk.time

LET A = (FOR fGps IN (FOR g IN Gps
                        FILTER GEO_DISTANCE([X_longitude, X_latitude], [g.longitude, g.latitude]) <= 10000 && g.time >= X_time && g.time < DATE_ADD(X_time, "PT1H")
                        RETURN {id: g.gps_id, lat: g.latitude, lon: g.longitude} )
         LET temp = (FOR s in Site FILTER s.properties.type == "roadnode"
                        SORT GEO_DISTANCE([fGps.lon, fGps.lat], s.geometry) ASC
                        LIMIT 1
                        RETURN s)
         RETURN {gps_id: fGps.id, roadnode_id: temp[0].site_id})

LET B = (FOR fShelter IN (FOR s IN Shelter
                            FOR sc IN Site_centroid
                            FILTER s.site_id == sc.site_id && GEO_DISTANCE([X_longitude, X_latitude], sc.centroid) <= 15000
                            RETURN {shelter_id: s.shelter_id, geom: sc.centroid})
        LET temp = (FOR s in Site FILTER s.properties.type == "roadnode"
                        SORT GEO_DISTANCE(fShelter.geom, s.geometry) ASC
                        LIMIT 1
                        RETURN s)
        RETURN {shelter_id: fShelter.shelter_id, roadnode_id: temp[0].site_id})

LET gps_nodes = (FOR n IN Roadnode FOR a IN A FILTER n.site_id == a.roadnode_id RETURN {gps_id: a.gps_id, node: n._id})
LET shelter_nodes = (FOR n IN Roadnode FOR b IN B FILTER n.site_id == b.roadnode_id RETURN {shelter_id: b.shelter_id, node: n._id})

LET C = (FOR src IN gps_nodes
            FOR dst IN shelter_nodes
                LET path = (FOR v, e IN OUTBOUND SHORTEST_PATH src.node TO dst.node GRAPH 'Road_Network' OPTIONS {weightAttribute: 'distance'}
                            RETURN e.distance)
         RETURN {gps_id: src.gps_id, shelter_id: dst.shelter_id, cost: SUM(path)} )

FOR c IN C
    INSERT c INTO T11_temp`);

res2 = db._query(` RETURN length(T11_temp) `);

res1_t = res1.getExtra();
res2_t = res2.getExtra();

print(res1)
print(res2)
print ( res1_t['stats']['executionTime'] + res2_t['stats']['executionTime'])
