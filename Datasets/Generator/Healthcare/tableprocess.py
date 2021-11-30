import pandas as pd 
import time
import csv
import names


#start_time = time.time()



def rename(df,source,update):
    df.rename(columns = {source : update}, inplace = True)

def table_process(mimic_dirpath):
    print("----- TABLE DATA ARRANGEMENT STARTED -----")
    patients_df = pd.read_csv(mimic_dirpath+"/PATIENTS.csv", low_memory=False)
#patients_df.rename(columns = {'SUBJECT_ID' : 'Patient_id'}, inplace = True)
    rename(patients_df,'SUBJECT_ID', 'patient_id')
    rename(patients_df,'GENDER', 'gender')
    rename(patients_df,'DOB', 'date_of_birth')
    rename(patients_df,'DOD', 'date_of_death')
    patients_df['patient_id'] = patients_df['patient_id'].astype(int)

    patient_name = []
    for i in range(len(patients_df)):
        patient_name.append(names.get_full_name())

    patients_df['patient_name'] = patient_name
    seed_patients_df = patients_df[['patient_id','patient_name', 'gender', 'date_of_birth','date_of_death' ]]

    seed_table =seed_patients_df.to_csv (r'../../healthcare/table/Patient.csv',index=False)

    print ('----- Patient.csv DONE -----')
    
    drug_df = pd.read_csv("drugs.csv", low_memory=False)

    prescription_df = pd.read_csv(mimic_dirpath+"/PRESCRIPTIONS.csv", low_memory=False)
    rename(prescription_df,'SUBJECT_ID', 'patient_id')
    rename(prescription_df,'DRUG', 'drug_name')
    rename(prescription_df,'STARTDATE', 'start_date')
    rename(prescription_df,'ENDDATE', 'end_date')
    seed_prescription_df = prescription_df[['patient_id', 'drug_name', 'start_date','end_date' ]]

    seed_table_df = pd.merge(seed_prescription_df, drug_df, on = "drug_name")
    seed_table_df = seed_table_df.sort_values(by = ['patient_id'])
    seed_table_df['patient_id'] = seed_table_df['patient_id'].astype(int)
    seed_table_df = seed_table_df[['patient_id','drug_id','start_date', 'end_date']]
    seed_table =seed_table_df.to_csv(r'../../healthcare/table/Prescription.csv',index=False)

    print ('----- Prescription.csv DONE -----')


    with open('ICD9CM_SNOMED_MAP_1TO1_202012.txt', 'r') as in_file:
        stripped = (line.strip() for line in in_file)
        lines = (line.split("\t") for line in stripped if line)
        with open('ICD9CM_SNOMED_MAP_1TO1_202012.csv', 'w') as out_file:
            writer = csv.writer(out_file)
            writer.writerows(lines)

    snomed_map_df = pd.read_csv("ICD9CM_SNOMED_MAP_1TO1_202012.csv", low_memory=False)
    rename(snomed_map_df,'SNOMED_CID','disease_id')
    rename(snomed_map_df,'ICD_NAME','DIAGNOSES_DESCRIPTION')

    snomed_map_df['ICD_CODE']=snomed_map_df['ICD_CODE'].str.replace(".",'', regex=True)
    snomed_map_df['disease_id'] = snomed_map_df['disease_id'].astype(int)
    snomed_map_df.loc[snomed_map_df['disease_id']=='0', 'disease_id'] = snomed_map_df['SNOMED_FSN']
    snomed_map = snomed_map_df[['ICD_CODE','disease_id']]
    #snom = snomed_map.to_csv(r'ICD_SNOMED_MAP.csv')

    diagnoses_df = pd.read_csv(mimic_dirpath+"/DIAGNOSES_ICD.csv", low_memory=False)
    diagnoses_df.rename(columns = {'SUBJECT_ID' : 'patient_id'}, inplace = True)
    diagnoses_df.rename(columns = {'ICD9_CODE' : 'ICD_CODE'}, inplace = True)
    diagnoses_df['patient_id'] = diagnoses_df['patient_id'].astype(int)

    diagnoses_seed_df = diagnoses_df[['patient_id','ICD_CODE']]
    seed_table_df = pd.merge(diagnoses_seed_df, snomed_map, on = "ICD_CODE")
    seed_table_df = seed_table_df.sort_values(by = ['patient_id'])
    seed_table =seed_table_df[['patient_id','disease_id']].to_csv (r'../../healthcare/table/Diagnosis.csv',index=False)

    print ('----- Diagnosis.csv DONE -----')


    print("----- TABLE DATA ARRANGEMENT DONE -----")

#print("--- %s seconds ---" % (time.time() - start_time))
#print("---TABLE %s seconds ---" % (time.time() - start_time))
