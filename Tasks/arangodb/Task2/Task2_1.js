   db.V.drop()
   db.W.drop()
   db.H.drop()
   db.WtV.drop()
   db.WtWH.drop()
   db.newH.drop()
   db.VHt.drop()
   db.WHHt.drop()
   db.newW.drop()

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
