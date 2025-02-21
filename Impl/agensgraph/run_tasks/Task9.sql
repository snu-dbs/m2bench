\timing
\pset pager off

EXPLAIN ANALYZE CREATE TEMPORARY TABLE drug_matrix AS (
    SELECT 
        data->'drug_id' AS drug, 
        jsonb_array_elements(data->'adverse_effect_list')->'adverse_effect_name' AS adverse_effect, 
        1 AS is_adverse_effect 
    FROM Drug
    GROUP BY drug, adverse_effect
);

CREATE INDEX ON drug_matrix (drug);
CREATE INDEX ON drug_matrix (adverse_effect);

EXPLAIN ANALYZE CREATE TEMPORARY TABLE similarity1 AS (
    SELECT 
        a.drug AS drug1, 
        b.drug AS drug2, 
        SUM(a.is_adverse_effect * b.is_adverse_effect) AS val
    FROM drug_matrix a, drug_matrix b
    WHERE a.adverse_effect = b.adverse_effect
    GROUP BY drug1, drug2
);

CREATE INDEX ON similarity1 (drug1);
CREATE INDEX ON similarity1 (drug2);

EXPLAIN ANALYZE CREATE TEMPORARY TABLE inv_norm AS (
    SELECT 
        drug1, 
        drug2, 
        1 / SQRT(val) AS val
    FROM similarity1
    WHERE drug1 = drug2
);

CREATE INDEX ON inv_norm (drug1);
CREATE INDEX ON inv_norm (drug2);

EXPLAIN ANALYZE CREATE TEMPORARY TABLE similarity2 AS (
    SELECT 
        similarity1.drug1 AS d1, 
        inv_norm.drug2 AS d2, 
        SUM(similarity1.val * inv_norm.val) AS val
    FROM similarity1, inv_norm
    WHERE similarity1.drug2 = inv_norm.drug1
    GROUP BY d1, d2
);

CREATE INDEX ON similarity2 (d1);
CREATE INDEX ON similarity2 (d2);

EXPLAIN ANALYZE CREATE TEMPORARY TABLE drug_similarity AS (
    SELECT 
        similarity2.d2 AS drug_1, 
        inv_norm.drug2 AS drug_2, 
        SUM(similarity2.val * inv_norm.val) AS val
    FROM similarity2, inv_norm
    WHERE similarity2.d1 = inv_norm.drug1
    GROUP BY drug_1, drug_2
);

CREATE INDEX ON drug_similarity (drug_1);
CREATE INDEX ON drug_similarity (drug_2);

EXPLAIN ANALYZE SELECT COUNT(*) FROM (
    WITH A AS (
        SELECT DISTINCT drug_id AS drug 
        FROM Prescription 
        WHERE patient_id = :X
    )
    SELECT drug_1, drug_2, val  
    FROM drug_similarity, A 
    WHERE drug_similarity.drug_1 = TO_JSONB(A.drug)
) AS res;

-- Answer Validation
-- COPY (
--     WITH A AS (
--         SELECT DISTINCT drug_id AS drug 
--         FROM Prescription 
--         WHERE patient_id = :X
--     )
--     SELECT drug_1, drug_2, val  
--     FROM drug_similarity, A 
--     WHERE drug_similarity.drug_1 = TO_JSONB(A.drug)
--     ORDER BY drug_1, drug_2
-- ) TO '/tmp/t9.csv' DELIMITER ',' CSV HEADER;
