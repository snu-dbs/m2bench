CREATE LINK customer TYPE LINK FROM Order.customer_id TO Customer.customer_id;
/* CREATE LINK product TYPE LINK FROM Order.order_line.product_id TO Product.product_id; */
CREATE LINK product TYPE LINK FROM Review.product_id TO Product.product_id;
CREATE LINK brand TYPE LINK FROM Product.brand_id TO Brand.brand_id;
CREATE LINK order TYPE LINK FROM Review.order_id TO Order.order_id;
CREATE LINK person TYPE LINK FROM Customer.person_id TO Person.person_id;

