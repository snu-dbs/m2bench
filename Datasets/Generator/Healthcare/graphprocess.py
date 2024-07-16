import pandas as pd
import csv

def nodes_generator(Nodes_path):

    print("----- GRAPH DATA ARRANGEMENT STARTED -----")
    pd.options.mode.chained_assignment = None
    with open(Nodes_path, 'r') as in_file:
        stripped = (line.strip() for line in in_file)
        lines = (line.split("\t") for line in stripped if line)
        with open('Nodes.csv', 'w') as out_file:
            writer = csv.writer(out_file)
            writer.writerows(lines)
    print('----- Nodes TXT to CSV conversion DONE -----')
    
    print('----- Processing Nodes STARTED-----')

    nodes = pd.read_csv('Nodes.csv',low_memory=False)
    nodes_df = pd.DataFrame(nodes)
    nodes_df.rename(columns = {'conceptId' : 'disease_id'}, inplace = True)

    nodes_seed_df = nodes_df[['disease_id','term']]
    nodes_seed_df = nodes_seed_df.groupby(['disease_id'], as_index= False).agg({'term': lambda x: ', '.join(x.astype(str))}).reset_index()  #['term'].apply(','.join).reset_index()
    nodes_seed_df = nodes_seed_df[['disease_id','term']] 
    node = nodes_seed_df.to_csv(r'../../healthcare/property_graph/Disease_network_nodes.csv',index=False)
    print('----- Disease_network_nodes.csv DONE ----')

def edges_generator(Edges_path):
    with open(Edges_path, 'r') as in_file:
        stripped = (line.strip() for line in in_file)
        lines = (line.split("\t") for line in stripped if line)
        with open('Edges.csv', 'w') as out_file:
            writer = csv.writer(out_file)
            writer.writerows(lines)
    print('----- Edges TXT to CSV conversion DONE -----')

    print('----- Processing edges STARTED -----')

    edges = pd.read_csv('Edges.csv',low_memory=False)
    edges_df = pd.DataFrame(edges)
    edges_df.rename(columns = {'destinationId' : 'destination_id'}, inplace = True)
    edges_df.rename(columns = {'sourceId' : 'source_id'}, inplace = True)

    edges_df[['source_id','destination_id']] = edges_df[['source_id','destination_id']].apply(pd.to_numeric)
    edges_seed_df = edges_df[['source_id','destination_id']]

    edges_seed_df.insert(1,'edge_label','is_a')

    edge = edges_seed_df.to_csv(r'../../healthcare/property_graph/Disease_network_edges.csv',index=False)
    print('----- Disease_network_edges.csv DONE -----')
    print("----- GRAPH DATA ARRANGEMENT DONE -----")



