const collections = ["V", "W", "H", "WtV", "WtWH", "newH", "VHt", "WHHt", "newW"];

collections.forEach(col => {
    if (db._collection(col) !== null) db[col].drop();
});

collections.forEach(col => db._create(col));

db.V.ensureIndex({ type: "persistent", fields: ["product_id"] });
db.V.ensureIndex({ type: "persistent", fields: ["customer_id"] });

db.H.ensureIndex({ type: "persistent", fields: ["feature_id"] });
db.H.ensureIndex({ type: "persistent", fields: ["product_id"] });
db.H.ensureIndex({ type: "persistent", fields: ["product_id", "feature_id"] });

db.W.ensureIndex({ type: "persistent", fields: ["customer_id"] });
db.W.ensureIndex({ type: "persistent", fields: ["feature_id"] });
db.W.ensureIndex({ type: "persistent", fields: ["feature_id", "customer_id"] });

db.WtV.ensureIndex({ type: "persistent", fields: ["feature_id", "product_id"] });
db.WtV.ensureIndex({ type: "persistent", fields: ["product_id"] });
db.WtV.ensureIndex({ type: "persistent", fields: ["feature_id"] });

db.WtWH.ensureIndex({ type: "persistent", fields: ["feature_id", "product_id"] });
db.WtWH.ensureIndex({ type: "persistent", fields: ["product_id"] });
db.WtWH.ensureIndex({ type: "persistent", fields: ["feature_id"] });

db.newH.ensureIndex({ type: "persistent", fields: ["feature_id", "product_id"] });
db.newH.ensureIndex({ type: "persistent", fields: ["product_id"] });
db.newH.ensureIndex({ type: "persistent", fields: ["feature_id"] });

db.VHt.ensureIndex({ type: "persistent", fields: ["feature_id", "customer_id"] });
db.VHt.ensureIndex({ type: "persistent", fields: ["customer_id"] });
db.VHt.ensureIndex({ type: "persistent", fields: ["feature_id"] });

db.WHHt.ensureIndex({ type: "persistent", fields: ["feature_id", "customer_id"] });
db.WHHt.ensureIndex({ type: "persistent", fields: ["customer_id"] });
db.WHHt.ensureIndex({ type: "persistent", fields: ["feature_id"] });

db.newW.ensureIndex({ type: "persistent", fields: ["feature_id", "customer_id"] });
db.newW.ensureIndex({ type: "persistent", fields: ["customer_id"] });
db.newW.ensureIndex({ type: "persistent", fields: ["feature_id"] });
