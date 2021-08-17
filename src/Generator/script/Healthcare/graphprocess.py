import pandas as pd
import csv
import time

start_time = time.time()
print("----- GRAPH DATA ARRANGEMENT STARTED -----")

pd.options.mode.chained_assignment = None
with open('sct2_Description_Snapshot-en_US1000124_20210301.txt', 'r') as in_file:
    stripped = (line.strip() for line in in_file)
    lines = (line.split("\t") for line in stripped if line)
    with open('sct2_Description_Snapshot-en_US1000124_20210301.csv', 'w') as out_file:
        writer = csv.writer(out_file)
        writer.writerows(lines)
print('----- Nodes TXT to CSV conversion DONE -----')

with open('sct2_Relationship_Snapshot_US1000124_20210301.txt', 'r') as in_file:
    stripped = (line.strip() for line in in_file)
    lines = (line.split("\t") for line in stripped if line)
    with open('sct2_Relationship_Snapshot_US1000124_20210301.csv', 'w') as out_file:
        writer = csv.writer(out_file)
        writer.writerows(lines)
print('----- Edges TXT to CSV conversion DONE -----')
print('----- Processing Nodes STARTED-----')
nodes = pd.read_csv('sct2_Description_Snapshot-en_US1000124_20210301.csv',low_memory=False)
nodes_df = pd.DataFrame(nodes)
nodes_df.rename(columns = {'conceptId' : 'disease_id'}, inplace = True)

nodes_seed_df = nodes_df[['disease_id','term']]
#nodes_seed_df.insert(0,'nodeLabel','Concept')
#nodes_seed_df[['conceptId']] = nodes_seed_df[['conceptId']].astype(np.double)
nodes_seed_df = nodes_seed_df.groupby(['disease_id'], as_index= False).agg({'term': lambda x: ', '.join(x.astype(str))}).reset_index()  #['term'].apply(','.join).reset_index()
nodes_seed_df = nodes_seed_df[['disease_id','term']] 
print(nodes_seed_df)
node = nodes_seed_df.to_csv(r'Disease_network_nodes.csv',index=False)
print('----- Disease_network_nodes.csv DONE ----')
print('----- Processing edges STARTED -----')
edges = pd.read_csv('sct2_Relationship_Snapshot_US1000124_20210301.csv',low_memory=False)
edges_df = pd.DataFrame(edges)
edges_df.rename(columns = {'destinationId' : 'destination_id'}, inplace = True)
edges_df.rename(columns = {'sourceId' : 'source_id'}, inplace = True)

edges_df[['source_id','destination_id']] = edges_df[['source_id','destination_id']].apply(pd.to_numeric)
edges_seed_df = edges_df[['source_id','destination_id']]

#edges_df.rename(columns = {'superTypeId' : 'destinationConceptId'}, inplace = True)
#edges_df.rename(columns = {'subTypeId' : 'sourceConceptId'}, inplace = True)
edges_seed_df.insert(1,'edge_label','is_a')

#edges_df = edges_df[['sourceConceptId','Relationship','destinationConceptId']]
edge = edges_seed_df.to_csv(r'Disease_network_edges.csv',index=False)
print('----- Disease_network_edges.csv DONE -----')
print("----- GRAPH DATA ARRANGEMENT DONE -----")

#print("--- GRAPH %s seconds ---" % (time.time() - start_time))
