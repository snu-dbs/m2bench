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
//select gender, count(gender) from Patient
//where patient_id in (select distinct patient_id from Diagnosis where patient_id != 9 and disease_id in
//(select distinct disease_id from (traverse in('Is_a') from (select from Disease where disease_id in
//(select disease_id from (traverse out('Is_a') from (select from Disease where disease_id in
//                                                              (select disease_id from Diagnosis where patient_id = 9)) MAXDEPTH 1)))
//MAXDEPTH 1)) and disease_id not in (select disease_id from Diagnosis where patient_id = 9)) GROUP BY gender
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
// drop class drug_temp unsafe
// drop class target_temp unsafe
// drop class has_bond unsafe

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
//create class has_bond extends e
//
//################ batch (80sec)
//
//LET $edge = select drug_id, targets.id as target_id from (SELECT drug_id, targets FROM Drug unwind targets) where targets.id is not null;
//LET $i = 0;
//
//while($i<$edge.size())
//{
//	create edge has_bond from (select from drug_temp where drug_id = $edge.drug_id[$i]) to (select from target_temp where target_id = $edge.target_id[$i]);
//	LET $i = $i + 1;
//}
//###############
//
//(10sec)
//select $ORIENT_DEFAULT_ALIAS_0.drug_name as drug1, drug_dst.drug_name as drug2, count(*) as common_target from
//(MATCH {class: drug_temp, where: (drug_id in (select distinct drug_id from Prescription where patient_id = 9))}.out("has_bond").in("has_bond")
//         {as: drug_dst} return $paths) where $ORIENT_DEFAULT_ALIAS_0 != drug_dst group by drug1, drug2 order by common_target desc
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
//create class drug_matrix1
//CREATE PROPERTY drug_matrix1.drug String
//CREATE PROPERTY drug_matrix1.adverse_effect String
//create index drug_matrix1.drug ON drug_matrix1 (drug) NOTUNIQUE
//create index drug_matrix1.adverse_effect ON drug_matrix1 (adverse_effect) NOTUNIQUE

// insert into drug_matrix1 from select distinct drug_id as drug, adverse_effect_list.adverse_effect_name as adverse_effect, 1 as is_adverse_effect
// from (SELECT drug_id, adverse_effect_list FROM Drug unwind adverse_effect_list)
// where adverse_effect_list.adverse_effect_name is not null
//
//create class drug_matrix2
//CREATE PROPERTY drug_matrix2.drug String
//CREATE PROPERTY drug_matrix2.adverse_effect String
//create index drug_matrix2.drug ON drug_matrix2 (drug) NOTUNIQUE
//create index drug_matrix2.adverse_effect ON drug_matrix2 (adverse_effect) NOTUNIQUE

// insert into drug_matrix2 from select distinct drug_id as drug, adverse_effect_list.adverse_effect_name as adverse_effect, 1 as is_adverse_effect
// from (SELECT drug_id, adverse_effect_list FROM Drug unwind adverse_effect_list)
// where adverse_effect_list.adverse_effect_name is not null
//
//CREATE LINK drug TYPE LINK FROM drug_matrix1.drug TO drug_matrix2.drug;
//CREATE LINK adverse_effect TYPE LINK FROM drug_matrix1.adverse_effect TO drug_matrix2.adverse_effect;

//
//create class similarity1
//CREATE PROPERTY similarity1.drug1 String
//CREATE PROPERTY similarity1.drug2 String
//create index similarity1.drug1 ON similarity1 (drug1) NOTUNIQUE
//create index similarity1.drug2 ON similarity1 (drug2) NOTUNIQUE
//
// insert into similarity1 as select a.drug as drug1, b.drug as drug2, sum(a.is_adverse_effect*b.is_adverse_effect)
// from drug_matrix as a, (select from drug_matrix) as b
// where a.adverse_effect in b.adverse_effect group by drug1, drug2

//select a.drug as drug1, $temp.drug as drug2, sum(a.is_adverse_effect*$temp.is_adverse_effect)
//from drug_matrix as a, LET $temp = (select from drug_matrix)
//where a.adverse_effect in $temp.adverse_effect group by drug1, drug2

//########################### Stop implementation (needs link creation between intermediate results)


}
