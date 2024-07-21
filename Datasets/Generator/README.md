# How to generate datasets

This README file describes how to generate datasets for each scenario that M2Bench provides, namely E-commerce, Healthcare, and Disaster & Safety.
Before getting started, please ensure you have prepared for the software described in the below "Requirements" section.

### Requirements
- GCC 9
- Python 3.8
- Python packages described in `requirements.txt` file (you can install the packages by `pip install -r requirements.txt`)
- GDAL 2.4.2
- `bison`, `byacc`, and `flex` (to build TPC-DS)

### How to
To generate datasets, you should do the following for each scenario (E-commerce, Healthcare, and Disaster & Safety):

1. Prepare the raw datasets (the `Preparation for raw datasets` section).
2. Run the data generator (the `Data generation` section).


## E-commerce

### Preparation for raw datasets
M2bench E-commerce datasets are generated based on TPC-DS and Unibench datasets. 
Thus, you need to prepare for these datasets.

#### 1. TPC-DS dataset (SF=1): [http://tpc.org/tpc_documents_current_versions/download_programs/tools-download-request5.asp?bm_type=TPC-DS](http://tpc.org/tpc_documents_current_versions/download_programs/tools-download-request5.asp?bm_type=TPC-DS&bm_vers=3.2.0)
- The TPC Tools are available free of charge, however all users must agree to the licensing terms and register prior to use.
      Please download and read the TPC-Tools License Agreement prior to registering for the download.
- After downloading, unzip the TPC-DS tool, navigate to the `tools` directory, and make generate binary files required to generate TPC-DS data.
```bash
$ make -f Makefile.suite
```

> **If you encounter a build error..**
> 
> If you encounter the multiple definition error during the build, please make sure that you are building TPC-DS with GCC 9.

- Run the following command in the `tools` directory of TPC-DS. It will generate the TPC-DS data of scale factor 1 to the `/Datasets/raw_datasets/tpcds_data` directory.
```bash
$ ./dsdgen -scale 1 -dir path/to/Datasets/raw_datasets/tpcds_data/
```

#### 2. Unibench dataset (SF=1)
- Since Unibench is no longer available from [the author's website](http://udbms.cs.helsinki.fi/?projects/ubench), we provide the Unibench dataset.
Please download the dataset from [here](https://drive.google.com/file/d/1Fai0jkswYn6O7X9Yg7wN5ggF8Qsa_DLf/view?usp=drive_link) and unzip it to the `/Datasets/raw_datasets/Unibench_SF1` directory.


### Data generation
- Navigate to the `/Datasets/Generator/Ecommerce` directory.
- Run the following command. It will generate data to the `/Datasets/ecommerce/` directory.

```bash
$ python3 main.py ../../raw_datasets/tpcds_data/ ../../raw_datasets/Unibench_SF1/
```
    
### Notes
- Since the TPC-DS Tool is steadily updated, the version of the tool used to generate datasets in M2Bench (v.3.0.1) can be different from the latest version.
- We also provide synthetic hashtag node data as a part of property graph dataset. It is located in the `/Datasets/ecommerce/property_graph` directory.


## Healthcare

### Preparation for raw datasets

#### 1. MIMIC III Clinical Dataset: [https://physionet.org/content/mimiciii/](https://physionet.org/content/mimiciii/)

- Please access the above link and download the MIMIC-III clinical dataset after fulfilling the requirements described on the website.
- Unzip the downloaded data. Note that the downloaded data also has zipped files. Please unzip these files as well.
- Put the unzipped files into the `/Datasets/raw_datasets/mimic` directory.

#### 2. SNOMED Terminology: [https://www.nlm.nih.gov/healthit/snomedct/us_edition.html](https://www.nlm.nih.gov/healthit/snomedct/us_edition.html)

- Before downloading, you have to set up an account for UMLS Terminology Services (UTS) (https://uts.nlm.nih.gov/uts/) to access the data.
- Download by clicking on the download now button.
- You would get a directory named `SnomedCT_xxx`. Please copy the two files below to the `/Datasets/raw_datasets/snomed` directory and rename these files. 
    - Copy the `SnomedCT_xxx/Snapshot/Terminology/ xxx_Description_Snapshot_xxx.txt` file and rename it to `description.txt`.
    - Copy the `SnomedCT_xxx/Snapshot/Terminology/ xxx_Relationship_Snapshot_xxx.txt` file and rename it to `relationship.txt`.


#### 3. Drugbank (drug json data): [https://portal.drugbank.com/](https://portal.drugbank.com/)

- Before downloading, you have to set up an account for Drugbank (https://go.drugbank.com/public_users/sign_up) and get approved to access the data.
- Download the latest DrugBank data in JSON format.
- Put the JSON files of the downloaded data into the `/Datasets/raw_datasets/drugbank` directory.


### Data generation 
- Navigate to the `/Datasets/Generator/Healthcare` directory and run the following command.

```bash
$ python3 main.py
```

### Notes

- Make sure the dataset directory names do not contain characters such as "()". Rename the folders if needed. 
- We provide `ICD9CM_SNOMED_MAP_1TO1_202012.txt` file for table data generation. Make sure the file is located in the same directory where you run `main.py`.


## Disaster & Safety 

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
2. Move all downloaded files to `/Datasets/raw_datasets/earthquake` of this repository.

#### 2. GPS: [SEDAC Gridded Population of the World (GPW), v4](https://sedac.ciesin.columbia.edu/data/set/gpw-v4-admin-unit-center-points-population-estimates-rev11/data-download)

1. Open the link above.
2. On the bottom of the contents, select "North America" of the "Geography" field and "Comma Separated Values (CSV)" of the "FileFormat" field, and then select the checkbox of the "USA: California".
3. Press the "Create Download" to download the data. (It may require logging in to the EARTHDATA.)
4. Unzip the downloaded file and move the unzipped files to `/Datasets/raw_datasets/gps`.
    - Make sure that the file named `gpw_v4_admin_unit_center_points_population_estimates_rev11_usaca.csv` exists.

#### 3. Shelter: [Homeland Infrastructure Foundation-Level Data (HIFLD)](https://hifld-geoplatform.opendata.arcgis.com/datasets/national-shelter-system-facilities)

1. Open the link above. Please be patient because it might take a long time to access.
2. Select the "Download" button (cloud icon) at the right of the left sidebar.
3. On the left sidebar, select the "Download Options" of the "CSV" and then select "Download file previously generated on ...". 
    - Make sure that the filename is `National_Shelter_System_Facilities.csv`.
4. Place the CSV file to `/Datasets/raw_datasets/shelter`.

#### 4. Road Network: [9th DIMACS Implementation Challenge - Shortest Paths](http://www.diag.uniroma1.it//~challenge9/download.shtml)

1. Download the below files.
    - Links: [USA-road-d.CAL.gr.gz](http://www.diag.uniroma1.it//~challenge9/data/USA-road-d/USA-road-d.CAL.gr.gz), [USA-road-d.CAL.co.gz](http://www.diag.uniroma1.it//~challenge9/data/USA-road-d/USA-road-d.CAL.co.gz)
    - If the above links are not available, follow the below instruction.
        1. Open [the download link](http://www.diag.uniroma1.it//~challenge9/download.shtml).
        2. In the "Challenge benchmarks", download the "Distance graph" and the "Coordinates" of the CAL.
2. Unzip all downloaded files and move the unzipped files to `/Datasets/raw_datasets/roadnetwork`.
    - Make sure that the filenames are `USA-road-d.CAL.gr` and `USA-road-d.CAL.co`.
    - Please note that you can find the `ca.json` file in the `roadnetwork` directory. This file is used to generate data.

#### 5. OSM: [Open Street Map](https://download.geofabrik.de/)

1. Download the below file.
    - Link: [california-latest.osm.bz2](https://download.geofabrik.de/north-america/us/california-latest.osm.bz2)
    - If the above link is not available, follow the below instruction.
        1. Open [the download link of the United States of America](https://download.geofabrik.de/north-america/us.html).
        2. Under the "Sub Regions", download the ".osm.bz2" of the "California".
2. Unzip the downloaded file and move the unzipped file to `/Datasets/raw_datasets/osm`.
    - Make sure that the filename is `california-latest.osm`.

#### 6. Finedust: MISE, An Array-Based Integrated System for Atmospheric Scanning LiDAR (SSDBM 2021)

We provide the Finedust dataset. 
Please download the dataset from [here](https://drive.google.com/file/d/1jQU2V4LEfF8vJnkqxpWsN7LQlAhT_CHU/view?usp=drive_link) and unzip it to the `/Datasets/raw_datasets/finedust` directory.

### Data generation

Set up the raw datasets described above, then run `Disaster/main.sh` **from the `/Datasets/Generator` directory**.
Result data will be stored in the `/Datasets/disaster` directory.

```bash
$ bash Disaster/main.sh
```

### Notes

- Since the Earthquakes (USGS) and OSM have been updated, the datasets (Earthquake, OSM, and Shelter) made from our generator can slightly be different from the dataset in the paper.
- Since the GPS data is generated probabilistically, the dataset made from our generator can slightly be different from the dataset in the paper.
- If you have trouble with the generator, please try [the other version of the data generator](https://github.com/snu-dbs/m2bench/tree/bdcbc21437a0b50585cfc28bec107e046d7a9d4f). 
