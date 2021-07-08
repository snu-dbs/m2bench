//
// Created by mxmdb on 21. 6. 28..
//

#ifndef M2BENCH_AO_ECOMMERCE_TASKS_H
#define M2BENCH_AO_ECOMMERCE_TASKS_H

#endif //M2BENCH_AO_ECOMMERCE_TASKS_H


/**
 *  [Task1] Sales Performance ([R, D]=>R).
 *  Find the Top_selling brand in last year
 *  and Compute the ratio of total revenue to revenue by item class for the Top_selling brand.
 *
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


 With A as (
            Select order_line->>'product_id' as product_id, sum((order_line->>'price')::Float) as order_price
            From (
                    Select jsonb_array_elements(data->'order_line') as order_line
                    from "order"
                    where data->>'order_date' <= '2021-06-01'  and data->>'order_date' >= '2020-06-01' ) as Atemp
            Group by product_id
            ),
       B as (
            Select brand_id, Product.product_id, title as product_name, order_price
            From A, Product
            Where Product.product_id = A.product_id
            ),
       C as (
            Select B.brand_id as top_brand, sum(B.order_price) as revenue
            From B
            Group By B.brand_id
            order by revenue DESC limit 1
       )
 Select name as brand_name, product_name,  (B.order_price/C.revenue)*100 AS percent_of_revenue
 From Brand, B, C
 Where  Brand.brand_id = B.brand_id
        and B.brand_id = C.top_brand
 ORDER By percent_of_revenue desc
*/



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


    Create temporary table V as
    (
    with temp as (

        Select "order".data->>'customer_id' as customer_id, review.data->>'product_id' as product_id, review.data->>'rating' as val
        From "order", review
        Where review.data->>'order_id'="order".data->>'order_id'

        )
    Select customer_id, product_id, avg(val::Float) as val
    From temp
    Group by customer_id, product_id
    );

    Create temporary table feature_size  as SELECT generate_series(1,50) AS feature_id;

    Create temporary table W as
    (
        Select customer_id, feature_id, random()::float as val
        from
            (Select distinct(customer_id) as customer_id from V) as customer,
            feature_size
    );

    Create temporary table H as
    (
        Select product_id, feature_id, random()::float as val
        from
            (Select distinct(product_id) as product_id from V) as product,
            feature_size
    );

    Create Index on V (customer_id);
    Create Index on V (product_id);

    Create Index on W (feature_id);
    Create Index on W (customer_id);
    Create Index on W (customer_id, feature_id);

    Create Index on H (feature_id);
    Create Index on H (product_id);
    Create Index on H (product_id, feature_id);

    Create temporary table WtV as
    (
        Select product_id, feature_id, sum(W.val*V.val) as val from W,V
        where V.customer_id = W.customer_id
        group by product_id, feature_id
    );

    Create Index on WtV (product_id);
    Create Index on WtV (feature_id);
    Create Index on WtV (feature_id, product_id);

    Create temporary table WtW as
    (
        Select W1.feature_id as feature_id1, W2.feature_id as feature_id2, sum(W1.val*W2.val) as val
        from W as W1, W as W2
        where W1.customer_id = W2.customer_id
        group by feature_id1, feature_id2
    );

    Create temporary table WtWH as
    (
        Select product_id, WtW.feature_id1 as feature_id, sum(WtW.val*H.val) as val
        from WtW, H
        where WtW.feature_id2 = H.feature_id
        group by product_id, WtW.feature_id1
    );

    Create Index on WtWH (product_id);
    Create Index on WtWH (feature_id);
    Create Index on WtWH (feature_id, product_id);

    Create temporary table newH as
    (

        Select H.product_id, H.feature_id , (H.val*WtV.val/WtWH.val) as val
            from WtV, WtWH, H
            where WtV.feature_id = WtWH.feature_id
                and WtV.feature_id = H.feature_id
                and WtV.product_id = WtWH.product_id
                and WtV.product_id = H.product_id

    );

    Create Index on newH (feature_id);
    Create Index on newH (product_id);
    Create Index on newH (product_id, feature_id);

    Create temporary table VHt as
    (
        Select customer_id, feature_id, sum(newH.val*V.val) as val
        from newH, V
        where newH.product_id = V.product_id
        group by customer_id, newH.feature_id
    );

    Create Index on VHt (customer_id);
    Create Index on VHt (feature_id);
    Create Index on VHt (feature_id, customer_id);

    Create temporary table HHt as
    (
        Select H1.feature_id as feature_id1, H2.feature_id as feature_id2, sum(H1.val*H2.val) as val
        from newH as H1, newH as H2
        where H1.product_id = H2.product_id
        group by feature_id1, feature_id2
    );

    Create temporary table WHHt as
    (
        Select customer_id,  HHt.feature_id1 as feature_id, sum(HHt.val*W.val) as val
        from HHt, W
        where HHt.feature_id2 =  W.feature_id
        group by customer_id, HHt.feature_id1
    );

    Create Index on WHHt (customer_id);
    Create Index on WHHt (feature_id);
    Create Index on WHHt (feature_id, customer_id);

    Create temporary table newW as
    (

        Select W.customer_id, W.feature_id , (W.val*VHt.val/WHHt.val) as val
            from VHt, WHHt, W
            where VHt.feature_id = WHHt.feature_id
                and VHt.feature_id = W.feature_id
                and VHt.customer_id = WHHt.customer_id
                and VHt.customer_id = W.customer_id

    );




/* 7 */

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

**/



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
 

  ecommerce
 SET graph_path = social_network;      
 WITH A as (select person_id
        	from Customer
        	where customer_id = (select customer_id from ( select data->>'customer_id' as customer_id, sum((data->>'total_price')::Float) as order_price
					from "order"
					where data->>'order_date' = '2018-07-07'
					group by customer_id
        				order by order_price DESC LIMIT 1) as cid)
	),
	
	B as (select customer_id
        	from Customer,A, (MATCH (p1: Person) - [r:follows*2]->(p2:Person) return p1,p2) as btemp
		    where (p2->>'person_id')::numeric = A.person_id and Customer.person_id = (p1->>'person_id')::numeric
	),
	
	C as (  select temp.customer_id as cid,
                    Product.product_id as pid,
                    Product.brand_id as brand_id
        	from B, Product,
            	(
                    Select  data->>'customer_id' as customer_id, jsonb_array_elements(data->'order_line')->>'product_id' as product_id
                    from "order"
            	) as temp
        	where temp.customer_id = B.customer_id
            	and Product.product_id = temp.product_id
	)
	
      	select Brand.industry, count(*) as customer_count 
      	from C, Brand
      	Where C.brand_id = Brand.brand_id
      	group by Brand.industry;
      	
   */   	
      
      
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
 
  ecommerce
 SET graph_path = social_network;
 With A as (select Customer.person_id
 		From "order", Review, Customer
 		where Review.data->>'product_id' = 'B007SYGLZO' and Review.data->>'order_id' =  "order".data->> 'order_id' 
 		and "order".data->>'order_date' <= '2021-06-01' and "order".data->>'order_date' >= '2020-06-01'
 		and "order".data->> 'customer_id' = Customer.customer_id and Customer.gender = 'F')
 
 	select p1->>'person_id' as p1,r,p2->>'person_id' as p2 from A, (MATCH (p1: person)-[r:follows]->(p2:person) return p1,r,p2) as b
 	where (p1->>'person_id')::numeric =  A.person_id;
 */
   
      	

      		



