 SET graph_path = social_network;

With A as (
	
		select person_id
		from Customer, (select temp.customer_id as customer_id, sum((temp.price)::Float) as total_spent
			from (
								Select data->>'customer_id' as customer_id,
								jsonb_array_elements(data->'order_line')->> 'product_id'as product_id, 
								jsonb_array_elements(data->'order_line')->> 'price' as price,
								product.product_id as product_idX,  
								product.brand_id
								from product, "order", Brand 
								where data->'order_line' @> concat('[{ "product_id":"',Product.product_id,'"}]')::jsonb
								and Brand.industry = 'Leisure' and Brand.brand_id = product.brand_id 
							) as temp
				where
				temp.product_id = temp.product_idX
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
	) as c 

