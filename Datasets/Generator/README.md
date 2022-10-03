# How to generate datasets

## E-commerce
To generate datasets for E-commerce, you should do the following:

1. Install the requirement softwares (the `Requirements` section).
2. Prepare the raw datasets (the `Preparation for raw datasets` section).
3. Run the data generator (the `Usage` section).


### Requirements (python packages)
- Python 3.8
- Python packages
    - randomtimestamp
    - pathlib
    - pandas

### Preparation for raw datasets
- M2bench Ecommerce datasets are generated based on TPC-DS and Unibench datasets.

#### 1. TPC-DS dataset (SF=1): [http://tpc.org/tpc_documents_current_versions/download_programs/tools-download-request5.asp?bm_type=TPC-DS](http://tpc.org/tpc_documents_current_versions/download_programs/tools-download-request5.asp?bm_type=TPC-DS&bm_vers=3.2.0)
- The TPC Tools are available free of charge, however all users must agree to the licensing terms and register prior to use.
      Please download and read the TPC-Tools License Agreement prior to registering for the download.
- After downloading, unzip the TPC-DS tool, navigate to `tools/` directory, and make file to generate binary files required to generate TPC-DS data.
```bash
$ make -f Makefile.suite
```
- Create a new directory in `Datasets/raw_datasets` (e.g. `tpcds_data/`) to store the TPC-DS data of scale factor 1, and run the following command in the `tools/` directory:
```bash
$ ./dsdgen -scale 1 -dir path/to/Datasets/raw_datasets/tpcds_data/
```

#### 2. Unibench dataset (SF=1): [http://udbms.cs.helsinki.fi/?projects/ubench](http://udbms.cs.helsinki.fi/?projects/ubench)
- Download the dataset by clicking "Download ready-made multi-model dataset" through the link above, unzip the file, and locate the whole folder in `Datasets/raw_datasets` directory.
- We provide Unibench dataset in `Datasets/raw_datasets` directory.


### Usage
- Navigate to `Datasets/Generator/Ecommerce` directory.
- Run "python3 main.py [arg1] [arg2]"
     (arg1: directory path to TPC-DS dataset/, arg2: directory path to Unibench dataset/)

```bash
$ python3 main.py ../../raw_datasets/tpcds_data/ ../../raw_datasets/Unibench_SF1/
```
    
### Notes
- Since the TPC-DS Tool is steadily updated, the version of the tool used to generate datasets in M2Bench (v.3.0.1) can be different from the latest version.
- Since the Unibench dataset has been updated, the datasets generated in M2Bench are slightly different from the datasets of the paper.
      Therefore, we provide the Unibench dataset as `Unibench_SF1` directory in `raw_datasets/` directory before the update since we have cited the source url above.
- We also provide synthetic hashtag node data as a part of property graph dataset. It is located in `Datasets/ecommerce/property_graph/` directory.


## Healthcare 

### Requirements  
    python pandas package: "pip3 install pandas"
    python names package: "pip3 install names"
    python glob package: "pip3 install glob3"

### Preparation for raw datasets

#### 1. MIMIC III Clinical Dataset (...)

#### 2. SNOMED Terminology : https://www.nlm.nih.gov/healthit/snomedct/us_edition.html (Download by clicking on the download now button)
- You have to set up an account for UMLS Terminology Services (UTS) (https://uts.nlm.nih.gov/uts/) to access the data.

#### 3. Drugbank (drug json data) : https://portal.drugbank.com/ (Download latest DrugBank data in JSON format)
- You have to set up an account for Drugbank (https://go.drugbank.com/public_users/sign_up) and get approved to access the data.


#### Usage: 
- Navigate to `Datasets/Generator/Healthcare` directory.
- 
```bash
$ python3 main.py [arg1][arg2][arg3][arg4]
```
- arg1: directory path to drugrugbank json dataset
- arg2: file path to SNOMED Concept description file from Snapshot library (e.g: SnomedCT_xxx/Snapshot/Terminology/ Description_Snapshot_xxx.txt) 
- arg3: file path to SNOMED Concept relationship file from Snapshot library (e.g: SnomedCT_xxx/Snapshot/Terminology/ Relationship_Snapshot_xxx.txt) 
- arg4: directory path to MIMIC III dataset)


#### Caution:  Do not forget to extract the downloaded files before usage. 
- Make sure the dataset directory names does not contain characters such as : "()". Rename the folders if needed. 
- Keep in mind that arg1, arg4 are directory paths and arg2, arg3 are file paths.*
- We provide 'ICD9CM_SNOMED_MAP_1TO1_202012.txt' file for table data generation. Make sure the file is located in the same directory where you execute main.py.




## Disaster & Safety 

To generate datasets for Disaster & Safety, you should do the following:

1. Install the requirement softwares (the `Requirements` section).
2. Prepare the raw datasets (the `Preparation for raw datasets` section).
3. Run the data generator (the `Usage` section).

### Requirements

- Python 3.8
- Python packages (you can use `requirements.txt`)
    - pandas (1.3.4 tested)
    - numpy (1.21.4 tested)
    - geopy (2.2.0 tested)
    - Shapely (1.8.0 tested)
    - scikit-learn (1.0.1 tested)
    - jsons (1.6.0 tested)
- GDAL (2.4.2 tested)

### Preparation for raw datasets

#### 1. Earthquake: [United States Geological Survey (USGS)](https://earthquake.usgs.gov/earthquakes/search)
        
1. Download the below files.
    - Links: [query-jan](https://earthquake.usgs.gov/fdsnws/event/1/query.csv?starttime=2020-01-01%2000:00:00&endtime=2020-01-31%2023:59:59&maxlatitude=41.972667&minlatitude=32.2605&maxlongitude=-114.998667&minlongitude=-124.995&minmagnitude=0&orderby=time), [query-feb](https://earthquake.usgs.gov/fdsnws/event/1/query.csv?starttime=2020-02-01%2000:00:00&endtime=2020-02-29%2023:59:59&maxlatitude=41.972667&minlatitude=32.2605&maxlongitude=-114.998667&minlongitude=-124.995&minmagnitude=0&orderby=time), [query-mar](https://earthquake.usgs.gov/fdsnws/event/1/query.csv?starttime=2020-03-01%2000:00:00&endtime=2020-03-31%2023:59:59&maxlatitude=41.972667&minlatitude=32.2605&maxlongitude=-114.998667&minlongitude=-124.995&minmagnitude=0&orderby=time), [query-apr](https://earthquake.usgs.gov/fdsnws/event/1/query.csv?starttime=2020-04-01%2000:00:00&endtime=2020-04-30%2023:59:59&maxlatitude=41.972667&minlatitude=32.2605&maxlongitude=-114.998667&minlongitude=-124.995&minmagnitude=0&orderby=time), [query-may](https://earthquake.usgs.gov/fdsnws/event/1/query.csv?starttime=2020-05-01%2000:00:00&endtime=2020-05-31%2023:59:59&maxlatitude=41.972667&minlatitude=32.2605&maxlongitude=-114.998667&minlongitude=-124.995&minmagnitude=0&orderby=time), [query-jun](https://earthquake.usgs.gov/fdsnws/event/1/query.csv?starttime=2020-06-01%2000:00:00&endtime=2020-06-30%2023:59:59&maxlatitude=41.972667&minlatitude=32.2605&maxlongitude=-114.998667&minlongitude=-124.995&minmagnitude=0&orderby=time), [query-jul](https://earthquake.usgs.gov/fdsnws/event/1/query.csv?starttime=2020-07-01%2000:00:00&endtime=2020-07-31%2023:59:59&maxlatitude=41.972667&minlatitude=32.2605&maxlongitude=-114.998667&minlongitude=-124.995&minmagnitude=0&orderby=time), [query-aug](https://earthquake.usgs.gov/fdsnws/event/1/query.csv?starttime=2020-08-01%2000:00:00&endtime=2020-08-31%2023:59:59&maxlatitude=41.972667&minlatitude=32.2605&maxlongitude=-114.998667&minlongitude=-124.995&minmagnitude=0&orderby=time), [query-sep](https://earthquake.usgs.gov/fdsnws/event/1/query.csv?starttime=2020-09-01%2000:00:00&endtime=2020-09-30%2023:59:59&maxlatitude=41.972667&minlatitude=32.2605&maxlongitude=-114.998667&minlongitude=-124.995&minmagnitude=0&orderby=time), [query-oct](https://earthquake.usgs.gov/fdsnws/event/1/query.csv?starttime=2020-10-01%2000:00:00&endtime=2020-10-31%2023:59:59&maxlatitude=41.972667&minlatitude=32.2605&maxlongitude=-114.998667&minlongitude=-124.995&minmagnitude=0&orderby=time), [query-nov](https://earthquake.usgs.gov/fdsnws/event/1/query.csv?starttime=2020-11-01%2000:00:00&endtime=2020-11-30%2023:59:59&maxlatitude=41.972667&minlatitude=32.2605&maxlongitude=-114.998667&minlongitude=-124.995&minmagnitude=0&orderby=time), [query-dec](https://earthquake.usgs.gov/fdsnws/event/1/query.csv?starttime=2020-12-01%2000:00:00&endtime=2020-12-31%2023:59:59&maxlatitude=41.972667&minlatitude=32.2605&maxlongitude=-114.998667&minlongitude=-124.995&minmagnitude=0&orderby=time)
    - If the above links are not available, follow the below instruction.
        1. Open [the link](https://earthquake.usgs.gov/earthquakes/search).
        2. Set the search options as following and click "Search" button at the bottom to download data:
            - Basic Options
                - Magnitude: Custom, Minimum = 0
                - Date & Time: Start/End = 2020-01-01 00:00:00 ~ 2020-01-31 23:59:59, ..., 2020-12-01 00:00:00 ~ 2020-12-31 23:59:59.
                    - You should download the files for all the dates manually since the website limits the maximum results to 20000.
                - Geographic Region: Custom
            - Advanced Options
                - North: 41.972667
                - West: -124.995000
                - East: -114.998667
                - South: 32.260500
            - Output Options
                - Format: CSV
2. Move all downloaded files to `/Datasets/raw_datasets/earthquake/` of this repository.

#### 2. GPS: [SEDAC Gridded Population of the World (GPW), v4](https://sedac.ciesin.columbia.edu/data/set/gpw-v4-admin-unit-center-points-population-estimates-rev11/data-download)

1. Open the link above.
2. On the bottom of the contents, select "North America" of the "Geography" field and "Comma Separated Values (CSV)" of the "FileFormat" field, and then select the checkbox of the "USA: California".
3. Press the "Create Download" to download the data. (It may require logging in to the EARTHDATA.)
4. Unzip the downloaded file and move the unzipped files to `/Datasets/raw_datasets/gps/`.
    - Make sure that the file named `gpw_v4_admin_unit_center_points_population_estimates_rev11_usaca.csv` exists.

#### 3. Shelter: [Homeland Infrastructure Foundation-Level Data (HIFLD)](https://hifld-geoplatform.opendata.arcgis.com/datasets/national-shelter-system-facilities)

1. Open the link above.
2. Select the "Download" button (cloud icon) at the right of the left sidebar.
3. On the left sidebar, select the "Download Options" of the "CSV" and then select "Download file previously generated on ...". 
    - Make sure that the filename is `National_Shelter_System_Facilities.csv`.

#### 4. Road Network: [9th DIMACS Implementation Challenge - Shortest Paths](http://www.diag.uniroma1.it//~challenge9/download.shtml)

1. Download the below files.
    - Links: [USA-road-d.CAL.gr.gz](http://www.diag.uniroma1.it//~challenge9/data/USA-road-d/USA-road-d.CAL.gr.gz), [USA-road-d.CAL.co.gz](http://www.diag.uniroma1.it//~challenge9/data/USA-road-d/USA-road-d.CAL.co.gz)
    - If the above links are not available, follow the below instruction.
        1. Open [the download link](http://www.diag.uniroma1.it//~challenge9/download.shtml).
        2. In the "Challenge benchmarks", download the "Distance graph" and the "Coordinates" of the CAL.
2. Unzip all downloaded files and move the unzipped files to `/Datasets/raw_datasets/roadnetwork/`.
    - Make sure that the filenames are `USA-road-d.CAL.gr` and `USA-road-d.CAL.co`.
    - `ca.json` is provided by us.

#### 5. OSM: [Open Street Map](https://download.geofabrik.de/)

1. Download the below file.
    - Link: [california-latest.osm.bz2](https://download.geofabrik.de/north-america/us/california-latest.osm.bz2)
    - If the above link is not available, follow the below instruction.
        1. Open [the download link of the United States of America](https://download.geofabrik.de/north-america/us.html).
        2. In the "Sub Regions", download the ".osm.bz2" of the "California".
2. Unzip the downloaded file and move the unzipped file to `/Datasets/raw_datasets/osm/`.
    - Make sure that the filename is `california-latest.osm`.

#### 6. Finedust: MISE, An Array-Based Integrated System for Atmospheric Scanning LiDAR (SSDBM 2021)

We provide the Finedust dataset. See `/Datasets/raw_datasets/finedust/`.

### Usage

Set up the datasets described above, then run `Disaster/main.py` from the `/Datasets/Generator` directory.
Result dataset will be saved in the `/Datasets/disaster/` directory.

```bash
$ python Disaster/main.py 
```

### Notes

- Since the Earthquakes (USGS) and OSM have been updated, the datasets (Earthquake, OSM, and Shelter) made from our generator can slightly be different from the dataset of the paper.
- Since the GPS data is generated probabilistically, the dataset made from our generator can slightly be different from the dataset of the paper.

