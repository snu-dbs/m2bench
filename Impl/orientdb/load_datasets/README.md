# Data Loading Instruction for OrientDB 

This document describes how to load data to the OrientDB. 

Before start, please be aware that:

- Make sure that OrientDB is installed in `/opt/orientdb/`. 
- Please do not change the directory structure.

To load datasets to the OrientDB, you should do the following:

1. Create Databases (`Creating Databases` section).
2. Load datasets using the ETL tool of the OrientDB (`Loading Datasets` section).
3. Make links for referencing between classes (`Creating Links` section).

## Creating Databases

Before loading datasets, you should create databases. In this instruction, we will create databases named  `Ecommerce` and `Healthcare`. 

1. Run the `console.sh` .

```bash
sudo /opt/orientdb/bin/console.sh
```

2. After run the console, connect to the OrientDB server and run the command to create `Ecommerce` databases. You should replace `<IP_ADDRESS>` and `<PASSWORD>` to your IP address and root password, respectively.

```sql
CONNECT remote:<IP_ADDRESS>:2424/ root <PASSWORD>
CREATE DATABASE remote:<IP_ADDRESS>:2424/Ecommerce root <PASSWORD>
```

3. Then, execute the contents of `/Impl/orientdb/load_datasets/ecommerce/initialize.sql`. You can just copy the contents of the file and paste it to the console.

3. Create `Healthcare`  database. Similar to the number 2, you should replace `<IP_ADDRESS>` and `<PASSWORD>`  to your IP address and root password, respectively.

```sql
CREATE DATABASE remote:<IP_ADDRESS>:2424/Healthcare root <PASSWORD>
```

5. Then, execute the contents of `/Impl/orientdb/load_datasets/healthcare/initialize.sql`. You can just copy the contents of the file and paste it to the console.



## Loading Datasets 

Use the following commands in the  `/Impl/orientdb/load_datasets/`  directory. 

```bash
bash ./ecommerce/import.sh
bash ./healthcare/import.sh
```

If the commands cause `com.orientechnologies.orient.core.exception.OStorageException: Database is locked by another process, please shutdown process and try again` error, then restart the OrientDB server (It seems a bug).



## Creating Links

To use the OrientDB properly, you should create links between classes. 

1. Run the `console.sh` .

```bash
sudo /opt/orientdb/bin/console.sh
```

2. After run the console, connect to the OrientDB server and then execute the contents of `/Impl/orientdb/load_datasets/ecommerce/links.sql`. You should replace `<IP_ADDRESS>` and `<PASSWORD>`  to your IP address and root password, respectively.

```sql
CONNECT remote:<IP_ADDRESS>:2424/Ecommerce root <PASSWORD>
```

3. Repeat the above for the `Healthcare`. You should use `/Impl/orientdb/load_datasets/healthcare/links.sql` instead of the `ecommerce` one.

