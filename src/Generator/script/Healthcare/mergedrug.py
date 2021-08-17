import json
import glob
from os import PRIO_PGRP
import pandas as pd
import time

start_time = time.time()
print("---MERGING DRUG.JSON ---")
result = []
for f in glob.glob('drugbank_json/*.json'):
    with open(f,"rb") as infile:
        result.append(json.load(infile))

with open("merged_drug.json","w") as outfile:
   json.dump(result,outfile)

print("---MERGING DRUG.JSON DONE ---")

print("---MERGE DRUG %s seconds ---" % (time.time() - start_time))
