\c ecommerce;

DROP TABLE IF EXISTS "order";
DROP TABLE IF EXISTS review;

CREATE TABLE IF NOT EXISTS "order" (
    data JSONB
);

CREATE TABLE IF NOT EXISTS review (
    data JSONB
);
