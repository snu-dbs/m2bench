import sys
import table_gen
import doc_gen
import graph_gen
import array_gen
import os
import time

from multiprocessing import Process

def main(argv):
    outdir = '../disaster/'
    # enforce to remove the directory
    try:
        os.system("rm -rf " + outdir)
    except:
        pass

    # make directories for dataset
    try:
        os.mkdir(outdir)
        for model in ('table', 'property_graph', 'json', 'array'):
            os.mkdir(outdir + model)
    except:
        pass

    earthquake_dirpath = '../raw_datasets/earthquake/'
    shelter_dirpath = '../raw_datasets/shelter/'
    gps_dirpath = '../raw_datasets/gps/'
    roadnetwork_dirpath = '../raw_datasets/roadnetwork/'
    site_dirpath = '../raw_datasets/osm/'
    finedust_dirpath = '../raw_datasets/finedust/'

    print("------ Disaster Data Generation ------")
    print("Start OSM Conversion in background")
    po = Process(target=doc_gen.ogr_background, args=(site_dirpath, ))
    po.start()

    print("Start Road Network Generation in background")
    pr = Process(target=graph_gen.roadnetwork_gen, args=(roadnetwork_dirpath, outdir + 'property_graph/'))
    pr.start()

    print("Start Earthquake Generation")
    table_gen.earthquake_gen(earthquake_dirpath, outdir + 'table/')

    print("Start Gps Generation")
    table_gen.gps_gen(gps_dirpath, outdir + 'table/')

    print("Start Finedust Generation")
    array_gen.finedust_gen(finedust_dirpath, outdir + 'array/')
    array_gen.finedust_idx_gen(finedust_dirpath, outdir + 'array/')

    print("Waiting Road Network Generation and OSM Conversion")
    pr.join()
    po.join()
    
    print("Start Site Generation")
    doc_gen.site_gen(outdir + 'json/')

    print("Start Shelter Generation")
    table_gen.shelter_gen(shelter_dirpath, outdir + 'table/')

if __name__ == '__main__':
    main(sys.argv)

