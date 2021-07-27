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

//    select count(*)
//    from
//    (select distinct(drug_name)
//    from
//            (
//                    select drug_interaction.interaction_drug.drug_name as drug_name from
//    (select drug.drug_interaction_list as drug_interaction from Prescription where patient_id =9
//    unwind drug_interaction)
//    )
//    )


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
//select distinct patient_id from Diagnosis
//let $set = (traverse both('Is_a') from (select from Disease where disease_id in (select disease_id from Diagnosis where patient_id = 9)) MAXDEPTH 2 STRATEGY BREADTH_FIRST)
//where disease_id in $set.disease_id and patient_id != 9
//
//select distinct patient_id from Diagnosis
//let $set = (traverse both('Is_a') from (select from Disease
//                                        let $set2=(select disease_id from Diagnosis where patient_id = 9) where disease_id in $set2) MAXDEPTH 2 STRATEGY BREADTH_FIRST)
//where disease_id in $set.disease_id and patient_id != 9
//
//select distinct patient_id from Diagnosis where patient_id != 9 and disease_id in
//(select distinct disease_id from (traverse both('Is_a') from (select from Disease where disease_id in
//                                                              (select disease_id from Diagnosis where patient_id = 9)) MAXDEPTH 2 STRATEGY BREADTH_FIRST))
//
//select distinct patient_id from Diagnosis where patient_id != 9 and disease_id in
//(select distinct disease_id from (traverse both('Is_a') from (select from Disease where disease_id in
//                                                              (select disease_id from Diagnosis where patient_id = 9)) MAXDEPTH 2 STRATEGY BREADTH_FIRST)
//where disease_id not in (select disease_id from Diagnosis where patient_id = 9))

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
//create class drug_temp extends v
//CREATE PROPERTY drug_temp.drug_id INTEGER
//create index drug_id ON drug_temp (drug_id) UNIQUE
//insert into drug_temp from select drug_id, drug_name from Drug
//
//create class target_temp extends v
//CREATE PROPERTY target_temp.target_id String
//create index target_id ON target_temp (target_id) UNIQUE
//insert into target_temp from select distinct targets.id as target_id, targets.name as target_name from (SELECT targets FRom Drug unwind targets) where targets.id is not null
//
//create class drug_e extends v
//CREATE PROPERTY drug_e.drug_id INTEGER
//create index drug_e.drug_id ON drug_e (drug_id) NOTUNIQUE
//insert into drug_e from select drug_id from (SELECT drug_id, targets FROM Drug unwind targets) where targets.id is not null
//CREATE LINK drug TYPE LINK FROM drug_e.drug_id TO drug_temp.drug_id;
//
//create class target_e extends v
//CREATE PROPERTY target_e.target_id String
//create index target_e.target_id ON target_e (target_id) NOTUNIQUE
//insert into target_e from select targets.id as target_id from (SELECT drug_id, targets FROM Drug unwind targets) where targets.id is not null
//CREATE LINK target TYPE LINK FROM target_e.target_id TO target_temp.target_id;
//
//create class has_bond extends e
//create edge has_bond from (select drug from drug_e) to (select target from target_e)
//select $A.drug, $B.target let $A = (select drug from drug_e), $B = (select target from target_e)

//
//SELECT drug_id, drug_name FROM Drug
//select distinct targets.id as target_id, targets.name as target_name from (SELECT targets FRom Drug unwind targets) where targets.id is not null
//select drug_id, targets.id as target_id from (SELECT drug_id, targets FROM Drug unwind targets) where targets.id is not null

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
 *  C: Cosine_similarity(B) -> (<similarity coefficient>[drug1, drug2]) //array
 *  D: SELECT * FROM C WHERE drug1 in (Select drug_id from Prescription where patient_id = X)  //array
 *
 */
void T9(){

}
