## Datasets

To assess a multi-model database system using M2Bench, we need to generate datasets that will be fed into the database system.
This README file describes how to generate and scale datasets.

### How to Generate Datasets
- Follow instructions in the `Generator/README.md` file.

### How to Scale Datasets
- Follow instructions in the `DataScaler/README.md` file.

### M2Bench Dataset Location 
- Each m2bench dataset is located in one of the three scenraio folders (`ecommerce`, `healthcare`, `disaster`). 
- Raw datasets (in the `raw_datasets` folder) are required to generate m2bench datasets.
- Download the required raw\_datasets following instructions in the `Generator` folder.

```
├── Generator                   # Tool for generating datasets
│   ├── Disaster
│   ├── Ecommerce
│   ├── Healthcare
│   └── README.md
│
├── raw_datasets                # Raw datasets required for generating datasets
│   ├── earthquake
│   ├── finedust
│   ├── gps
│   ├── osm
│   ├── roadnetwork
│   ├── shelter
│   └── Unibench_SF1
│
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
