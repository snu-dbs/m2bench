 SET graph_path = social_network;
 \timing

 SELECT COUNT(*) FROM TEMP_w;
 With A as (
    select person_id
    from Customer, (select temp.customer_id as customer_id, sum((temp.price)::Float) as total_spent
           from Product,Brand,(
                Select data->>'customer_id' as customer_id,
                    jsonb_array_elements(data->'order_line')->> 'product_id'as product_id,           
                    jsonb_array_elements(data->'order_line')->> 'price' as price
                from "order"
            ) as temp
            where Brand.industry ='Leisure'
                and Product.brand_id = Brand.brand_id
                and temp.product_id = Product.product_id
                group by customer_id
                having sum((temp.price)::Float)>10000
            ) as atemp
    where atemp.customer_id = Customer.customer_id
),
B as (
    Select p2->'person_id' as person_idrs
    from (MATCH (p2: person)<-[r:follows]-(p1:person)
        where p2.person_id in (Select to_jsonb(person_id) from  A)
        return p2, count(p1) as cnt
        order by cnt desc
        limit 10
    ) as b
)

Select count(c.t) as tag_id
from (
    MATCH (p1: person)-[r:interested_in]->(t: hashtag)
        where p1.person_id in (Select * from B)
        Return t, count(*) as cnt
) as c;