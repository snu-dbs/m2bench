db.Customer.ensureIndex({ type: "persistent", fields: [ "customer_id" ] })
db.Order.ensureIndex({type:"persistent", fields: ["order_line[*].product_id"]})
db.Order.ensureIndex({ type: "persistent", fields: [ "customer_id" ] })
db.Order.ensureIndex({ type: "persistent", fields: [ "order_id" ] })
db.Review.ensureIndex({ type: "persistent", fields: [ "order_id" ] })
db.Review.ensureIndex({ type: "persistent", fields: [ "product_id" ] })
db.Product.ensureIndex({ type: "persistent", fields: [ "product_id"] })
