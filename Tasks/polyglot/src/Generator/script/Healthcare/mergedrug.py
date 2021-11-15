import json
import glob


def merge_drug(drug_dirpath):
    #start_time = time.time()
    print("\n----- MERGING json data into merged_drug.json -----")
    result = []
    for f in glob.glob(drug_dirpath+'/*.json'):
        with open(f,"rb") as infile:
            result.append(json.load(infile))

    with open("merged_drug.json","w") as outfile:
        json.dump(result,outfile)

    print("----- merged_drug.json DONE -----")

    #print("----- MERGE DRUG %s seconds -----" % (time.time() - start_time))
