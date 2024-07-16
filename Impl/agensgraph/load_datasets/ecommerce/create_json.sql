\c ecommerce;
  
DROP TABLE IF EXISTS "order";
DROP TABLE IF EXISTS review;

CREATE TABLE IF NOT EXISTS "order" (
        data jsonb
        );

CREATE TABLE IF NOT EXISTS review (
        data jsonb
        );
