//
// Created by mxmdb on 21. 6. 29..
//

#ifndef M2BENCH_AO_HEALTHCARE_TASKS_H
#define M2BENCH_AO_HEALTHCARE_TASKS_H

#endif //M2BENCH_AO_HEALTHCARE_TASKS_H

/**
 *
 * [Task6] Drug Interaction (R,D=>R)
 *  Find drugs that has known interaction with the prescribed drugs for a give patient
 *
 *      A: Select distinct(drug_id) as drug_id
 *          From Prescription
 *          Where patient_id = @param
 *
 *      B: Select interaction_drug.drug_name
 *          From Drug, A
 *          Unnest Drug.drug_interaction_list as interaction_drug
 *          Where A.drug_id == Drug.drug_id
 *          Group by interaction_drug.name
 *
 *      @param patient_id
 *

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
 *
 */

