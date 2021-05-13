# Prerequsite

Install neo4j client using following github
```
$ sudo apt-get install -y libedit-dev
$ sudo apt-get install cypher-lint libcypher-parser-dev
$ git clone https://github.com/majensen/libneo4j-client.git
$ cd libneo4j-client
$ ./autogen.sh
$ ./configure  
$ make clean check  # Remove  "-Wno-error=stringop-truncation" in the "configure.ac" file,  if  it invokes an error. 
$ sudo make install
```



Install mysql connector
```
$ sudo apt install libmysqlcppconn-dev
```

Install mongocxx connector

```
http://mongocxx.org/mongocxx-v3/installation/
```



# M2Bench

```

mkdir build

cd build

cmake ..

make

./m2bench

```



