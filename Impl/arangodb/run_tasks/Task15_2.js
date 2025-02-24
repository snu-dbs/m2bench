let res1 = db._profileQuery(
  `
    LET Z1 = 10
    LET Z2 = 20
    LET CLON = -118.0614431
    LET CLAT = 34.068509

    FOR site IN Site_centroid
        FILTER site.properties.type == 'roadnode'
        SORT GEO_DISTANCE([CLON, CLAT], site.centroid) ASC
        LIMIT 1
        RETURN site
`,
  {},
  { colors: false }
);

let res2 = db._profileQuery(
  `
    LET Z1 = 10
    LET Z2 = 20
    LET CLON = -118.0614431
    LET CLAT = 34.068509

    LET AB1 = (
        FOR cell IN Finedust_idx
            FILTER (Z1 <= cell.timestamp) AND (cell.timestamp <= Z2)
            LET win = (
                FOR wcell IN Finedust_idx
                    FILTER (cell.timestamp == wcell.timestamp)
                        AND ((cell.latitude - 2) <= wcell.latitude)
                        AND (wcell.latitude <= (2 + cell.latitude))
                        AND ((cell.longitude - 2) <= wcell.longitude)
                        AND (wcell.longitude <= (2 + cell.longitude))
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
                coordinates: [
                    -118.34501002237936 + (longitude * 0.000216636),
                    34.011898718557454 + (latitude * 0.000172998)
                ],
                pm10_avg: (SUM(g[*].cell.pm10_sum) / SUM(g[*].cell.pm10_count))
            }
    )

    FOR cell IN AB
        SORT cell.pm10_avg DESC
            FOR site IN Site_centroid
                FILTER site.properties.type == 'roadnode'
                SORT GEO_DISTANCE([cell.coordinates[0], cell.coordinates[1]], site.centroid) ASC
                LIMIT 1
                RETURN site
`,
  {},
  { colors: false }
);

// let res3 = res1.getExtra();
// let res4 = res2.getExtra();

// print(res1, res2);
// print(res3["stats"]["executionTime"] + res4["stats"]["executionTime"]);

// Answer Validation
// const fs = require("fs");

// let data1 = res1.toArray();
// let data2 = res2.toArray();

// let csvContent = "int4\n";

// data1.forEach((row) => {
//   csvContent += `${row.site_id}\n`;
// });
// data2.forEach((row) => {
//   csvContent += `${row.site_id}\n`;
// });

// let filePath = "/tmp/t15.csv";
// fs.write(filePath, csvContent);
