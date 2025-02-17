db._useDatabase("Healthcare");

res1 = db._query(`
    LET A = (
        FOR d IN Drug
        FOR ae IN d.adverse_effect_list
        RETURN DISTINCT {
            drug: d.drug_id,
            adverse_effect: ae.adverse_effect_name,
            is_adverse_effect: 1
        }
    )
    FOR a IN A
    INSERT { drug: a.drug, adverse_effect: a.adverse_effect, is_adverse_effect: a.is_adverse_effect } INTO drug_matrix
`).getExtra();

res2 = db._query(`
    FOR a IN drug_matrix
    FOR b IN drug_matrix
        FILTER a.adverse_effect == b.adverse_effect
        COLLECT row = a.drug, col = b.drug
        AGGREGATE res = SUM(a.is_adverse_effect * b.is_adverse_effect)
    INSERT { drug1: row, drug2: col, val: res } INTO similarity1
`).getExtra();

res3 = db._query(`
    FOR s IN similarity1
        FILTER s.drug1 == s.drug2
    INSERT { drug1: s.drug1, drug2: s.drug2, val: 1 / SQRT(s.val) } INTO inv_norm
`).getExtra();

res4 = db._query(`
    FOR a IN similarity1
    FOR b IN inv_norm
        FILTER a.drug2 == b.drug1
        COLLECT row = a.drug1, col = b.drug2
        AGGREGATE res = SUM(a.val * b.val)
    INSERT { drug1: row, drug2: col, val: res } INTO similarity2
`).getExtra();

res5 = db._query(`
    FOR a IN similarity2
    FOR b IN inv_norm
        FILTER a.drug1 == b.drug1
        COLLECT row = a.drug2, col = b.drug2
        AGGREGATE res = SUM(a.val * b.val)
    INSERT { drug1: row, drug2: col, val: res } INTO drug_similarity
`).getExtra();

res6 = db._query(`
    LET A = (
        FOR p IN Prescription
            FILTER p.patient_id == 90
            RETURN DISTINCT { drug: p.drug_id }
    )
    LET R = (
        FOR a IN A
        FOR ds IN drug_similarity
            FILTER ds.drug1 == a.drug
        RETURN ds
    )
    RETURN LENGTH(R)
`);

res7 = res6.getExtra();

print(res6);
print(
    res1['stats']['executionTime'] +
    res2['stats']['executionTime'] +
    res3['stats']['executionTime'] +
    res4['stats']['executionTime'] +
    res5['stats']['executionTime'] +
    res7['stats']['executionTime']
);

// Answer Validation
// const fs = require("fs");

// let cursor = db._query(`
//     LET A = (
//         FOR p IN Prescription
//             FILTER p.patient_id == 90
//             RETURN DISTINCT { drug: p.drug_id }
//     )

//     FOR a IN A
//     FOR ds IN drug_similarity
//         FILTER ds.drug1 == a.drug
//     RETURN ds
// `);
// let data = cursor.toArray();

// let csvContent = "drug_1,drug_2,val\n";

// data.forEach(row => {
//     csvContent += `${row.drug1},${row.drug2},${row.val}\n`;
// });

// let filePath = "/tmp/t9.csv";
// fs.write(filePath, csvContent);
