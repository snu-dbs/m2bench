res = db._query(`

LET Z1 = "2020-06-01T00:00:00.000Z"
LET Z2 = "2020-06-01T02:00:00.000Z"

LET A = (FOR e IN Earthquake
        FILTER e.time >= Z1  && e.time < Z2
        RETURN {eqk_id: e.earthquake_id, eqk_lat: e.latitude, eqk_lon: e.longitude} )

LET B = (FOR s IN Site FILTER s.properties.type == "roadnode"
            FOR a IN A
            FILTER GEO_DISTANCE([a.eqk_lon, a.eqk_lat], s.geometry) <= 5000
            RETURN {roadnode_id: s.site_id} )

LET C = ( FOR c IN (FOR b IN B
                        FOR node IN Roadnode
                          FILTER node.site_id == b.roadnode_id RETURN node)
        FOR v, e IN OUTBOUND c GRAPH 'Road_Network'
        RETURN {c, v, e})

RETURN length(C) `);

res2 = res.getExtra();

print(res)
print ( res2['stats']['executionTime'] )

