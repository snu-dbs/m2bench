//
// Created by mxmdb on 21. 5. 3..
//

/**
 *  [Task1] Sales Performance ([R, D]=>R).
 *  Find the Top_selling brand in last year
 *  and
 *  Compute the ratio of total revenue to revenue by item class for the Top_selling brand.

    A: SELECT order_line.product_id , SUM(order_line.price) AS order_price
        FROM Order  // Relational
        UNNEST order_line
        WHERE order_date = current_date - 1 year
        GROUP BY order_line.product_id // Document

    B: SELECT brand_id, Product.product_id as product_id, title as product_name, order_price
        FROM A, Product // Relational
        WHERE Product.product_id = A.product_id

    C:  SELECT B.brand_id AS top_brand, SUM(B.order_price) AS revenue
        From B
        GROUP BY B.brand_id
        ORDER BY revenue DESC LIMIT 1 // Relational

    D: SELECT brand_name, product_name,  (B.order_price/C.revenue)*100 AS percent_of_revenue
        FROM  Brand, B, C
        WHERE
            Brand.brand_id = B.brand_id
            and B.brand_id = C.top_brand
        ORDER By percent_of_revenue // Relational
 *
 */


void T1(){

//Let A = (For order in Order
//            Filter order.order_date <= "2021-06-01"  and order.order_date >= "2020-06-01"
//            For order_line in order.order_line
//                Collect product_id = order_line.product_id  
//                Aggregate order_price = Sum(order_line.price) 
//                Return { product_id: product_id, order_price : order_price })
//
//        
//Let B = (For a in A
//           For product in Product
//                Filter a.product_id == product.product_id 
//                Return {brand_id :  product.brand_id, product_id: a.product_id, product_name: product.title, order_price: a.order_price} )
//            
//
//Let C = (For b in B 
//            Collect top_brand = b.brand_id 
//            Aggregate revenue = Sum(b.order_price)
//            SORT revenue DESC 
//            Limit 1
//            Return { top_brand, revenue })
//            
//
//
//Let D = (For c in C
//           For brand in Brand
//               For b in B 
//                   Filter b.brand_id == brand.brand_id
//                   Filter b.brand_id == c.top_brand
//                   Let percent_of_revenue = (b.order_price/c.revenue)*100
//                   Sort percent_of_revenue Desc
//                   Return {brand_name : brand.name, product_name: b.product_name})


}



/**
 *  [Task2] Product Recommendation ([D, A]=>A).
 *  Perform product recommendation using "Factorization" based on the past customer ratings.
 *
 *      A: SELECT Order.customer_id AS customer_id, Review.product_id AS product_id, Review.rating AS rating
 *          FROM Review, Order // Document
 *          WHERE Review.order_id=Order.order_id // Relational
 *
 *      B, C: A.toArray(<val(=rating)>[customer_id,product_id]).Factorization // 2 Arrays
 *
 *      D: B(<val: latent_factor>[customer_id,k]) * C(<val: latent_factor>[k,product_id]) // Array•E: SELECT pid FROM D WHERE cid=‘x’ AND val > 4 // Relational
 *
 */


void T2(){

//    For order in Order
//    For review in Review
//    Filter review.order_id == order.order_id
//    Return {customer_id: order.customer_id, product_id: review.product_id, rating: review.rating}

/*

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




 */


/* 1 */
//    Let ratings = (For order in Order
//    For review in Review
//    Filter review.order_id == order.order_id
//    Collect customer_id =  order.customer_id, product_id = review.product_id
//    Aggregate val = average(review.rating)
//    Return {customer_id, product_id, val}
//    )
//
//    For v in ratings
//    Insert v into V


/* 2 */
//    Let D1  = (For v in V
//    Collect product_id =  v.product_id
//    Return{product_id}
//    )
//
//    Let Feature_Size = 50
//
//    For d in D1
//    For i IN 1..Feature_Size
//    Insert {product_id: d.product_id, feature_id: i , val: rand() } into H


/* 3 */
//    Let D1  = (For v in V
//    Collect customer_id =  v.customer_id
//    Return{customer_id}
//    )
//
//    Let Feature_Size = 50
//
//    For d in D1
//    For i IN 1..Feature_Size
//    Insert {customer_id: d.customer_id, feature_id: i , val: rand() } into W



/* 4 */
//    For w in W
//    For v in V
//    Filter v.customer_id == w.customer_id
//    collect product_id = v.product_id, feature_id = w.feature_id
//    aggregate val = sum(v.val*w.val)
//    Insert  { product_id, feature_id , val } into WtV


/* 5 */
//    Let WtW = (For w1 in W
//    For w2 in W
//    Filter w1.customer_id == w2.customer_id
//    collect feature_id1 = w1.feature_id, feature_id2 = w2.feature_id
//    aggregate val = sum(w1.val*w2.val)
//    Return { feature_id1 , feature_id2 , val }
//    )
//
//
//    For wtw in WtW
//    For h in H
//    Filter wtw.feature_id2 == h.feature_id
//    collect feature_id = wtw.feature_id1,  product_id = h.product_id
//    aggregate val = sum(wtw.val*h.val)
//    Insert { product_id, feature_id ,  val } into WtWH


/* 6 */
//    For h in H
//    For wtwh in WtWH
//    For wtv in WtV
//    Filter wtv.product_id == wtwh.product_id and  wtv.feature_id == wtwh.feature_id
//    Filter h.feature_id == wtwh.feature_id and  h.product_id == wtwh.product_id
//    Let val = h.val*(wtv.val/wtwh.val)
//    Insert { product_id: h.product_id, feature_id: h.feature_id ,  val } into newH

/* 7 */
//    For h in newH
//    For v in V
//    Filter v.product_id == h.product_id
//    Collect customer_id  = v.customer_id, feature_id = h.feature_id
//    aggregate val = sum(v.val*h.val)
//    Insert { customer_id , feature_id , val } into VHt

/* 8 */
//    Let HHt =
//    ( For  h1 in newH
//    For h2 in newH
//    Filter h1.product_id == h2.product_id
//    Collect feature_id1 = h1.feature_id, feature_id2 = h2.feature_id
//    Aggregate val = sum(h1.val*h2.val)
//    Return{ feature_id1 , feature_id2, val } )
//
//    For hht in  HHt
//    For w in W
//    Filter w.feature_id == hht.feature_id1
//    Collect customer_id = w.customer_id, feature_id = hht.feature_id2
//    Aggregate val = sum(w.val*hht.val)
//    Insert { customer_id, feature_id , val } into WHHt

/* 9 */
//    For w in W
//    For vht in VHt
//    For whht in WHHt
//    Filter w.customer_id == vht.customer_id and w.feature_id == vht.feature_id
//    Filter vht.customer_id == whht.customer_id and vht.feature_id == whht.feature_id
//    Let val = vht.val*w.val/whht.val
//    Insert { customer_id : w.customer_id , feature_id : w.feature_id , val} into newW



}
/**
 *  [Task3] Product Purchase Propensities ([R, D, G]=>R).
 *  Given a certain special day, find the customer who spent the highest amount of money in orders, 
 *  and analyze the purchase propensities of people within 3-hop relationships.
 *
 *      A: SELECT cid, SUM(total_price) AS order_price FROM Order
 *         WHERE  order_date = 2021/12/25
 *         GROUP BY cid ORDER BY order_price DESC LIMIT 1 // Document
 *
 *      B: SELECT SNS.p2.person_id AS 2hop_cid FROM A, SNS
 *         WHERE (p1:Person) - [r:FOLLOWS*2] - > (p2:Person) AND SNS.p2.person_id=A.cid // Relational
 * 
 *      C: SELECT Order.cid AS cid, Order.order_line.pid AS pid, Product.brand_id AS brand_id FROM  B, Order, Product
 *         UNNEST Order.order_line WHERE Order.cid=B.2hop_cid AND Product.pid=Order.order_line.pid // Document
 * 
 *      D: SELECT Brand.industry, COUNT(*) AS customer_count FROM C, Brand WHERE C.brand_id=Brand.brand_id GROUP BY Brand.industry // Relational
 *
 
 
void T3(){
Let param_date = "2018-07-07" 

Let A = (for order in Order 
	        Filter order.order_date == param_date 
	            Collect customer_id = order.customer_id 
	            Aggregate order_price = Sum(order.total_price) 
	            Sort order_price Desc Limit 1 
	            For customer in Customer 
	                Filter customer.customer_id == customer_id 
	                    Return {person_id: customer.person_id, customer: customer.customer_id}) 
Let B = (For person in Person 
	        For a in A Filter  
	        TO_STRING(a.person_id) == person._key 
	            For v, e in 2..2 INBOUND person Follows 
	            Return v) 
Let C = (For b in B 
	           for customer in Customer 
	                Filter TO_NUMBER(b._key) == customer.person_id 
	                    For order in Order 
	                        Filter customer.customer_id == order.customer_id 
	                            for order_line in order.order_line 
	                                For product in Product 
	                                    Filter product.product_id == order_line.product_id 
	                                        Return{cid: order.customer_id, pid: order_line.product_id, brand_id: product.brand_id}) 
Let D = (for c in C 
        	for brand in Brand 
	            filter c.brand_id == brand.brand_id 
	                collect industry = brand.industry with count into customer_count  
	                    return{ industry: industry, customer_count: customer_count}) 
return length(D)
) */

/**
 *  [Task4] Customer Interests ([R, D, G]=>R).
 *  Find the interests of top-N famous customers who made more than a certain amount of orders in a given product category.
 *
 *      A: SELECT DISTINCT Order.cid AS cid , SUM(Order.Order_line.price) as total_spent
 *         FROM Product, Order, Brand UNNEST Order.order_line
 *         WHERE Product.pid=Order.order_line.pid AND Brand.brand_id=Product.brand_id AND Brand.industry = @param 
 *         GROUP BY cid
 *          HAVING total_spent > @param// Document
 * 
 *      B: SELECT SNS.influencer.person_id AS person_id, COUNT(SNS.n) AS followers FROM A, SNS
 *         WHERE (n:Person)  - [r:FOLLOWS] - > (influencer:Person) AND SNS.influencer.person_id=A.cid ORDER BY followers DESC LIMIT N = @param // Relational
 * 
 *      C: SELECT SNS.t.tid FROM B, SNS  WHERE (p:Person)  - [r:HAS_INTEREST] - > (t:Tag) AND SNS.p.person_id=B.person_id // Relational
 *
 *      @param industry
 *      @param N
 *      @param min_spent
 

void T4(){
    

let param_industry = "Leisure"
let param_topN = 10
let param_amount = 10000

Let A = (for brand in Brand
            filter brand.industry == param_industry
            for product in Product
                filter brand.brand_id == product.brand_id
                for order in Order
                    filter product.product_id in order.order_line[*].product_id 
                        for order_line in order.order_line
                            filter product.product_id == order_line.product_id
                                collect cid = order.customer_id 
                                aggregate total_spent = sum(order_line.price)
                                filter total_spent > param_amount
                                    for customer in Customer 
                                        filter cid == customer.customer_id
                                            return distinct {cpid: customer.person_id, total_spent: total_spent})

Let B =(For a in A
            For person in Person
            Filter person._key == TO_STRING(a.cpid)
                LET followers = LENGTH(FOR v IN 1..1 INBOUND person Follows RETURN 1)
                sort followers Desc limit param_topN
                Return  {person: person._key, followers: followers})

Let C = (for person in Person
         for b in B
            filter b.person == person._key
            for v in 1..1 OUTBOUND person Interested_in
            return distinct v._id)
            
return (length(C))
    
} */

/**
 * [Task5]. Filtering social network (R, D, G) => Graph
 *  Extract social network whose nodes are woman customers who has bougth the product 'X' within 1 year and wrote the reviews.
 *  A: SELECT Customer.person_id AS person_id
 *      FROM Order, Review, Customer
 *      WHERE Review.product_id=‘X’ AND Review.order_id=Order.order_id
 *          AND Order.order_date = current_date – 1 year
 *          AND Order.customer_id=Customer.customer_id AND Customer.gender=‘female’ // Relational
 *
 *  B: SELECT p, r, node AS subGraph FROM A, SNS
 *      WHERE (p:Person) - [r] -> (node)
 *      AND SNS.p.person_id=A.person_id // Graph
 

void T5(){

Let param_product_id = "B007SYGLZO"
Let param_curdate = "2021-06-01"
Let param_olddate = "2020-06-01"

Let A = (For order in Order 
            For review in Review 
                For customer in Customer 
                    Filter review.product_id == param_product_id 
                    Filter order.order_date <= param_curdate 
                    Filter order.order_date >= param_olddate
                    Filter review.order_id == order.order_id
                    Filter order.customer_id == customer.customer_id 
                    Filter customer.gender == "F" 
                    Return {person_id : customer.person_id })


Let B = (For person in Person
            For a in A
            Filter TO_STRING(a.person_id) == person._key
                For v, e in OUTBOUND person Follows 
                Return {person, v, e})
                
Return LENGTH(B)

}
*/