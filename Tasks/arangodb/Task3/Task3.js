res = db._query('Let param_date = "2018-07-07" Let A = (for order in Order Filter order.order_date == param_date Collect customer_id = order.customer_id Aggregate order_price = Sum(order.total_price) Sort order_price Desc Limit 1 For customer in Customer Filter customer.customer_id == customer_id Return {person_id: customer.person_id, customer: customer.customer_id}) Let B = (For person in Person For a in A Filter  TO_STRING(a.person_id) == person._key For v, e in 2..2 INBOUND person Follows Return v) Let C = (For b in B for customer in Customer Filter TO_NUMBER(b._key) == customer.person_id For order in Order Filter customer.customer_id == order.customer_id for order_line in order.order_line For product in Product Filter product.product_id == order_line.product_id Return{cid: order.customer_id, pid: order_line.product_id, brand_id: product.brand_id}) Let D = (for c in C for brand in Brand filter c.brand_id == brand.brand_id collect industry = brand.industry with count into customer_count  return{ industry: industry, customer_count: customer_count}) return length(D)'); 

res2 =  res.getExtra();

print (res)
print( res2 ['stats']['executionTime'] );
