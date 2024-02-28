## Data Scaler

The data scaler produces scaled-up data from the generated data described in `/Datasets/Generator`.
After installing the required software in the `Requirements` section, build this data scaler by following the `How to build` section and then run the command in the `How to run` section.

### Requirements
- GCC
- Cmake (3.17 or above)
- Python 2.7
- Python packages: you can use `requirements.txt` to install the package easily.
    - `geopy`: 1.23.0
    - `pandas`: 0.24.2

### How to build
```bash
mkdir build
cd build
cmake ..
make 
cd ..
```

### How to run
```bash
./scaler.sh [ScaleFactor]
# e.g., ./scaler.sh 2
```
- Scaled datasets are located in each scenario folder. For example, `/Datasets/ecommerce/table/Customer_SF2.csv`
