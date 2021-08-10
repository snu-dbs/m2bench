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

    Select product_name, (order_price/$C.revenue[0]) as percent_of_revenue From
            (
                    SELECT brand_name, product_name, brand_id, order_price, $C FROM $B
    Let $B = (SELECT brand_id, brand_name, product_name, A.order_price as order_price
    From
    (SELECT brand_id, brand.name as brand_name, product_id, title as product_name,
    (SELECT product_id, sum(price) as order_price
    From (SELECT order_line.product_id as product_id, order_line.price as price
    From (SELECT *
          From Order
    Where order_date >= "2020-06-01" and order_date <= "2021-06-01"
    UNWIND order_line))
    Group by product_id) as A
    From Product
    UNWIND A)
    Where A.product_id == product_id),
    $C = (Select brand_id as top_brand, sum(order_price) as revenue From $B Group by brand_id Order by revenue DESC limit 1)
    UNWIND $C
    )
    Where brand_id == $C.top_brand
    Order by percent_of_revenue

//    Elapsed Time: 813s

    Select brands.name as brand_name,  product_name, percent_of_revenue
    From (
      Select brand_id, product_name, percent_of_revenue, $D as brands
    From 
    (
    Select brand_id, product_name, (order_price/$C.revenue[0]) as percent_of_revenue From
            (
                    SELECT brand_id, product_name, brand_id, order_price, $C FROM $B
    Let $B = (SELECT brand_id, product_name, A.order_price as order_price
    From
    (SELECT brand_id,  product_id, title as product_name,
    (SELECT product_id, sum(price) as order_price
    From (SELECT order_line.product_id as product_id, order_line.price as price
    From (SELECT *
          From Order
    Where order_date >= "2020-06-01" and order_date <= "2021-06-01"
    UNWIND order_line))
    Group by product_id) as A
    From Product
    UNWIND A)
    Where A.product_id == product_id),
    $C = (Select brand_id as top_brand, sum(order_price) as revenue From $B Group by brand_id Order by revenue DESC limit 1)
    UNWIND $C
    )
    Where brand_id == $C.top_brand
    Order by percent_of_revenue
	)
    LET $D = (Select brand_id, name from Brand)
    unwind $D
    ) 
    where brands.brand_id = brand_id

}


void T2(){}


/**
 [Task4] Customer Interests ([R, D, G]=>R).
 *  Find the interests of top-N famous customers who made more than a certain amount of orders in a given product category.
 *
 *      A: SELECT DISTINCT Order.cid AS cid , SUM(Order.Order_line.price) as total_spent
 *         FROM Product, Order, Brand UNNEST Order.order_line
 *         WHERE Product.pid=Order.order_line.pid AND Brand.brand_id=Product.brand_id AND Brand.industry = @param
 *         GROUP BY cid
 *         HAVING total_spent > @param// Document
 *
 *      B: SELECT SNS.influencer.person_id AS person_id, COUNT(SNS.n) AS followers FROM A, SNS
 *         WHERE (n:Person)  - [r:FOLLOWS] - > (influencer:Person) AND SNS.influencer.person_id=A.cid ORDER BY followers DESC LIMIT N = @param // Relational
 *
 *      C: SELECT SNS.t.tid FROM B, SNS  WHERE (p:Person)  - [r:HAS_INTEREST] - > (t:Tag) AND SNS.p.person_id=B.person_id // Relational
 *
 *      @param industry
 *      @param N
 *      @param min_spent

*/
void T4(){
    
select count(tag_id)
from (select distinct tag_id as tag_id 
from (select out('Interested_in').tag_id as tag_id
from Person
where person_id in 
(select person_id 
 from (select in('Follows').size() as follower, person_id
from Person 
where person_id in (select person_id
		from Customer 
		where customer_id in (select distinct customer_id
			from (select customer_id, product_id,total_spent 
				from (select customer_id, order_line.product_id as product_id, sum(order_line.price) as total_spent
			from Order
			group by customer_id
			unwind order_line, product_id, price)
		where total_spent>10000
		and product_id in (select product_id 
                  		from Product
                  		where brand_id in (select brand_id 
                                    		from Brand 
                                    		where industry = "Sports")))))
group by person_id
order by follower desc limit 10))
unwind tag_id))    
    
}



/**
 * [Task5]. Filtering social network (R, D, G) => Graph
 *  Extract social network whose nodes are woman customers who has bougth the product 'X' within 1 year and wrote the reviews. 2021-06-01
 *  A: SELECT Customer.person_id AS person_id
 *      FROM Order, Review, Customer
 *      WHERE Review.product_id=‘B007SYGLZO’ AND Review.order_id=Order.order_id
 *          AND Order.order_date = current_date – 1 year
 *          AND Order.customer_id=Customer.customer_id AND Customer.gender=‘female’ // Relational
 *
 *  B: SELECT p, r, node AS subGraph FROM A, SNS
 *      WHERE (p:Person) - [r] -> (node)
 *      AND SNS.p.person_id=A.person_id // Graph
*/
void T5(){
    
select expand(out('Follows')) 
from Person 
where person_id in (select person_id 
from Customer
Where gender = "F"
and customer_id in (Select customer_id 
                   from Order
                   where order_id in (select order_id
                                   from Review 
                                   where product_id = "B007SYGLZO")
                   and  order_date > "2020-06-21"
                   and order_date<"2021-06-21"
                                  ))
        
    
}
