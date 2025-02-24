res = db._profileQuery(
  `
    LET Z1 = 30
    LET Z2 = 40
    LET interval = 10800

    LET lat_min = 34.01189870
    LET arrayinfo_lat_grid_interval = 0.000172998

    LET lon_min = -118.3450100223
    LET arrayinfo_lon_grid_interval = 0.000216636

    LET lat_max = arrayinfo_lat_grid_interval * 522 + lat_min
    LET lon_max = arrayinfo_lon_grid_interval * 522 + lon_min

    LET B = (
        FOR site IN Site 
            FILTER site.properties.type == "building"
            FILTER site.properties.description == "school"

            LET multipolygon = site.geometry.coordinates

            FOR n1 IN multipolygon
                FOR n2 IN n1
                    FOR n3 IN n2
                        COLLECT site_id = site.site_id
                        AGGREGATE lon = AVERAGE(n3[0]), lat = AVERAGE(n3[1])
                        RETURN { site_id, lon, lat }
    )

    LET A = (
        FOR finedust IN Finedust_idx
            FILTER finedust.timestamp >= Z1
            FILTER finedust.timestamp <= Z2 
            COLLECT latitude = finedust.latitude, longitude = finedust.longitude
            AGGREGATE avg_pm10 = AVG(finedust.pm10)
            RETURN { latitude, longitude, avg_pm10 }
    )

    LET C = (
        FOR b IN B
            FILTER b.lat >= lat_min 
            FILTER b.lon >= lon_min 
            FILTER b.lat <= lat_max
            FILTER b.lon <= lon_max
            FOR a IN A 
                FILTER (b.lat - 34.01189870) / arrayinfo_lat_grid_interval <= a.latitude + 1 
                FILTER (b.lat - 34.01189870) / arrayinfo_lat_grid_interval >= a.latitude
                FILTER (b.lon - (-118.3450100223)) / arrayinfo_lon_grid_interval <= a.longitude + 1
                FILTER (b.lon - (-118.3450100223)) / arrayinfo_lon_grid_interval >= a.longitude 
                RETURN { lat: b.lat, lon: b.lon, pm10: a.avg_pm10 }
                // RETURN { site_id: b.site_id, pm10: a.avg_pm10 } // Answer validation
    )

    RETURN LENGTH(C)
    // RETURN C // Answer Validation
`,
  {},
  { colors: false }
);

// res2 = res.getExtra();

// print(res);
// print(res2["stats"]["executionTime"]);

// Answer Validation
// const fs = require("fs");

// let data = res.toArray();

// let csvContent = "site_id,pm10\n";

// data[0].forEach((row) => {
//   csvContent += `${row.site_id},${row.pm10}\n`;
// });

// let filePath = "/tmp/t16.csv";
// fs.write(filePath, csvContent);
