//
// Created by mxmdb on 21. 5. 3..
//

/**
 *  [Task1] Sales Performance ([R, D]=>R).
 *  Find the Top_selling brand in last year
 *  and
 *  Compute the ratio of total revenue to revenue by item class for the Top_selling brand.

    A: SELECT order_line.pid AS product_id, SUM(order_line.price) AS order_price
        FROM Order  // Relational
        UNNEST order_line
        WHERE order_date = current_date - 1 year
        GROUP BY order_line.pid // Document

    B: SELECT brand_id, Product.product_id as product_id, title as product_name, order_price
        FROM A, Product // Relational
        WHERE Product.product_id = A.product_id

    C:  SELECT brand_id.id AS top_brand, SUM(B.order_price) AS revenue
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
//            Filter order.order_date <= "2021-06-01"  && order.order_date >= "2020-06-01"
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



}


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
 */

void T5(){

//Let param_product_id = "B007SYGLZO"
//Let param_curdate = "2021-06-01"
//Let param_olddate = "2020-06-01"
//
//Let A = (For order in Order
//        For review in Review
//        For customer in Customer
//        Filter review.product_id == param_product_id
//        Filter order.order_date <= param_curdate
//        Filter order.order_date >= param_olddate
//        Filter review.order_id == order.order_id
//        Filter order.customer_id == customer.customer_id
//        Filter customer.gender == "F"
//Return {person_id : customer.person_id })
//
//
//Let B = (For person in Person
//        For a in A
//        Filter a.person_id == TO_NUMBER(person._key)
//For v, e in OUTBOUND person Follows, Interested_in
//Return {person, v, e})
//Return Length(B)

}
