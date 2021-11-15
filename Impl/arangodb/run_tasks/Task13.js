let res = db._query("LET A=(FOR eq IN Earthquake     FILTER eq.magnitude >= 4.5     FOR site IN Site_centroid     FILTER site.properties.type == 'building'      LET dist = GEO_DISTANCE([eq.longitude, eq.latitude], site.centroid)     FILTER dist <= 30000          COLLECT description = site.properties.description         WITH COUNT INTO cnt          RETURN {description, cnt})  RETURN Length(A)");
let res2 = res.getExtra();

print(res);
print(res2['stats']['executionTime']);
