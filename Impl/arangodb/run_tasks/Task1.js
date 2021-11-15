res = db._query('Let A = (For order in Order Filter order.order_date <= "2021-06-01"  && order.order_date >= "2020-06-01" For order_line in order.order_line Collect product_id = order_line.product_id  Aggregate order_price = Sum(order_line.price) Return { product_id: product_id, order_price : order_price }) Let B = (For a in A For product in Product  Filter a.product_id == product.product_id   Return {brand_id :  product.brand_id, product_id: a.product_id, product_name: product.title, order_price: a.order_price} ) Let C = (For b in B  Collect top_brand = b.brand_id  Aggregate revenue = Sum(b.order_price)  SORT revenue DESC  Limit 1 Return { top_brand, revenue }) Let D = (For c in C         For brand in Brand               For b in B  	                   Filter b.brand_id == brand.brand_id                    Filter b.brand_id == c.top_brand                   Let percent_of_revenue = (b.order_price/c.revenue)*100                    Sort percent_of_revenue Desc                  Return {brand_name : brand.name, product_name: b.product_name}) Return Length(D)');

res2 =  res.getExtra();

print (res)
print( res2 ['stats']['executionTime'] );

