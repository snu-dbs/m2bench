\timing
 SET graph_path = social_network;
 With A as (select Customer.person_id
 		From "order", Review, Customer
 		where Review.data->>'product_id' = 'B007SYGLZO' and Review.data->>'order_id' =  "order".data->> 'order_id' 
 		and "order".data->>'order_date' <= '2021-06-01' and "order".data->>'order_date' >= '2020-06-01'
 		and "order".data->> 'customer_id' = Customer.customer_id and Customer.gender = 'F')
 
 select count(*) from
( 	
 	select p1->>'person_id' as p1,r,p2->>'person_id' as p2	
	from (MATCH (p1: person)-[r:follows]->(p2:person) 
	where p1.person_id in (Select to_jsonb(person_id) from A )
	return p1,r,p2
	) as b
) as res;

