# M2Bench Tasks

This file contains a summary of operations and full SQL sentences of the M2bench tasks.

## Summary of Operations

<!-- NOTE: The latex sections (i.e., $ ~~~ $) are not displayed properly in the gitlab but these are properly displayed in github. -->

| Syntax/Function | Description  |
|---|---|
| pattern: (*a*: *A*)-[*r*: *R*]-(*b*: *B*) | Node *a* is linked with node *b* through the edge *r*. *A*, *R* and *B* denote the label or field name. Directed edge can be represented using `-[r:R]->` instead of `-[r:R]-`. |
| MATCH *patterns* WHERE *conditions* RETURN *variables* | Match all *patterns* satisfying the *conditions* in the graph and return a set of *variables* in the matched *patterns*. |
| Unnest | If a document or object contains a nested array, perform a join of the nested array with its parent object and return the joined object. | 
| Dot(.) | Return the value of the corresponding nested field. | 
| toArray( $d_1, d_2, ..., d_n, \space A_1, A_2, ..., A_n$ ) | Convert an object to an array of which size is ( $d_1, d_2, ..., d_n$ ) having $A_1, A_2, ..., A_n$ as attributes. | 
| toGraph(*n*, *e*)  |  Convert an object to a graph of which nodes are *n* and edges are *e*. | 
| MatMul( $m_1, m_2$ ) | Matrix multiplication on two matrices $m_1$ and $m_2$ . | 
| Factorization | Perform the matrix factorization and produce two matrices, $m_1$ and $m_2$, containing the latent factors. | 
| ST_Distance( $g_1, g_2$ )  | Find the euclidean distance between two geometries $g_1$ and $g_2$ . | 
| ST_ClosestObject(*d*, *o*, *c*) | In the dataset *d*, find the object *o* of which coordinate is closest from the coordinate *c*. | 
| ShortestPath(*G*, *s*, *e*)  | Find the shortest path and its cost in the graph *G* with start node *s* and end node *e*. | 
| OVER WINDOW ( $d_1, d_2, ..., d_n$ ) | Calculate window aggregation with a size of $d_1 \times d_2 \times , ..., \times d_n$ . | 
| LogisticRegression(*X*, *y*)  | Build a logistic model from a matrix *X* and a label vector *y*. | 
| Cosine_similarity(*a*)  | Calculate the row-wise cosine similarity of given array *a*, and returns similarity matrix. | 

## Tasks

### E-commerce

#### T0. Building a Logistic Model

Build a logistic regression model to predict if a user prefers the given brand.


```SQL
A = SELECT p.person_id, t.tag_id
	FROM (MATCH (p: Person)-[:Interested_in]->(t: Hashtag) RETURN p, t

B = SELECT Customer.person_id, Product.brand_id, COUNT(Review.review_id) as cnt
	FROM Product, Review, Order, Customer
	WHERE Customer.customer_id = Order.customer_id AND
		Order.order_id = Review.order_id AND
		Review.product_id = Product.product_id AND
		Review.rating = 5
	GROUP BY Customer.person_id, Product.brand_id

C = SELECT B.person_id, MIN(B.brand_id)
	FROM B, (SELECT person_id, MAX(cnt) as max_cnt FROM B GROUP BY person_id) AS T1
	WHERE B.person_id = T1.person_id AND
		B.cnt = T1.max_cnt
	GROUP BY B.person_id

D = A.toArray(dim1: person_id, dim2: tag_id, val: 1)

E = C.toArray(dim1: person_id, val: (brand_id == given_brand_id) ? 1 : 0)

F = LogisticRegression(D, E)
```

#### T1. Sales Performance

Compute the ratio of total revenue to revenue by item class for the brand which produced the highest revenue last year.

```SQL
A = SELECT oid, order_line.pid AS pid, order_line.price AS order_price 
    FROM Order UNNEST order_line 
    WHERE order_date = current_date - 1 year

B = SELECT A.oid AS oid, Product.pid AS pid, Brand.brand_name AS brand_name, A.order_price AS order_price 
    FROM A, Product, Brand 
    WHERE Product.pid=A.pid AND Product. brand_name = Brand.brand_name

C = SELECT B. brand_name AS topseller_brand, SUM(B.order_price) AS revenue 
    FROM B 
    GROUP BY B.brand_name 
    ORDER BY revenue 
    DESC LIMIT 1

D = SELECT B.pid, (SUM(B.order_price)/C.revenue*100) AS percent_of_revenue 
    FROM B, C 
    WHERE C.topseller_brand=B.brand_name 
    GROUP BY B.pid
```


####  T2. Product Recommendation

Perform the product recommendation based on the past customer ratings.

```SQL
A = SELECT customer_id, product_id, rating
    FROM Review, "Order"
    WHERE Review.order_id="Order".order_id

B, C = A.toArray(
   dim1: customer_id, 
   dim2: product_id, 
   val: avg(rating)).Factorization

D = MatMul(B,C)
```

#### T3. Purchase Propensities

Given a certain day, find the customer who spent the highest amount of money in orders, and analyze the purchase propensities of people within 2-hop relationship with the customer. 

```SQL
A = SELECT Customer.person_id as person_id, SUM(Order.total_price) AS order_price 
    FROM Order,Customer 
    WHERE order_date = ‘2018-07-07’ AND Order.customer_id = Customer.customer_id 
    GROUP BY person_id 
    ORDER BY order_price 
    DESC LIMIT 1

B = SELECT Customer.customer_id as 2hop_customer_id 
    FROM A, SNS, Customer 
    WHERE (p1:Person) - [FOLLOWS*2] - > (p2:Person) AND SNS.p2.person_id = A.person_id AND SNS.p1.person_id = Customer.person_id

C = SELECT Order.customer_id AS customer_id, Order.order_line.pid AS pid, Product.brand_name AS brand_name 
    FROM  B, Order, Product UNNEST Order.order_line 
    WHERE Order.customer_id = B.2hop_customer_id AND Product.product_id=Order.order_line.product_id

D = SELECT Brand.industry, COUNT(*) AS customer_count 
    FROM C, Brand 
    WHERE C.brand_name=Brand.brand_name 
    GROUP BY Brand.industry
```


#### T4. Customer Interests

Find the interests of top-N famous customers who made more than a certain amount of orders from a given product category.
- X = 'Leisure' 

```SQL
A = SELECT DISTINCT Customer.person_id as person_id, SUM(Order.Order_line.price) as total_spent 
    FROM Product, Order, Brand UNNEST Order.order_line
    WHERE Product.product_id=Order.order_line.product_id 
          AND Brand.brand_name=Product.brand_name 
          AND Brand.industry=‘x’ 
          AND Order.customer_id = Customer.customer_id 
    GROUP BY person_id 
    HAVING total_spent> 'x'

B = SELECT SNS.influencer.person_id AS person_id, COUNT(SNS.n) AS followers 
    FROM A, SNS
    WHERE (n:Person)  - [FOLLOWS] - > (influencer:Person) AND SNS.influencer.person_id=A.person_id 
    ORDER BY followers 
    DESC LIMIT N

C = SELECT DISTINCT SNS.t.tag_id 
    FROM B, SNS  
    WHERE (p:Person)  - [HAS_INTEREST] - > (t:Tag) AND SNS.p.person_id=B.person_id
```


#### T5. Filtering social network

Extract social network sub-graph of female customers who has bought the given product 'X' within 1 year and wrote the reviews.
- X = 'B007SYGLZO'   if SF = 0 
- X = 'B007SYGLZO0'  if SF > 1 

```SQL
A = SELECT Customer.person_id AS person_id
    FROM Order, Review, Customer
    WHERE Review.product_id=‘X’ 
          AND Review.order_id=Order.order_id
          AND Order.order_date = current_date – 1 year
          AND Order.customer_id=Customer.customer_id 
          AND Customer.gender=‘female’ // Relational

B = SELECT p, r, node AS subGraph 
    FROM A, SNS
    WHERE (p:Person) - [r] -> (node)
         AND SNS.p.person_id=A.person_id // Graph
```



### Healthcare

#### T6. Drug interaction

Find drugs that has known interaction with the prescribed drugs for a given patient ( Relational x Document → Relational )
- X = 9*SF

```sql
A = SELECT drug_id 
    FROM Prescription 
    WHERE patient_id = 'X'

B = SELECT drug_interaction_list.drug_id 
    FROM Drug 
    UNNEST drug_interaction_list
    WHERE drug_id == A
```


#### T7. Group of patients with similar disease

Find patients suffering from a similar disease with a given patient. ( Relational x Graph → Relational)
- X = 9*SF

```SQL
A = SELECT disease_id 
    FROM Diagnosis 
    WHERE patient_id = "X" 
           
B = SELECT distinct(d3.disease_id)
    FROM A,
        (MATCH (d1: Disease)-[:is_a]->(d2: Disease)<-[:is_a]-(d3: Disease) 
         RETURN d1, d2, d3)
    WHERE  A.disease_id = d1 

C = SELECT distinct(patient_id) 
           FROM B, Diagnosis 
           WHERE Diagnosis.disease_id ==  B.disease_id  
                 and Diagnosis.patient_id != "X"
                 and B.disease_id not in A
            
D = SELECT gender, count(gender) 
    From Patient, C
    WHERE Patient.patient_id = C.patient_id  
    GROUP BY gender
```


#### T8. Potential drug interaction

Find all potential interaction drugs with the given patient's prescription drug. ( Relational x Document → Relational)
- X = 9*SF

```SQL
A =  SELECT Patient.drug_name 
     FROM Patient 
     WHERE patient_id = “X” // table

B = SELECT drug_id, drug_name AS drug node 
    FROM Drug

C = SELECT target_id, target_name  AS target node 
    FROM Drug 
    UNNEST targets

D = SELECT drug_target_edge(drug node, target node) AS edge 
    FROM drug

E = Drug.toGraph (Node : B,C {label: drug, target} edge : D {label: has_bond} ) //graph

F = SELECT d2.drug_name, count(t) AS common_target 
    FROM E 
    WHERE (d1: Drug) - [has_bond] - (t: Target) -[has_bond]-  (d2: Drug) 
          AND d1.drug_name = A 
          AND d1.drug_name != d2.drug_name 
    GROUP BY d2 
    ORDER BY common_target DESCENDING // table
```


#### T9. Drug similarity

Find similar drugs for a given patient's prescribed drug. (Relational x Document → Array)
- X = 9*SF


```SQL
A = SELECT UNIQUE drug_id 
    FROM Patient 
    WHERE patient_id="" //table

B = SELECT UNIQUE adverse_effect_list.adverse_effect_name 
    FROM Drug UNNEST adverse_effect_list

C = SELECT drug_id, is_adverse_effect_of_drug(B, Drug.adverse_effect_list ) as X 
    FROM Drug //table

D = B.toArray  -> (<X>[drug_id, adverse_effect_name]) //array
E = Cosine_similarity(B) -> (<similarity coefficient>[drug_id1, drug_id2]) //array
F = SELECT * 
    FROM E 
    WHERE drug_id1 == A //array
```


### Disaster & Safety

#### T10. Road Network Filtering

For the earthquakes which occurred between time Z1 and Z2, find the road network subgraph within 5km from the earthquakes' location.
- Z1 = '2020-06-01 00:00:00'
- Z2 = '2020-06-01 02:00:00'

```SQL
A = SELECT n1, r, n2 AS subgraph 
    FROM Earthquake, Site, RoadNode 
    WHERE (n1:RoadNode) - [r:Road] -> (n2:RoadNode) 
           AND ST_Distance(Site.geometry, Earthquake.coordinates) <= 5km 
           AND Earthquake.time >= Z1 AND Earthquake.time < Z2 
           AND RoadNode.site_id = Site.site_id
```


#### T11. Closest Shelter

For a given earthquake information X, find the cost of the shortest path for each GPS coordinate and Shelter pair. (GPS coordinates are limited by 1 hour and 10km from the X. Shelters are limited by 15km from the X.)
- X = 41862

````SQL
A = SELECT GPS.gps_id, ST_ClosestObject(Site, roadnode, GPS.coordinates) AS roadnode_id 
    FROM  GPS, Site, RoadNode 
    WHERE GPS.time >= X.time AND GPS.time < X.time + 1 hour 
          AND ST_Distance(GPS.coordinates, X.coordinates) <= 10km 
          AND RoadNode.site_id = Site.site_id
          
B = SELECT t.shelter_id, ST_ClosestObject(Site, roadnode, ST_Centroid(t.geometry)) AS roadnode_id 
    FROM RoadNode, Site, ( 
                           SELECT Shelter.shelter_id, Site.geometry 
                           FROM   Site, Shelter 
                           WHERE  ST_Distance(ST_Centroid(Site.geometry), X.coordinates) <= 15km 
                                  AND Shelter.site_id = Site.site_id
                          ) AS t 
    WHERE RoadNode.site_id = Site.site_id
    
C = SELECT A.gps_id, B.shelter_id, ShortestPath(RoadNode, startNode:A.roadnode_id, endNode:B.roadnode_id) AS cost 
    FROM A, B, RoadNode
````


#### T12. New Shelter

For the shelter of which the number of GPS coordinates are the most within 5km from the shelter between time Z1 and Z2, find five closest buildings from the shelter. The buildings are limited by 1km from the shelter.
- Z1 = '2020-09-17 00:00:00'
- Z2 = '2020-09-17 01:00:00'
```SQL
A = SELECT Shelter.shelter_id, Site.geometry 
    FROM Shelter, Site, (
                          SELECT Shelter.shelter_id, COUNT(GPS.gps_id) AS cnt 
                          FROM Shelter, GPS, Site 
                          WHERE ST_Distance(GPS.coordinates, ST_Centroid(Site.geometry)) <= 5km 
                                AND Site.site_id = Shelter.site_id 
                                AND GPS.time >= Z1 AND GPS.time <= Z2 
                          GROUP BY Shelter.id 
                          ORDER BY cnt 
                          DESC LIMIT 1) AS t 
    WHERE Shelter.shelter_id = t.shelter_id 
          AND Site.site_id = Shelter.site_id

B = SELECT A.shelter_id, ST_ClosestObject(Site, roadnode, ST_centroid(A.geometry)) AS roadnode_id 
    FROM A, RoadNode, Site 
    WHERE Site.site_id = RoadNode.site_id

C = SELECT t.site_id, ST_ClosestObject(Site, roadnode, ST_Centroid(t.geometry)) AS roadnode_id 
    FROM Site, RoadNode, (
                           SELECT Site.site_id, Site.geometry 
                           FROM Site, A 
                           WHERE ST_Distance(Site.geometry, ST_centroid(A.geometry)) <= 1km 
                                 AND Site.properties.type = 'building'
                         ) AS t 
    WHERE RoadNode.site_id = Site.site_id

D = SELECT C.site_id, ShortestPath(RoadNode, startNode:B.roadnode_id, endNode:C.roadnode_id) AS cost 
    FROM B, C, RoadNode 
    ORDER BY cost 
    LIMIT 5
```


#### T13. Damage Statistics

For earthquakes of which magnitude is 4.5 or above, find the building statistics. The buildings are limited by 30km from the earthquake location. (Relational, Document) → Document

```SQL
A = SELECT Site.properties.description, COUNT(*) 
    FROM Earthquake, Site 
    WHERE ST_Distance(Site.geometry, Earthquake.coordinates) <= 30km 
          AND Site.properties.type = 'building' 
          AND Earthquake.magnitude >= 4.5 
    GROUP BY Site.properties.description //Document
```



#### T14. Sources of Fine Dust

Given timestamps Z1 and Z2, find the nearest building from a fine dust hotspot for each date between Z1 and Z2. To find the hotspot, use window aggregation with a size of 5. (Document, Array) → Document
- Z1 = 5*SF
- Z2 = 10*SF
```SQL
A = SELECT date, timestamp, latitude, longitude, AVG(pm10) AS pm10_avg 
    FROM FineDust 
    WHERE timestamp >= Z1 AND timestamp <= Z2 
    WINDOW 1, 5, 5 // Array

B = REDIMENSION(A, <pm10_avg: float>[date=0:*, timestamp=0:*, latitude=0:*, longitude=0:*]) // Array

C = SELECT t1.date, t1.timestamp, (t1.latitude, t1.longitude) AS coordinates 
    FROM B AS t1, (
                    SELECT date, MAX(pm10_avg) AS pm10_max 
                    FROM B 
                    GROUP BY date) AS t2 
    WHERE t1.pm10_avg = t2.pm10_max AND t1.date = t2.date // Array

D = SELECT C.date, C.timestamp, ST_ClosestObject(Site, building, C.coordinates) AS site_id 
    FROM C, Site 
    ORDER BY C.date ASC // Document
```


#### T15. Fine Dust Cleaning Vehicles
Given timestamps Z1 and Z2 and current coordinates, find the shortest path from the current coordinates to a hotspot of the finedust between Z1 and Z2. To find the hotspot, use window aggregation with a size of 5. (Graph, Document, Array) → Relational
- Z1 = 5*SF
- Z2 = 10*SF
- X.lat = 34.068509
- X.lon = -118.0614431


```SQL
Q1. A = SELECT (latitude, longitude) AS coo, 
               AVG(pm10) AS pm10_avg 
        OVER WINDOW (*, 5, 5)
        FROM FineDust 
        WHERE timestamp >= Z1 AND timestamp <= Z2
           
Q2. B = SELECT ShortestPath(
                Graph: RoadNetwork, 
                StartNode: ST_ClosestObject(Site, RoadNode, "X"), 
                EndNode: ST_ClosestObject(Site, RoadNode, A.coo)) 
        FROM A, RoadNetwork, Site 
        WHERE Site.site_id = RoadNode.site_id 
        ORDER BY A.pm10_avg DESC LIMIT 1 
```


### T16. Fine Dust Backtesting

For a given timestamp, hindcast the pm10 values of the schools. The Z is the number between min, max of the timestamp dimension. (Document, Array) → Relational
- Z1 = 3*SF
- Z2 = 4*SF

```SQL
A = SELECT latitude, longitude, AVG(pm10) AS pm10_avg 
    FROM FineDust 
    WHERE timestamp >= Z1 AND timestamp <= Z2 
    GROUP BY latitude, longitude // Document

B = Site.site_id, A.pm10_avg 
    FROM Site, A 
    WHERE WithIN(Box(A.latitude, A.longitude, A.latitude+e1, A.longitude+e2), ST_Centroid(Site.geometry)) 
          AND Site.properties.type = 'building' 
          AND Site.properties.description = 'school' // Document
```
