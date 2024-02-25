
if (db._collection("V") != null) db.V.drop();
if (db._collection("W") != null) db.W.drop();
if (db._collection("H") != null) db.H.drop();
if (db._collection("WtV") != null) db.WtV.drop();
if (db._collection("WtWH") != null) db.WtWH.drop();
if (db._collection("newH") != null) db.newH.drop();
if (db._collection("VHt") != null) db.VHt.drop();
if (db._collection("WHHt") != null) db.WHHt.drop();
if (db._collection("newW") != null) db.newW.drop();

db._create("V")
db._create("W")
db._create("H")
db._create("WtV")
db._create("WtWH")
db._create("newH")
db._create("VHt")
db._create("WHHt")
db._create("newW")

db.V.ensureIndex({type:"persistent", fields:["product_id"]})
db.V.ensureIndex({type:"persistent", fields:["customer_id"]})

db.H.ensureIndex({type:"persistent", fields:["feature_id"]})
db.H.ensureIndex({type:"persistent", fields:["product_id"]})
db.H.ensureIndex({type:"persistent", fields:["product_id", "feature_id"]})

db.W.ensureIndex({type:"persistent", fields:["customer_id"]})
db.W.ensureIndex({type:"persistent", fields:["feature_id"]})
db.W.ensureIndex({type:"persistent", fields:["feature_id", "customer_id"]})

db.WtV.ensureIndex({type:"persistent", fields:["feature_id", "product_id"]})
db.WtV.ensureIndex({type:"persistent", fields:["product_id"]})
db.WtV.ensureIndex({type:"persistent", fields:["feature_id"]})

db.WtWH.ensureIndex({type:"persistent", fields:["feature_id", "product_id"]})
db.WtWH.ensureIndex({type:"persistent", fields:["product_id"]})
db.WtWH.ensureIndex({type:"persistent", fields:["feature_id"]})

db.newH.ensureIndex({type:"persistent", fields:["feature_id", "product_id"]})
db.newH.ensureIndex({type:"persistent", fields:["product_id"]})
db.newH.ensureIndex({type:"persistent", fields:["feature_id"]})


db.VHt.ensureIndex({type:"persistent", fields:["feature_id", "customer_id"]})
db.VHt.ensureIndex({type:"persistent", fields:["customer_id"]})
db.VHt.ensureIndex({type:"persistent", fields:["feature_id"]})

db.WHHt.ensureIndex({type:"persistent", fields:["feature_id", "customer_id"]})
db.WHHt.ensureIndex({type:"persistent", fields:["customer_id"]})
db.WHHt.ensureIndex({type:"persistent", fields:["feature_id"]})

db.newW.ensureIndex({type:"persistent", fields:["feature_id", "customer_id"]})
db.newW.ensureIndex({type:"persistent", fields:["customer_id"]})
db.newW.ensureIndex({type:"persistent", fields:["feature_id"]})
