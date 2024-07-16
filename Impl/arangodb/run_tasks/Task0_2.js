/* 1. Task Section */
/* 1-1. Initialize TEMP_A (Step D in task) */
res1 = db._query('FOR person_id IN 0..9948 FOR tag_id IN 0..299 INSERT {_key: CONCAT(TO_STRING(person_id), ",", TO_STRING(tag_id)), person_id: person_id, tag_id: tag_id, val: 0} INTO TEMP_A').getExtra();
/* 1-2. Create TEMP_A (Step A and D in task) */
res2 = db._query('FOR person in Person FOR hashtag IN 1..1 OUTBOUND person Interested_in COLLECT person_id_str = person._key, tag_id_str = hashtag._key UPDATE {_key: CONCAT(person_id_str, ",", tag_id_str)} WITH {val: 1} INTO TEMP_A').getExtra();
/* 1-3. Initialize TEMP_C (Step E in task) */
res3 = db._query('FOR person_id IN 0..9948 INSERT {_key: TO_STRING(person_id), person_id: person_id, val: 0} INTO TEMP_C').getExtra();
/* 1-4. Create TEMP_C (Step B, C, and E in task) */
res4 = db._query('LET B = ( FOR customer in Customer FOR order in Order FILTER customer.customer_id == order.customer_id FOR review in Review FILTER order.order_id == review.order_id AND review.rating == 5 FOR product in Product FILTER review.product_id == product.product_id COLLECT person_id = customer.person_id, brand_id = product.brand_id AGGREGATE cnt = COUNT(review) RETURN {person_id: person_id, brand_id: brand_id, cnt: cnt}) LET C1 = ( FOR row in B COLLECT person_id = row.person_id AGGREGATE max_cnt = MAX(row.cnt) RETURN {person_id: person_id, max_cnt: max_cnt}) FOR out_row in B FOR in_row in C1 FILTER out_row.person_id == in_row.person_id AND out_row.cnt == in_row.max_cnt COLLECT person_id = out_row.person_id AGGREGATE brand_id = MIN(out_row.brand_id) UPDATE {_key: TO_STRING(person_id)} WITH {val: brand_id == 50 ? 1 : 0} INTO TEMP_C').getExtra();
res5 = db._query('FOR i IN 0..299 INSERT {i: i, val: 1} INTO LR_w').getExtra();

/* 3. Logistic Regression */
res6 = db._query('LET Xw = ( FOR x IN TEMP_A FOR y IN LR_w FILTER x.tag_id == y.i COLLECT person_id = x.person_id AGGREGATE val = SUM(x.val * y.val) RETURN {person_id: person_id, val: val}) LET Diff = ( FOR a IN Xw FOR b IN TEMP_C FILTER a.person_id == b.person_id LET diff = (1.0 / (1.0 + EXP(-1.0 * a.val))) - b.val RETURN {person_id: a.person_id, diff: diff}) LET XtDiff = ( FOR x IN TEMP_A FOR y IN Diff FILTER x.person_id == y.person_id COLLECT i = x.tag_id AGGREGATE val = SUM(x.val * y.diff) RETURN {i: i, val: 0.0001 * val}) FOR x IN XtDiff FOR w IN LR_w FILTER x.i == w.i INSERT {i: x.i, val: w.val - x.val} INTO LR_w_new').getExtra();
res7 = db._query('FOR row IN LR_w REMOVE row IN LR_w').getExtra();
res8 = db._query('FOR row IN LR_w_new INSERT row INTO LR_w').getExtra();
res9 = db._query('FOR row IN LR_w_new REMOVE row IN LR_w_new').getExtra();

// print
print(res1['stats']['executionTime'] + res2['stats']['executionTime'] + res3['stats']['executionTime'] + res4['stats']['executionTime'] + res5['stats']['executionTime'] + res6['stats']['executionTime'] + res7['stats']['executionTime'] + res8['stats']['executionTime'] + res9['stats']['executionTime']);
