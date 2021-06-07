//
// Created by mxmdb on 21. 5. 3..
//

/**
 *  [Task1] Sales Performance ([R, D]=>R).
 *  Find the Top_selling brand in last year
 *  and
 *  Compute the ratio of total revenue to revenue by item class for the Top_selling brand.
 *
 *      A: SELECT order_line.pid AS product_id, SUM(order_line.price) AS order_price
 *          FROM Order  // Relational
 *          UNNEST order_line
 *          WHERE order_date = current_date - 1 year
 *          GROUP BY order_line.pid // Document
 *
 *      B: SELECT brand_id, Product.product_id as product_id, title as product_name, order_price
 *          FROM A, Product // Relational
 *          WHERE Product.product_id = A.product_id
 *
 *      C:  SELECT brand_id.id AS Top_brand, SUM(B.order_price) AS revenue
 *          GROUP BY B.brand_id
 *          ORDER BY revenue DESC LIMIT 1 // Relational
 *
 *      D: SELECT brand_name, product_name,  (B.order_price/C.revenue)*100 AS percent_of_revenue
 *          FROM  Brand, B, C
 *          WHERE
 *              Brand.brand_id = B.brand_id
 *              and B.brand_id = C.Top_brand
 *          GROUP BY B.pid
 *          ORDER By percent_of_revenue // Relational
 */
void T1(){

//     SELECT order_line.pid AS product_id, SUM(order_line.price) AS order_price
//     FROM Order  // Relational
//     UNNEST order_line
//     WHERE order_date = current_date - 1 year
//     GROUP BY order_line.pid // Document



}


void T2(){}

