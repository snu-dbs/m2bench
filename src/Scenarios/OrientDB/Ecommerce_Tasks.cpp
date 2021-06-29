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

//    Select product_name, (order_price/$C.revenue[0]) as percent_of_revenue From
//            (
//                    SELECT brand_name, product_name, brand_id, order_price, $C FROM $B
//    Let $B = (SELECT brand_id, brand_name, product_name, A.order_price as order_price
//    From
//    (SELECT brand_id, brand.name as brand_name, product_id, title as product_name,
//    (SELECT product_id, sum(price) as order_price
//    From (SELECT order_line.product_id as product_id, order_line.price as price
//    From (SELECT *
//          From Order
//    Where order_date >= "2020-06-01" and order_date <= "2021-06-01"
//    UNWIND order_line))
//    Group by product_id) as A
//    From Product
//    UNWIND A)
//    Where A.product_id == product_id),
//    $C = (Select brand_id as top_brand, sum(order_price) as revenue From $B Group by brand_id Order by revenue DESC limit 1)
//    UNWIND $C
//    )
//    Where brand_id == $C.top_brand
//    Order by percent_of_revenue

//    Elapsed Time: 813s

}


void T2(){}

