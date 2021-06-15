//
// Created by mxmdb on 21. 5. 3..
//


/**
 *
 * [Task6] Drug Interaction (R,D=>R)
 *  Find drugs that has known interaction with the prescribed drugs for a give patient
 *
 *      A: Select distinct(drug_id) as drug_id
 *          From Prescription
 * *        Where patient_id = @param

 *      B: Select interaction_drug.name
 *          From Drug, A
 *          Unnest Drug.drug_interaction_list as interaction_drug
 *          Where A.drug_id == Drug.drug_id
 *          Group by interaction_drug.name
 *
 *
 *      @param patient_id
 */
void T6(){

//    Let param_patient_id = 9
//
//    Let A = (For prescription in Prescription
//        Filter prescription.Patient_id == param_patient_id
//        Collect drug_id = prescription.drug_id
//        Return {drug_id}
//    )
//
//
//    Let B= (For a in A
//                For drug in Drug
//                    Filter a.drug_id == drug.drug_id
//                    For elem in drug.drug_interactions_list
//                        collect drug_name = elem.interaction_drug.name
//                        Return drug_name)
//    Return Length(B)



}


void T7(){


}

void T8(){


}

/**
 *
 *  [Task9] Drug similarity (R,D=>A)
 *  Find similar drugs for a given patient X's prescribed drug
 *
 *  A: SELECT drug_id as drug, adverse_effect.title as adverse_effect, 1 as is_adverse_effect
 *          FROM Drug
 *          UNNEST adverse_effects_list as adverse_effect  //table
 *
 *  B: A.toArray  -> (<is_adverse_effect>[drug, adverse_effect]) //array
 *  C: Cosine_similarity(B) -> (<similarity coefficient>[drug, adverse_effect]) //array
 *  D: SELECT * FROM E WHERE drug in  (Select drug_id  from Prescription where patient_id = X)  //array
 *
 */

void T9(){



}