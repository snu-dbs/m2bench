//
// Created by mxmdb on 21. 5. 3..
//
#include <iostream>
#include "include/Connection/mysql_connector.h"

void mysql_example(){
    std::cout << "\n"
                 "==================\n"
                 "Run Mysql...\n"
                 "==================\n" << std::endl;

    std::cout << "[MYSQL] Query Example1\n" << std::endl;
    auto mysql = new mysql_connector();
    auto myTable = mysql->mysess->getSchema("umls").getTable("MRCONSO");

    auto result = myTable.select("*").limit(5).execute();
    auto rows = result.fetchAll();
    for( auto row : rows){
        for(int i = 0 ;  i  < row.colCount() ; i++) {
            cout << row[i] << "\t";
        }
        cout << endl;
    }
    std::cout<< "FINISHED\n\n" << std::endl;

    std::cout << "[MYSQL] Query Example2\n" << std::endl;
    mysql->mysess->sql("use umls").execute();
    auto sql1 = mysql->mysess->sql("select * from MRCONSO where LAT Like ? and STT Like ? limit 5");
    auto result2 = sql1.bind("ENG", "VC").execute();
    auto rows2 = result2.fetchAll();
    for( auto row : rows2){
        for(int i = 0 ;  i  < row.colCount() ; i++) {
            cout << row[i] << "\t";
        }
        cout << endl;
    }
    std::cout<< "FINISHED\n\n" << std::endl;

    delete mysql;
}