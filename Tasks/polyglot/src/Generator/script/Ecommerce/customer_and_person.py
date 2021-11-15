import csv
import pandas as pd

SF1_customer_rows = 9948

def generator(tpc_ds_dirpath):
    data1 = pd.read_csv(tpc_ds_dirpath+"customer.dat", encoding='ISO-8859-1', sep="|",
                        usecols=[0,1,8,9,11,12,13,14,16],
                        names=['person_id', 'customer_id','firstname','lastname','day','month','year', 'nationality', 'email'])

    data2 = pd.read_csv(tpc_ds_dirpath+"customer_demographics.dat", encoding='ISO-8859-1', sep="|",
                        usecols=[0,1],
                        names=['person_id', 'gender'])

    data3 = pd.read_csv(tpc_ds_dirpath+"customer_address.dat", encoding='ISO-8859-1', sep="|",
                        usecols=[0,6,7,8,9],
                        names=['person_id', 'city', 'county', 'state', 'zipcode'] )

    merged = data1.merge(data2, on='person_id')
    merged = merged.merge(data3, on='person_id')
    merged.dropna(axis=0, how='any', inplace=True)

    cols=['year', 'month', 'day', 'zipcode']
    for c in cols:
        merged[c]=merged[c].astype(int)
    cols.pop()
    merged['date_of_birth'] = merged[cols].apply(lambda x: '-'.join(x.values.astype(str)), axis="columns")
    merged['date_of_birth']=pd.to_datetime(merged['date_of_birth'])
    merged.drop(cols, axis=1, inplace=True)

    merged = merged.reset_index()
    merged['person_id'] = merged.index

    ## generate Customer.csv
    merged.loc[:SF1_customer_rows].to_csv('Customer.csv', header= True, index= False, columns=['customer_id', 'person_id', 'gender', 'date_of_birth', 'zipcode', 'city', 'county', 'state'], sep='|')

    ## generate person_node.csv
    merged.loc[:SF1_customer_rows].to_csv('person_node.csv', header= True, index= False, columns=['person_id', 'gender', 'date_of_birth', 'firstname', 'lastname', 'nationality', 'email'], sep='|')