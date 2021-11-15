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

*      B: Select drug_interaction.interaction_drug.name
 *          From Drug, A
 *          Unnest Drug.drug_interaction_list as drug_interaction
 *          Where A.drug_id == Drug.drug_id
 *          Group by drug_interaction.interaction_drug.name
 *
 *
 *      @param patient_id
 */
void T6(){

//    Let param_patient_id = 9
//
//
//    Let A = (For prescription in Prescription
//    Filter prescription.patient_id == param_patient_id
//    Collect drug_id = prescription.drug_id
//    Return {drug_id}
//    )
//
//
//    Let B= (For a in A
//    For drug in Drug
//    Filter a.drug_id == drug.drug_id
//    For elem in drug.drug_interaction_list
//    collect drug_name = elem.interaction_drug.drug_name
//    Return drug_name)
//    Return Length(B)



}

/**
 * [Task 7] Group of patients with similar disease (RxG=>R)
 * Find the number of female and male patients suffering from a similar disease with a given patient.
 *
 * A: SELECT disease_id FROM Diagnosis WHERE patient_id = @param
 * B: SELECT d3.disease_id FROM Disease Network, A
 *    WHERE (d1: Disease) - [:is_a] -> (d2: Disease) <- [:is_a] - (d3: Disease) AND d1.disease_id in A AND d3.disease_id not in A //table
 * C: SELECT patient_id FROM Diagnosis, B
 *    WHERE Diagnosis.disease_id = B.disease_id AND Diagnosis.patient_id != @param
 * D: SELECT gender, count(gender) FROM Patient, C WHERE Patient.patient_id = C.patient_id GROUP BY gender
 *
 * @param patient_id
 */

void T7(){

    //Let A = (For diagnosis in Diagnosis
    //Filter diagnosis.patient_id == 9
    //Return diagnosis.disease_id)
    //
    //Let B = (For a in A
    //For disease in Disease_network_nodes
    //FILTER TO_STRING(a) == disease._key
    //For v1, e1 in 1..1 OUTBOUND disease Disease_network_edges
    //For v2, e2 in 1..1 INBOUND v1 Disease_network_edges
    //RETURN DISTINCT {disease_id: TO_NUMBER(v2._key)})
    //
    //Let C = (For diagnosis in Diagnosis
    //For b in B
    //Filter diagnosis.disease_id == b.disease_id and diagnosis.patient_id != 9 and b.disease_id not in A
    //RETURN distinct {patient_id: diagnosis.patient_id})
    //
    //For p in Patient
    //For c in C
    //Filter c.patient_id == p.patient_id
    //COLLECT gender=p.gender
    //aggregate count = LENGTH(1)
    //RETURN {gender, count}

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

 //db.drug_temp.drop()
 //db.target_temp.drop()
 //db.has_bond.drop()

 //db._create("drug_temp")
 //db._create("target_temp")
 //db._createEdgeCollection("has_bond")

    //For drug in Drug
    //INSERT {_key:TO_STRING(drug.drug_id), drug_name:drug.drug_name} INTO drug_temp
    //
    //Let B = (For drug in Drug
    //For target in drug.targets
    //RETURN DISTINCT {target_id: target.id, target_name: target.name})
    //For b in B
    //INSERT {_key: TO_STRING(b.target_id), target_name:b.target_name} INTO target_temp
    //
    //For drug in Drug
    //For target in drug.targets
    //For dt in drug_temp
    //For tt in target_temp
    //FILTER dt._key == TO_STRING(drug.drug_id) and tt._key == TO_STRING(target.id)
    //INSERT {_from: dt._id, _to: tt._id} INTO has_bond
    //
    //LET T = (For p in Prescription
    //FILTER p.patient_id == 9
    //RETURN DISTINCT {drug_id: p.drug_id})
    //
    //For drug in drug_temp
    //For p in T
    //FILTER TO_STRING(p.drug_id) == drug._key
    //For v1,e1 in 1..1 Any drug has_bond
    //For v2,e2 in 1..1 Any v1 has_bond
    //FILTER drug._key != v2._key
    //COLLECT drug1=drug.drug_name, drug2=v2.drug_name
    //aggregate common_target = LENGTH(1)
    //sort common_target desc
    //RETURN {drug1, drug2, common_target}
}

/**
 *  [Task9] Drug similarity (R,D=>A)
 *  Find similar drugs for a given patient X's prescribed drug
 *
 *  A: SELECT drug_id as drug, adverse_effect_list.adverse_effect_name as adverse_effect, 1 as is_adverse_effect
 *          FROM Drug
 *          UNNEST adverse_effect_list //table
 *
 *  B: A.toArray  -> (<is_adverse_effect>[drug, adverse_effect]) //array
 *  C: Cosine_similarity(B) -> (<similarity coefficient>[drug1, drug2]) //array
 *  D: SELECT * FROM C WHERE drug1 in (Select drug_id from Prescription where patient_id = X)  //array
 *
 */

void T9(){
    /*
     * db.drug_matrix.drop()
     * db.similarity1.drop()
     * db.inv_norm.drop()
     * db.similarity2.drop()
     * db.drug_similarity.drop()
     *
     * db._create("drug_matrix")
     * db._create("similarity1")
     * db._create("inv_norm")
     * db._create("similarity2")
     * db._create("drug_similarity")
     *
     * db.drug_matrix.ensureIndex({type:"persistent", fields:["drug"]})
     * db.drug_matrix.ensureIndex({type:"persistent", fields:["adverse_effect"]})
     * db.similarity1.ensureIndex({type:"persistent", fields:["drug1"]})
     * db.similarity1.ensureIndex({type:"persistent", fields:["drug2"]})
     * db.inv_norm.ensureIndex({type:"persistent", fields:["drug1"]})
     * db.inv_norm.ensureIndex({type:"persistent", fields:["drug2"]})
     * db.similarity2.ensureIndex({type:"persistent", fields:["drug1"]})
     * db.similarity2.ensureIndex({type:"persistent", fields:["drug2"]})
     * db.drug_similarity.ensureIndex({type:"persistent", fields:["drug1"]})
     * db.drug_similarity.ensureIndex({type:"persistent", fields:["drug2"]})
    */

    ////Let A = (for d in Drug
    ////for ae in d.adverse_effect_list
    ////return DISTINCT {drug: d.drug_id, adverse_effect: ae.adverse_effect_name, is_adverse_effect: 1})
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
