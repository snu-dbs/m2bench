CREATE CLASS Customer;
CREATE PROPERTY Customer.customer_id STRING;
CREATE PROPERTY Customer.person_id INTEGER;

CREATE CLASS Product;
CREATE PROPERTY Product.product_id STRING;
CREATE PROPERTY Product.brand_id INTEGER;

CREATE CLASS Brand;
CREATE PROPERTY Brand.brand_id INTEGER;

CREATE CLASS Order;
CREATE PROPERTY Order.order_id STRING;
CREATE PROPERTY Order.customer_id STRING;
CREATE PROPERTY Order.order_line EMBEDDEDLIST;

CREATE CLASS Review;
CREATE PROPERTY Review.review_id STRING;
CREATE PROPERTY Review.order_id STRING;
CREATE PROPERTY Review.product_id STRING;

CREATE CLASS Person EXTENDS V;
CREATE PROPERTY Person.person_id INTEGER;

CREATE CLASS Hashtag EXTENDS V;
CREATE PROPERTY Hashtag.tag_id INTEGER;

