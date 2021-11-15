import sys
import table_gen
import doc_gen
import graph_gen
import array_gen
import os

def main(argv):
    outdir = './sf1_dataset_output/'
    if os.path.exists(outdir):
        os.system("rm -rf "+outdir)
    os.mkdir(outdir)

    earthquake_dirpath = argv[1]
    shelter_dirpath = argv[2]
    gps_dirpath = argv[3]
    roadnetwork_dirpath = argv[4]
    site_dirpath = argv[5]
    finedust_dirpath = argv[6]

    print("------ Disaster Data Generation ------")

    print("Start Earthquake Generation")
    table_gen.earthquake_gen(earthquake_dirpath, outdir)

    print("Start Gps Generation")
    table_gen.gps_gen(gps_dirpath, outdir)

    print("Start Road Network Generation")
    graph_gen.roadnetwork_gen(roadnetwork_dirpath, outdir)

    print("Start Site Generation")
    doc_gen.site_gen(site_dirpath, outdir)

    print("Start Shelter Generation")
    table_gen.shelter_gen(shelter_dirpath, outdir)

    print("Start Finedust Generation")
    array_gen.finedust_gen(finedust_dirpath, outdir)
    array_gen.finedust_idx_gen(finedust_dirpath, outdir)

if __name__ == '__main__':
    main(sys.argv)