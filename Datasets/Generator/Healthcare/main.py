import sys
import mergedrug
import graphprocess
import documentprocess
import tableprocess

#import time

def main ():
    drug_dirpath = "../../raw_datasets/drugbank/"
    Nodes_path = "../../raw_datasets/snomed/description.txt"
    Edges_path = "../../raw_datasets/snomed/relationship.txt"
    mimic_dirpath = "../../raw_datasets/mimic/"

    mergedrug.merge_drug(drug_dirpath)
    graphprocess.nodes_generator(Nodes_path)
    graphprocess.edges_generator(Edges_path)
    documentprocess.document_process()
    tableprocess.table_process(mimic_dirpath)

if __name__ == "__main__":
    main()