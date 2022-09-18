res = db._query('Let param_product_id="B007SYGLZO" Let param_curdate="2021-06-01" Let param_olddate="2020-06-01" Let A = ( For review in Review Filter review.product_id == param_product_id For order in Order Filter review.order_id == order.order_id Filter order.order_date <= param_curdate Filter order.order_date >= param_olddate For customer in Customer Filter customer.gender == "F" Filter order.customer_id == customer.customer_id Return {person_id : customer.person_id }) Let B =(For person in Person For a in A Filter TO_STRING(a.person_id)==person._key For v, e in OUTBOUND person Follows Return{person, v, e}) Return LENGTH(B)'); 

res2 =  res.getExtra();

print (res)
print( res2 ['stats']['executionTime'] );
