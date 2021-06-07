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
 *          ORDER By percent_of_revenue // Relational
 */
void T1(){

/** 
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
**/

Let A = (For order in Order
            Filter order.order_date <= "2021-06-01"  && order.order_date >= "2020-06-01"
            For order_line in order.order_line
                Collect product_id = order_line.product_id  
                Aggregate order_price = Sum(order_line.price) 
                Return { product_id: product_id, order_price : order_price })

        
Let B = (For a in A
            For product in Product
                Filter a.product_id == product.product_id 
                Return {brand_id :  product.brand_id, product_id: a.product_id, product_name: product.title, order_price: a.order_price} )
            

Let C = (For b in B 
            Collect top_brand = b.brand_id 
            Aggregate revenue = Sum(b.order_price)
            SORT revenue DESC 
            Limit 1
            Return { top_brand, revenue })
            


Let D = (For c in C
           For brand in Brand
               For b in B 
                   Filter b.brand_id == brand.brand_id
                   Filter b.brand_id == c.top_brand
                   Let percent_of_revenue = (b.order_price/c.revenue)*100
                   Sort percent_of_revenue Desc
                   Return {brand_name : brand.name, product_name: b.product_name})
                
Return Length(D)



}


void T2(){}

