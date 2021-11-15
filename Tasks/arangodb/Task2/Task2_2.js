res1 = db._query(`
	    Let ratings = (For order in Order
		        For review in Review
		        Filter review.order_id == order.order_id
		        Collect customer_id =  order.customer_id, product_id = review.product_id
		        Aggregate val = average(review.rating)
		        Return {customer_id, product_id, val}
		        )

	    For v in ratings
	    Insert v into V`).getExtra()



res2 = db._query(`Let D1  = (For v in V
	    Collect product_id =  v.product_id
	    Return{product_id}
	    )

	    Let Feature_Size = 50

	    For d in D1
	    For i IN 1..Feature_Size
	    Insert {product_id: d.product_id, feature_id: i , val: rand() } into H`).getExtra();


res3 =  db._query(`Let D1  = (For v in V
	    Collect customer_id =  v.customer_id
	    Return{customer_id}
	    )

	    Let Feature_Size = 50

	    For d in D1
	    For i IN 1..Feature_Size
	    Insert {customer_id: d.customer_id, feature_id: i , val: rand() } into W`).getExtra();

res4 = db._query(`For w in W
	    For v in V
	    Filter v.customer_id == w.customer_id
	    collect product_id = v.product_id, feature_id = w.feature_id
	    aggregate val = sum(v.val*w.val)
	    Insert  { product_id, feature_id , val } into WtV`).getExtra();


res5 = db._query(`Let WtW = (For w1 in W
	    For w2 in W
	    Filter w1.customer_id == w2.customer_id
	    collect feature_id1 = w1.feature_id, feature_id2 = w2.feature_id
	    aggregate val = sum(w1.val*w2.val)
	    Return { feature_id1 , feature_id2 , val }
	    )

	    For wtw in WtW
	    For h in H
	    Filter wtw.feature_id2 == h.feature_id
	    collect feature_id = wtw.feature_id1,  product_id = h.product_id
	    aggregate val = sum(wtw.val*h.val)
	    Insert { product_id, feature_id ,  val } into WtWH`).getExtra();

res6 = db._query(`For h in H
	    For wtwh in WtWH
	    For wtv in WtV
	    Filter wtv.product_id == wtwh.product_id and  wtv.feature_id == wtwh.feature_id
	    Filter h.feature_id == wtwh.feature_id and  h.product_id == wtwh.product_id
	    Let val = h.val*(wtv.val/wtwh.val)
	    Insert { product_id: h.product_id, feature_id: h.feature_id ,  val } into newH`).getExtra();

res7 = db._query(`For h in newH
	    For v in V
	    Filter v.product_id == h.product_id
	    Collect customer_id  = v.customer_id, feature_id = h.feature_id
	    aggregate val = sum(v.val*h.val)
	    Insert { customer_id , feature_id , val } into VHt`).getExtra();

res8 = db._query(`Let HHt =
	    ( For  h1 in newH
		        For h2 in newH
		        Filter h1.product_id == h2.product_id
		        Collect feature_id1 = h1.feature_id, feature_id2 = h2.feature_id
		        Aggregate val = sum(h1.val*h2.val)
		        Return{ feature_id1 , feature_id2, val } )

	    For hht in  HHt
	    For w in W
	    Filter w.feature_id == hht.feature_id1
	    Collect customer_id = w.customer_id, feature_id = hht.feature_id2
	    Aggregate val = sum(w.val*hht.val)
	    Insert { customer_id, feature_id , val } into WHHt`).getExtra();

res9 = db._query(`For w in W
	    For vht in VHt
	    For whht in WHHt
	    Filter w.customer_id == vht.customer_id and w.feature_id == vht.feature_id
	    Filter vht.customer_id == whht.customer_id and vht.feature_id == whht.feature_id
	    Let val = vht.val*w.val/whht.val
	    Insert { customer_id : w.customer_id , feature_id : w.feature_id , val} into newW`).getExtra();

print ( res1['stats']['executionTime'] 
	        + res2['stats']['executionTime']
	        + res3['stats']['executionTime']
	        + res4['stats']['executionTime']
	        + res5['stats']['executionTime']
	        + res6['stats']['executionTime']
	        + res7['stats']['executionTime']
	        + res8['stats']['executionTime']
	        + res9['stats']['executionTime'])

    


