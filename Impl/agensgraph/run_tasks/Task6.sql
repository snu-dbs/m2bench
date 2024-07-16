\timing

With A as (
          Select distinct(drug_id) as drug_id
          From Prescription
          Where patient_id = 9 
        )

        Select count(*)
        from
         (
            Select distinct(jsonb_array_elements(data->'drug_interaction_list')->'interaction_drug'->>'drug_name')
            From Drug, A
            Where A.drug_id = (data->>'drug_id')::INT
          ) as B
