\timing
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
            Where Product.product_id = A.product_id::character(11)
            ),
       C as (
            Select B.brand_id as top_brand, sum(B.order_price) as revenue
            From B
            Group By B.brand_id
            order by revenue DESC limit 1
       )
 Select count(*) from
 (
 	Select  name as brand_name, product_name,  (B.order_price/C.revenue)*100 AS percent_of_revenue  
	 From Brand, B, C
	 Where  Brand.brand_id = B.brand_id
	        and B.brand_id = C.top_brand
	 ORDER By percent_of_revenue desc
 ) as res
