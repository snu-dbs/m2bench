import pandas as pd

#start_time = time.time()


def document_process():
     print("----- DOCUMENT DATA ARRANGEMENT STARTED -----")
     drug_data = pd.read_json("merged_drug.json")
     drug_df = pd.DataFrame(drug_data)
     drug_df.rename(columns = {'drugbank_id' : 'drug_id'}, inplace = True)
     drug_df.rename(columns = {'name' : 'drug_name'}, inplace = True)
     drug_df.rename(columns = {'structured_adverse_effects' : 'adverse_effect_list'}, inplace = True)
     drug_df.rename(columns = {'structured_drug_interactions' : 'drug_interaction_list'}, inplace = True)
     
     drug_df['drug_id']=drug_df['drug_id'].str.replace("DB",'', regex=True).astype(int)
     drug_df_seed = drug_df[['drug_id', 'drug_name','targets','adverse_effect_list','drug_interaction_list' ]]

     drugIdName = drug_df[['drug_id','drug_name']].to_csv(r'drugs.csv',index=False)  # making drug_id, drug_name file for Prescription table data

     adverse_effect_unwanted_keys = ["route", "regions", "evidence_type", "incidences","dose_form","dose_strength","age_groups","excluded_age_groups","event","admin",
     "timeline","usage","patient_characteristics","excluded_patient_characteristics","associated_with","with_therapy","with_drugs","with_categories"]
     effect_unwanted_keys = ["drugbank_id","meddra_id","icd10_id","synonyms","modification_of","severity"]
     ae_indices = [i for i,x in enumerate(drug_df_seed['adverse_effect_list']) if x]

     for i in ae_indices:
          for unwanted_key in adverse_effect_unwanted_keys: 
               for j in range((len(drug_df_seed['adverse_effect_list'][i]))):
                    drug_df_seed['adverse_effect_list'][i][j]['adverse_effect_name'] = drug_df_seed['adverse_effect_list'][i][j]['effect']['title']
                    if unwanted_key in drug_df_seed['adverse_effect_list'][i][j]:
                         del drug_df_seed['adverse_effect_list'][i][j][unwanted_key]
         
          
     for i in ae_indices:
          for j in range((len(drug_df_seed['adverse_effect_list'][i]))):
               if "effect" in drug_df_seed['adverse_effect_list'][i][j]:
                    del drug_df_seed['adverse_effect_list'][i][j]["effect"]

     #print("----ADVERSE EFFECT DONE-----")

     ddi_unwanted_keys = ["severity", "summary", "action", "subject_dosage", "affected_dosage","subject_category","affected_category","extended_description", "management", "evidence_level", "subject_category", "references", "conditions"]
     ddi_key = ["subject_drug","affected_drug"]
     ddi_indices = [i for i,x in enumerate(drug_df_seed['drug_interaction_list']) if x]

     for i in ddi_indices:
          for unwanted_key in ddi_unwanted_keys: 
               for j in range((len(drug_df_seed['drug_interaction_list'][i]))):
                    if unwanted_key in drug_df_seed['drug_interaction_list'][i][j]: 
                         del drug_df_seed['drug_interaction_list'][i][j][unwanted_key]
                    if "affected_drug" in drug_df_seed['drug_interaction_list'][i][j] and drug_df_seed['drug_name'][i] in drug_df_seed['drug_interaction_list'][i][j]["affected_drug"]["name"]:
                              del drug_df_seed['drug_interaction_list'][i][j]["affected_drug"]
                              drug_df_seed['drug_interaction_list'][i][j]["interaction_drug"] = drug_df_seed['drug_interaction_list'][i][j].pop("subject_drug")
                              drug_df_seed['drug_interaction_list'][i][j]["interaction_drug"]["drug_name"] = drug_df_seed['drug_interaction_list'][i][j]["interaction_drug"].pop("name")
                              drug_df_seed['drug_interaction_list'][i][j]["interaction_drug"]["drugbank_id"] = int(drug_df_seed['drug_interaction_list'][i][j]["interaction_drug"]["drugbank_id"].replace('DB' ,''))
                              drug_df_seed['drug_interaction_list'][i][j]["interaction_drug"]["drug_id"] = drug_df_seed['drug_interaction_list'][i][j]["interaction_drug"].pop("drugbank_id")
                    elif "subject_drug" in drug_df_seed['drug_interaction_list'][i][j] and drug_df_seed['drug_name'][i] in drug_df_seed['drug_interaction_list'][i][j]["subject_drug"]["name"]:
                         del drug_df_seed['drug_interaction_list'][i][j]["subject_drug"]
                         drug_df_seed['drug_interaction_list'][i][j]["interaction_drug"] = drug_df_seed['drug_interaction_list'][i][j].pop("affected_drug")
                         drug_df_seed['drug_interaction_list'][i][j]["interaction_drug"]["drug_name"] = drug_df_seed['drug_interaction_list'][i][j]["interaction_drug"].pop("name")
                         drug_df_seed['drug_interaction_list'][i][j]["interaction_drug"]["drugbank_id"] = int(drug_df_seed['drug_interaction_list'][i][j]["interaction_drug"]["drugbank_id"].replace('DB' ,''))
                         drug_df_seed['drug_interaction_list'][i][j]["interaction_drug"]["drug_id"] = drug_df_seed['drug_interaction_list'][i][j]["interaction_drug"].pop("drugbank_id")

     #print("----DDI DONE-----")

     target_unwanted_keys = ["polypeptides","references","known_action"]
     target_indices = [i for i,x in enumerate(drug_df_seed['targets']) if x] 

     for i in target_indices:
          for j in range ((len(drug_df_seed['targets'][i]))):
               for unwanted_key in target_unwanted_keys: del drug_df_seed['targets'][i][j][unwanted_key]

     #print("----TARGET DONE-----")

     drug_json = drug_df_seed.to_json('../../healthcare/json/drug.json', orient="records" ,lines=True) 

     print("----- drug.json DONE -----")
     print("----- DOCUMENT DATA ARRANGEMENT DONE -----")

     #print("--- DOCUMENT%s seconds ---" % (time.time() - start_time))
