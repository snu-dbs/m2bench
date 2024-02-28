## Datasets

To assess a multi-model database system using M2Bench, we need to generate datasets that will be fed into the database system.
This README file describes how to generate and scale datasets.

### How to Generate Datasets
- Follow instructions in the `Generator/README.md` file.

### How to Scale Datasets
- Follow instructions in the `DataScaler/README.md` file.

### How to Use Scaled-Up Data

After you scale the generated data, you can find scaled-up files named like `xxx_SFx.xxx`. 
For example, in the `ecommerce/table` directory, you would find `Customer_SF2.csv`, `Customer_SF5.csv`, and `Customer_SF10.csv` files that are scaled-up from the `Customer.csv`.

In order to use those files, simply rename the files to one without `_SFx` (e.g., rename `Customer_SF2.csv` to `Customer.csv`) and do the data loading and task running steps.

### M2Bench Dataset Location 
- Each m2bench dataset is located in one of the three scenraio folders (`ecommerce`, `healthcare`, and `disaster`). 
- Raw datasets (in the `raw_datasets` folder) are required to generate m2bench datasets. Download the required raw\_datasets following instructions in the `Generator` folder.

```
├── Generator                   # Tool for generating datasets
│   ├── Disaster
│   ├── Ecommerce
│   ├── Healthcare
│   └── README.md
│
├── raw_datasets                # Raw datasets required for generating datasets
│   ├── drugbank
│   ├── earthquake
│   ├── finedust
│   ├── gps
│   ├── mimic
│   ├── osm
│   ├── roadnetwork
│   ├── shelter
│   ├── snomed
│   ├── tpcds_data
│   └── Unibench_SF1
│
├── ecommerce                   # Folder where Ecommerce  datasets will be stored after the data generation
│   ├── table
│   ├── json
│   ├── property_graph
│   └── array
│
├── healthcare                  # Folder where Healthcare datasets will be stored after the data generation
│   ├── table
│   ├── json
│   ├── property_graph
│   └── array
│
├── disaster                    # Folder where  Disaster  datasets will be stored after the data generation
│   ├── table
│   ├── json
│   ├── property_graph
│   └── array
│
├── DataScaler                  # Tool for scaling up datasets
│   ├── CMakeLists.txt
│   ├── DataScaler.cpp
│   ├── DataScaler.h
│   ├── disaster_scaler
│   ├── main.cpp
│   ├── README.md
│   ├── scaler.sh
│   └── schema  
└── README.md
```
