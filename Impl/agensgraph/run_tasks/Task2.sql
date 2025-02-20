\timing

CREATE TEMPORARY TABLE V AS (
    WITH temp AS (
        SELECT "order".data->>'customer_id' AS customer_id, 
               review.data->>'product_id' AS product_id, 
               review.data->>'rating' AS val
        FROM "order", review
        WHERE review.data->>'order_id' = "order".data->>'order_id'
    )
    SELECT customer_id, 
           product_id, 
           AVG(val::FLOAT) AS val
    FROM temp
    GROUP BY customer_id, product_id
);

CREATE TEMPORARY TABLE feature_size AS 
EXPLAIN ANALYZE SELECT generate_series(1, 50) AS feature_id;

CREATE TEMPORARY TABLE W AS (
    -- SELECT customer_id, feature_id, random()::FLOAT AS val
    SELECT customer_id, feature_id, 1.0 AS val
    FROM (SELECT DISTINCT customer_id FROM V) AS customer,
         feature_size
);

CREATE TEMPORARY TABLE H AS (
    -- SELECT product_id, feature_id, random()::FLOAT AS val
    SELECT product_id, feature_id, 1.0 AS val
    FROM (SELECT DISTINCT product_id FROM V) AS product,
         feature_size
);

CREATE INDEX ON V (customer_id);
CREATE INDEX ON V (product_id);

CREATE INDEX ON W (feature_id);
CREATE INDEX ON W (customer_id);
CREATE INDEX ON W (customer_id, feature_id);

CREATE INDEX ON H (feature_id);
CREATE INDEX ON H (product_id);
CREATE INDEX ON H (product_id, feature_id);

CREATE TEMPORARY TABLE WtV AS (
    SELECT product_id, feature_id, SUM(W.val * V.val) AS val 
    FROM W, V
    WHERE V.customer_id = W.customer_id
    GROUP BY product_id, feature_id
);

CREATE INDEX ON WtV (product_id);
CREATE INDEX ON WtV (feature_id);
CREATE INDEX ON WtV (feature_id, product_id);

CREATE TEMPORARY TABLE WtW AS (
    SELECT W1.feature_id AS feature_id1, 
           W2.feature_id AS feature_id2, 
           SUM(W1.val * W2.val) AS val
    FROM W AS W1, W AS W2
    WHERE W1.customer_id = W2.customer_id
    GROUP BY feature_id1, feature_id2
);

CREATE TEMPORARY TABLE WtWH AS (
    SELECT product_id, 
           WtW.feature_id1 AS feature_id, 
           SUM(WtW.val * H.val) AS val
    FROM WtW, H
    WHERE WtW.feature_id2 = H.feature_id
    GROUP BY product_id, WtW.feature_id1
);

CREATE INDEX ON WtWH (product_id);
CREATE INDEX ON WtWH (feature_id);
CREATE INDEX ON WtWH (feature_id, product_id);

CREATE TEMPORARY TABLE newH AS (
    SELECT H.product_id, 
           H.feature_id, 
           (H.val * WtV.val / WtWH.val) AS val
    FROM WtV, WtWH, H
    WHERE WtV.feature_id = WtWH.feature_id
      AND WtV.feature_id = H.feature_id
      AND WtV.product_id = WtWH.product_id
      AND WtV.product_id = H.product_id
);

CREATE INDEX ON newH (feature_id);
CREATE INDEX ON newH (product_id);
CREATE INDEX ON newH (product_id, feature_id);

CREATE TEMPORARY TABLE VHt AS (
    SELECT customer_id, 
           feature_id, 
           SUM(newH.val * V.val) AS val
    FROM newH, V
    WHERE newH.product_id = V.product_id
    GROUP BY customer_id, newH.feature_id
);

CREATE INDEX ON VHt (customer_id);
CREATE INDEX ON VHt (feature_id);
CREATE INDEX ON VHt (feature_id, customer_id);

CREATE TEMPORARY TABLE HHt AS (
    SELECT H1.feature_id AS feature_id1, 
           H2.feature_id AS feature_id2, 
           SUM(H1.val * H2.val) AS val
    FROM newH AS H1, newH AS H2
    WHERE H1.product_id = H2.product_id
    GROUP BY feature_id1, feature_id2
);

CREATE TEMPORARY TABLE WHHt AS (
    SELECT customer_id,  
           HHt.feature_id1 AS feature_id, 
           SUM(HHt.val * W.val) AS val
    FROM HHt, W
    WHERE HHt.feature_id2 = W.feature_id
    GROUP BY customer_id, HHt.feature_id1
);

CREATE INDEX ON WHHt (customer_id);
CREATE INDEX ON WHHt (feature_id);
CREATE INDEX ON WHHt (feature_id, customer_id);

CREATE TEMPORARY TABLE newW AS (
    SELECT W.customer_id, 
           W.feature_id, 
           (W.val * VHt.val / WHHt.val) AS val
    FROM VHt, WHHt, W
    WHERE VHt.feature_id = WHHt.feature_id
      AND VHt.feature_id = W.feature_id
      AND VHt.customer_id = WHHt.customer_id
      AND VHt.customer_id = W.customer_id
);

EXPLAIN ANALYZE SELECT COUNT(*) FROM newW;

-- Answer Validation
-- COPY newW TO '/tmp/t2.csv' DELIMITER ',' CSV HEADER;
