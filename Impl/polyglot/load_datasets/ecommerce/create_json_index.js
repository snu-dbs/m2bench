db.Review.createIndex({"order_id":1});
db.Review.createIndex({"product_id":1});
db.Review.createIndex({'rating': 1})
db.Review.createIndex({'rating': 1, 'order_id': 1})

db.Order.createIndex({"order_id":1});
db.Order.createIndex({"order_line.product_id":1});
db.Order.createIndex({"customer_id":1});

