res = db._query('let param_industry = "Leisure" let param_topN = 10 let param_amount = 10000 Let A = ( for brand in Brand filter brand.industry == param_industry for product in Product filter brand.brand_id == product.brand_id for order in Order filter product.product_id in order.order_line[*].product_id for order_line in order.order_line filter product.product_id == order_line.product_id collect cid = order.customer_id aggregate total_spent = sum(order_line.price) filter total_spent > param_amount for customer in Customer filter cid == customer.customer_id return distinct {cpid: customer.person_id, total_spent: total_spent}) Let B =(For a in A For person in Person Filter person._key == TO_STRING(a.cpid) LET followers = LENGTH(FOR v IN 1..1 INBOUND person Follows RETURN 1) sort followers Desc limit param_topN Return  {person: person._key, followers: followers}) Let C = (for person in Person for b in B filter b.person == person._key for v in 1..1 OUTBOUND person Interested_in return distinct v._id) Return Length(C)');  

res2 =  res.getExtra();

print (res)
print( res2 ['stats']['executionTime'] );
