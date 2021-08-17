import sys
import mergedrug
import graphprocess
import documentprocess
import tableprocess

#import time

def main (argv):
    drug_dirpath = argv[0]
    Nodes_path = argv[1]
    Edges_path = argv[2]
    mimic_dirpath =argv[3]
    mergedrug.merge_drug(drug_dirpath)
    graphprocess.nodes_generator(Nodes_path)
    graphprocess.edges_generator(Edges_path)
    documentprocess.document_process()
    tableprocess.table_process(mimic_dirpath)


if __name__ == "__main__":

    main(sys.argv[1:])