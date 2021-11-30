### How to generate Datasets
- Follow instructions in the Generator folder

### How to scale Datasets
- Follow instructions in the DataScaler folder
- 
### M2Bench Dataset Location 
- Each m2bench dataset is located in one of the three scenraio folders (ecommerce, healthcare, disaster). 
- Raw\_datasets (in the raw\_datasets folder) are required to generate m2bench datasets.
- Download the required raw\_datasets following instructions in the Generator folder.

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
