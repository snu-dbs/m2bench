res1 = db._profileQuery(
  `
    LET ratings = (
        FOR order IN Order
        FOR review IN Review
            FILTER review.order_id == order.order_id
            COLLECT customer_id = order.customer_id, product_id = review.product_id
            AGGREGATE val = AVERAGE(review.rating)
            RETURN { customer_id, product_id, val }
    )

    FOR v IN ratings
    INSERT v INTO V
`,
  {},
  { colors: false }
);

res2 = db._profileQuery(
  `
    LET D1 = (
        FOR v IN V
        COLLECT product_id = v.product_id
        RETURN { product_id }
    )

    LET Feature_Size = 50

    FOR d IN D1
    FOR i IN 1..Feature_Size
    INSERT { product_id: d.product_id, feature_id: i, val: 1.0 } INTO H
`,
  {},
  { colors: false }
);

res3 = db._profileQuery(
  `
    LET D1 = (
        FOR v IN V
        COLLECT customer_id = v.customer_id
        RETURN { customer_id }
    )

    LET Feature_Size = 50

    FOR d IN D1
    FOR i IN 1..Feature_Size
    INSERT { customer_id: d.customer_id, feature_id: i, val: 1.0 } INTO W
`,
  {},
  { colors: false }
);

res4 = db._profileQuery(
  `
    FOR w IN W
    FOR v IN V
        FILTER v.customer_id == w.customer_id
        COLLECT product_id = v.product_id, feature_id = w.feature_id
        AGGREGATE val = SUM(v.val * w.val)
    INSERT { product_id, feature_id, val } INTO WtV
`,
  {},
  { colors: false }
);

res5 = db._profileQuery(
  `
    LET WtW = (
        FOR w1 IN W
        FOR w2 IN W
            FILTER w1.customer_id == w2.customer_id
            COLLECT feature_id1 = w1.feature_id, feature_id2 = w2.feature_id
            AGGREGATE val = SUM(w1.val * w2.val)
            RETURN { feature_id1, feature_id2, val }
    )

    FOR wtw IN WtW
    FOR h IN H
        FILTER wtw.feature_id2 == h.feature_id
        COLLECT feature_id = wtw.feature_id1, product_id = h.product_id
        AGGREGATE val = SUM(wtw.val * h.val)
    INSERT { product_id, feature_id, val } INTO WtWH
`,
  {},
  { colors: false }
);

res6 = db._profileQuery(
  `
    FOR h IN H
    FOR wtwh IN WtWH
    FOR wtv IN WtV
        FILTER wtv.product_id == wtwh.product_id AND wtv.feature_id == wtwh.feature_id
        FILTER h.feature_id == wtwh.feature_id AND h.product_id == wtwh.product_id
        LET val = h.val * (wtv.val / wtwh.val)
    INSERT { product_id: h.product_id, feature_id: h.feature_id, val } INTO newH
`,
  {},
  { colors: false }
);

res7 = db._profileQuery(
  `
    FOR h IN newH
    FOR v IN V
        FILTER v.product_id == h.product_id
        COLLECT customer_id = v.customer_id, feature_id = h.feature_id
        AGGREGATE val = SUM(v.val * h.val)
    INSERT { customer_id, feature_id, val } INTO VHt
`,
  {},
  { colors: false }
);

res8 = db._profileQuery(
  `
    LET HHt = (
        FOR h1 IN newH
        FOR h2 IN newH
            FILTER h1.product_id == h2.product_id
            COLLECT feature_id1 = h1.feature_id, feature_id2 = h2.feature_id
            AGGREGATE val = SUM(h1.val * h2.val)
            RETURN { feature_id1, feature_id2, val }
    )

    FOR hht IN HHt
    FOR w IN W
        FILTER w.feature_id == hht.feature_id1
        COLLECT customer_id = w.customer_id, feature_id = hht.feature_id2
        AGGREGATE val = SUM(w.val * hht.val)
    INSERT { customer_id, feature_id, val } INTO WHHt
`,
  {},
  { colors: false }
);

res9 = db._profileQuery(
  `
    FOR w IN W
    FOR vht IN VHt
    FOR whht IN WHHt
        FILTER w.customer_id == vht.customer_id AND w.feature_id == vht.feature_id
        FILTER vht.customer_id == whht.customer_id AND vht.feature_id == whht.feature_id
        LET val = vht.val * w.val / whht.val
    INSERT { customer_id: w.customer_id, feature_id: w.feature_id, val } INTO newW
`,
  {},
  { colors: false }
);

res10 = db._profileQuery(`RETURN COUNT(newW)`, {}, { colors: false });

/* Print result and execution time */
// print(res10.next());
// print(
//   "Elapsed Time: ",
//   res1["stats"]["executionTime"] +
//     res2["stats"]["executionTime"] +
//     res3["stats"]["executionTime"] +
//     res4["stats"]["executionTime"] +
//     res5["stats"]["executionTime"] +
//     res6["stats"]["executionTime"] +
//     res7["stats"]["executionTime"] +
//     res8["stats"]["executionTime"] +
//     res9["stats"]["executionTime"] +
//     res10.getExtra()["stats"]["executionTime"]
// );

// Answer Validation
// const fs = require("fs");

// let cursor = db._profileQuery(
//   "FOR row IN newW RETURN row",
//   {},
//   { colors: false }
// );
// let data = cursor.toArray();

// let csvContent = "customer_id,feature_id,val\n";

// data.forEach((row) => {
//   csvContent += `${row.customer_id},${row.feature_id},${row.val}\n`;
// });

// let filePath = "/tmp/t2.csv";
// fs.write(filePath, csvContent);
