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

/**
 * [Task 7] Group of patients with similar disease (RxG=>R)
 * Find patients suffering from a similar disease with a given patient
 *
 * A: SELECT snomed_id FROM Diagnosis WHERE patient_id = @param //table
 * B: SELECT SNOMED.concept2.snomed_id FROM SNOMED, A
 *      WHERE SNOMED.concept1(snomed_id=A.snomed_id) - [Has_Relationship*2] - (SNOMED.concept2)//table
 * C: SELECT Diagnosis.patient_id FROM Diagnosis, B WHERE Diagnosis.snomed_id = B.snomed_id //table
 *
 * @param patient_id
 */

void T7(){
    //
    //Let A = (For diagnosis in Diagnosis
    //Filter diagnosis.patient_id == 9
    //Return {disease_id: diagnosis.disease_id})
    //
    //Let B = (For a in A
    //For disease in Disease_network_nodes
    //FILTER a.disease_id == TO_NUMBER(disease._key)
    //For v, e in 2..2 Any disease Disease_network_edges
    //RETURN DISTINCT {disease_id: TO_NUMBER(v._key)})
    //
    //For diagnosis in Diagnosis
    //For b in B
    //Filter diagnosis.disease_id == b.disease_id and diagnosis.patient_id != 9
    //RETURN DISTINCT diagnosis.patient_id

}


/**
 * [Task 8] Potential drug interaction (RxD=>R)
 * Find all potential interaction drugs with the given patient's prescription drug
 *
 * A: SELECT drug_id, drug_name FROM Drug
 * B: SELECT d.targets.id, d.targets.name FROM Drug UNNEST targets d
 * C: SELECT d.drug_id, d.target.id FROM Drug UNNEST targets d
 * D: Drug.toGraph(Node : A,B {label: drug, target} edge : C {label: has_bond} )//graph
 * E: SELECT d1.drug_name, d2.drug_name, count(t) AS common_target FROM D, Prescription
 *      WHERE (d1:drug)-[:has_bond]-(t:target)-[:has_bond]-(d2:drug)
 *      AND Prescription.patient_id = @param AND Prescription.drug_id = d1.drug_id
 *      GROUP BY d1.drug_name, d2.drug_name ORDER BY common_target DESC //table
 *
 * @param patient_id
 */

void T8(){
    ////For drug in Drug
    ////INSERT {_key:TO_STRING(drug.drug_id), drug_name:drug.drug_name} INTO drug_temp
    //
    ////Let B = (For drug in Drug
    ////For target in drug.targets
    ////RETURN DISTINCT {target_id: target.id, target_name: target.name})
    ////For b in B
    ////INSERT {_key: TO_STRING(b.target_id), target_name:b.target_name} INTO target_temp
    //
    ////For drug in Drug
    ////For target in drug.targets
    ////For dt in drug_temp
    ////For tt in target_temp
    ////FILTER dt._key == TO_STRING(drug.drug_id) and tt._key == TO_STRING(target.id)
    ////INSERT {_from: dt._id, _to: tt._id} INTO has_bond
    //
    //LET T = (For p in Prescription
    //FILTER p.patient_id == 9
    //RETURN DISTINCT {drug_id: p.drug_id})
    //
    //For drug in drug_temp
    //For target in target_temp
    //For p in T
    //FILTER p.drug_id == TO_NUMBER(drug._key)
    //For v1,e1 in 1..1 Any drug has_bond
    //FILTER target._key == v1._key
    //For v2,e2 in 1..1 Any target has_bond
    //FILTER drug._key != v2._key
    //COLLECT drug1=drug.drug_name, drug2=v2.drug_name
    //aggregate common_target = LENGTH(1)
    //sort common_target desc
    //RETURN {drug1, drug2, common_target}
    //
    ////For d in drug_temp
    ////Remove d in drug_temp
    //
    ////For t in target_temp
    ////Remove t in target_temp
    //
    ////For h in has_bond
    ////Remove h in has_bond
}

/**
 *  [Task9] Drug similarity (R,D=>A)
 *  Find similar drugs for a given patient X's prescribed drug
 *
 *  A: SELECT drug_id as drug, adverse_effect.title as adverse_effect, 1 as is_adverse_effect
 *          FROM Drug
 *          UNNEST adverse_effects_list as adverse_effect  //table
 *
 *  B: A.toArray  -> (<is_adverse_effect>[drug, adverse_effect]) //array
 *  C: Cosine_similarity(B) -> (<similarity coefficient>[drug, adverse_effect]) //array
 *  D: SELECT * FROM C WHERE drug in  (Select drug_id  from Prescription where patient_id = X)  //array
 *
 */

void T9(){
    ////Let A = (for d in Drug
    ////for ae in d.adverse_effects_list
    ////return DISTINCT {drug: d.drug_id, adverse_effect: ae.effect.title, is_adverse_effect: 1})
    ////For a in A
    ////INSERT {drug: a.drug, adverse_effect: a.adverse_effect, is_adverse_effect: a.is_adverse_effect} INTO drug_matrix
    //
    ////For dm in drug_matrix
    ////Remove dm in drug_matrix
    //
    ////for a in drug_matrix
    ////for b in drug_matrix
    ////filter a.adverse_effect == b.adverse_effect
    ////collect row=a.drug, col=b.drug
    ////aggregate res = sum(a.is_adverse_effect*b.is_adverse_effect)
    ////INSERT {drug1: row, drug2: col, val: res} INTO similarity1
    //
    ////for s in similarity1
    ////filter s.drug1 == s.drug2
    ////INSERT {drug1: s.drug1, drug2: s.drug2, val: 1/sqrt(s.val)} INTO inv_norm
    //
    ////for a in similarity1
    ////for b in inv_norm
    ////filter a.drug2 == b.drug1
    ////collect row=a.drug1, col=b.drug2
    ////aggregate res = sum(a.val*b.val)
    ////INSERT {drug1: row, drug2: col, val: res} INTO similarity2
    //
    ////for a in similarity2
    ////for b in inv_norm
    ////filter a.drug1 == b.drug1
    ////collect row=a.drug2, col=b.drug2
    ////aggregate res = sum(a.val*b.val)
    ////INSERT {drug1: row, drug2: col, val: res} INTO drug_similarity
    //
    //Let A=(for p in Prescription
    //filter p.patient_id == 9
    //return DISTINCT {drug: p.drug_id})
    //
    //for a in A
    //for ds in drug_similarity
    //filter ds.drug1 == a.drug
    //return ds
}