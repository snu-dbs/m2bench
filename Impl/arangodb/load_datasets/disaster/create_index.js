db.Earthquake.ensureIndex({type:"persistent",fields:["earthquake_id"]})
db.Earthquake.ensureIndex({type: "geo", fields: ["latitude", "longitude"]})
db.Earthquake.ensureIndex({ type: "persistent", fields: [ "magnitude" ] })

db.Finedust_idx.ensureIndex({type:"hash", fields: ["timestamp"]})
db.Finedust_idx.ensureIndex({type: "persistent", fields: ["timestamp", "latitude", "longitude"]})
db.Finedust_idx.ensureIndex({type:  "persistent", fields: ["latitude", "longitude"] })

// db.Finedust.ensureIndex({type:"persistent", fields: ["timestamp"]})
// db.Finedust.ensureIndex({type:"persistent", fields: ["longitude"]})
// db.Finedust.ensureIndex({type:"persistent", fields: ["latitude"]})
// db.Finedust.ensureIndex({type: "persistent", fields: ["timestamp", "longitude", "latitude"], inBackground: true})
// db.Finedust.ensureIndex({type: "persistent", fields: ["timestamp", "longitude", "latitude", "pm10"]})
// db.Finedust.ensureIndex({type: "hash", fields: ["timestamp", "longitude", "latitude"]})

db.Gps.ensureIndex({type: "persistent", fields: ["gps_id"]})
db.Gps.ensureIndex({type: "geo", fields: ["latitude", "longitude"]})

db.Roadnode.ensureIndex({type: "persistent", fields: ["site_id"]})

db.Shelter.ensureIndex({type: "persistent", fields: ["shelter_id"]})
db.Shelter.ensureIndex({type: "persistent", fields: ["site_id"]})

db.Site.ensureIndex({ type: "geo", fields: ["geometry"], geoJson:true})
db.Site.ensureIndex({ type: "persistent", fields: ["site_id"]})
db.Site.ensureIndex({ type: "persistent", fields: [ "properties.type" ] })

db.Site_centroid.ensureIndex({ type: "geo", fields: ["centroid"], geoJson:true})
db.Site_centroid.ensureIndex({ type: "persistent", fields: ["site_id"]})
db.Site_centroid.ensureIndex({ type: "persistent", fields: [ "properties.type" ] })

