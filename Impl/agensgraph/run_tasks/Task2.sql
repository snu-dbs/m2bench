    \timing

Create temporary table V as
    (
    with temp as (

        Select "order".data->>'customer_id' as customer_id, review.data->>'product_id' as product_id, review.data->>'rating' as val
        From "order", review
        Where review.data->>'order_id'="order".data->>'order_id'

        )
    Select customer_id, product_id, avg(val::Float) as val
    From temp
    Group by customer_id, product_id
    );

    Create temporary table feature_size  as SELECT generate_series(1,50) AS feature_id;

    Create temporary table W as
    (
        Select customer_id, feature_id, random()::float as val
        from
            (Select distinct(customer_id) as customer_id from V) as customer,
            feature_size
    );

    Create temporary table H as
    (
        Select product_id, feature_id, random()::float as val
        from
            (Select distinct(product_id) as product_id from V) as product,
            feature_size
    );

    Create Index on V (customer_id);
    Create Index on V (product_id);

    Create Index on W (feature_id);
    Create Index on W (customer_id);
    Create Index on W (customer_id, feature_id);

    Create Index on H (feature_id);
    Create Index on H (product_id);
    Create Index on H (product_id, feature_id);

    Create temporary table WtV as
    (
        Select product_id, feature_id, sum(W.val*V.val) as val from W,V
        where V.customer_id = W.customer_id
        group by product_id, feature_id
    );

    Create Index on WtV (product_id);
    Create Index on WtV (feature_id);
    Create Index on WtV (feature_id, product_id);

    Create temporary table WtW as
    (
        Select W1.feature_id as feature_id1, W2.feature_id as feature_id2, sum(W1.val*W2.val) as val
        from W as W1, W as W2
        where W1.customer_id = W2.customer_id
        group by feature_id1, feature_id2
    );

    Create temporary table WtWH as
    (
        Select product_id, WtW.feature_id1 as feature_id, sum(WtW.val*H.val) as val
        from WtW, H
        where WtW.feature_id2 = H.feature_id
        group by product_id, WtW.feature_id1
    );

    Create Index on WtWH (product_id);
    Create Index on WtWH (feature_id);
    Create Index on WtWH (feature_id, product_id);

    Create temporary table newH as
    (

        Select H.product_id, H.feature_id , (H.val*WtV.val/WtWH.val) as val
            from WtV, WtWH, H
            where WtV.feature_id = WtWH.feature_id
                and WtV.feature_id = H.feature_id
                and WtV.product_id = WtWH.product_id
                and WtV.product_id = H.product_id

    );

    Create Index on newH (feature_id);
    Create Index on newH (product_id);
    Create Index on newH (product_id, feature_id);

    Create temporary table VHt as
    (
        Select customer_id, feature_id, sum(newH.val*V.val) as val
        from newH, V
        where newH.product_id = V.product_id
        group by customer_id, newH.feature_id
    );

    Create Index on VHt (customer_id);
    Create Index on VHt (feature_id);
    Create Index on VHt (feature_id, customer_id);

    Create temporary table HHt as
    (
        Select H1.feature_id as feature_id1, H2.feature_id as feature_id2, sum(H1.val*H2.val) as val
        from newH as H1, newH as H2
        where H1.product_id = H2.product_id
        group by feature_id1, feature_id2
    );

    Create temporary table WHHt as
    (
        Select customer_id,  HHt.feature_id1 as feature_id, sum(HHt.val*W.val) as val
        from HHt, W
        where HHt.feature_id2 =  W.feature_id
        group by customer_id, HHt.feature_id1
    );

    Create Index on WHHt (customer_id);
    Create Index on WHHt (feature_id);
    Create Index on WHHt (feature_id, customer_id);

    Create temporary table newW as
    (

        Select W.customer_id, W.feature_id , (W.val*VHt.val/WHHt.val) as val
            from VHt, WHHt, W
            where VHt.feature_id = WHHt.feature_id
                and VHt.feature_id = W.feature_id
                and VHt.customer_id = WHHt.customer_id
                and VHt.customer_id = W.customer_id

    );


