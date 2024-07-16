//
// Created by mxmdb on 21. 7. 6..
//

#ifndef M2BENCH_AO_DATASCALER_H
#define M2BENCH_AO_DATASCALER_H
#include <map>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <boost/algorithm/string.hpp>
using json = nlohmann::json;
using namespace std;
class DataScaler {

public:
    map<string, string> types;
    vector<string> ordered_attrs;

    vector<string> split(string input, char delimiter) {
        vector<string> answer;
        string temp ="";
        int quote =0 ;
        for ( int s = 0; s < input.size() ; s++){
            if( quote == 0 ){
                if(input[s]!= delimiter){
                    temp+=input[s];
                    if( input[s] == '"'){
                        quote = 1;
                    }
                }
                else {
                    answer.push_back(temp);
                    temp = "";
                }
            }
            else{
                temp+=input[s];
                if( input[s] == '"'){
                    quote = 0;
                }
            }
        }
        answer.push_back(temp);

        return answer;
    }



    void readSchema(string filename){

        std::ifstream inputfile(filename);
        if(inputfile.fail()){
            return  ;
        }

        json j;
        inputfile >> j;

        for( auto val : j.items()){
            string key = val.key();
            string value = val.value();
            types[key] = value;
        }
//        for( auto val :  types ){
//            cout << val.first << "\t" << val.second << endl;
//        }
    }

    string genValue(string type , int sf, int SF,  string value)
    {
        // pk or fk -> scalable pk
        if( type == "pk_int"  ||  type == "fk_int_sf" ){
            long long pk = atoll(value.c_str())*SF;
            return to_string(pk+sf);
        }
        else if( type == "pk_string" || type == "fk_string_sf" )
        {
            return value+to_string(sf);
        }
            // fk -> non scalable pk
        else if( type == "fk_int" ||type == "fk_string" )
        {
            return value;
        }
            // category
        else if( type== "category" )
        {
            return value;
        }
        else if( type== "category_int" )
        {
            return value;
        }
            // int
        else if(type == "int" ){
            long long valueL =  atoll(value.c_str());
            long long random = rand() % (int) ceil((double)valueL/10.0);
            if( rand() % 2 == 0 ) return to_string(valueL - random);
            else return to_string(valueL + random);
        }
        else if(type == "double" ){
            float valuef =  atof(value.c_str());
            float random = (double) rand() / double( RAND_MAX);
            if( rand() % 2 == 0 ) return to_string(valuef - 0.1*random*valuef);
            else return to_string(valuef + 0.1*random*valuef);
        }
            // string
        else if(type == "string" ){
            int random  = rand() % value.size();
            while (value[random] == '"'){
                random  = rand() % value.size();
            }
            value[random] = 'a' + rand()%26;

            return value;
        }
        // custom field
        else if(type == "date(YYYY-MM-DD)" ){

            int a, b, c;
            char rand_date[11];
            sscanf(value.c_str(), "%d-%d-%d", &a, &b, &c);
            if( b == 2 ){
                sprintf(rand_date, "%d-%02d-%02d", a, b, rand()%28+1);
                return rand_date;
            }
            else if( b%2 == 1){
                sprintf(rand_date, "%d-%02d-%02d", a, b, rand()%30+1);
                return rand_date;
            }
            else{
                sprintf(rand_date, "%d-%02d-%02d", a, b, rand()%30+1);
                return rand_date;
            }
        }
        else if(type == "date(YYYY-MM-DD HH:MM:SS)" ){

            int a, b, c, d, e, f;
            char rand_date[20];
            sscanf(value.c_str(), "%d-%d-%d %d:%d:%d", &a, &b, &c, &d, &e, &f);
            if( b == 2 ){
                sprintf(rand_date, "%d-%02d-%02d %02d:%02d:%02d", a, b, rand()%28+1, rand()%24, rand()%60, rand()%60);
                return rand_date;
            }
            else if( b%2 == 1){
                sprintf(rand_date, "%d-%02d-%02d %02d:%02d:%02d", a, b, rand()%30+1, rand()%24, rand()%60, rand()%60);
                return rand_date;
            }
            else{
                sprintf(rand_date, "%d-%02d-%02d %02d:%02d:%02d", a, b, rand()%30+1, rand()%24, rand()%60, rand()%60);
                return rand_date;
            }
        }
        else if(type == "date(DD/MM/YYYY/HH:MM:SS)"){

            int a, b, c, d, e, f;
            char rand_date[20];
            sscanf(value.c_str(), "%d/%d/%d/%d:%d:%d", &a, &b, &c, &d, &e, &f);
            if( b == 2 ){
                sprintf(rand_date, "%02d/%02d/%d/%02d:%02d:%02d", rand()%28+1, b, c, rand()%24, rand()%60, rand()%60);
                return rand_date;
            }
            else if( b%2 == 1){
                sprintf(rand_date, "%02d/%02d/%d/%02d:%02d:%02d", rand()%30+1, b, c, rand()%24, rand()%60, rand()%60);
                return rand_date;
            }
            else{
                sprintf(rand_date, "%02d/%02d/%d/%02d:%02d:%02d", rand()%30+1, b, c, rand()%24, rand()%60, rand()%60);
                return rand_date;
            }

        }
        else if(type== "email" ) {
            string id = "abcdefghij";
            for (int i = 0; i < 10; i++) {
                id[i] = 'a' + rand() % 26;
            }
            int val = rand();
            if ( val % 3 == 0 ) return (id + "@gmail.com");
            if ( val % 3 == 1 ) return (id + "@daum.com");
            if ( val % 3 == 2 ) return (id + "@naver.com");
        }
        else{
            cout << "NOT SUPPORTED TYPE:" << type <<":"<< value<< endl;
            return "";
        }
    }

    string getValue(json j)
    {
        if( j.is_number_integer() ) return to_string(j.get<int>());
        if( j.is_number_float() ) return to_string(j.get<float>());
        if( j.is_string()) return j.get<string>();

    }

    void scaleTable(string filename, int SF, string separater=","){

        std::ifstream inputfile(filename);
        if(inputfile.fail()){
            return  ;
        }
        string line = "";
        int nline = 0 ;
	size_t lastindex = filename.find_last_of(".");
        string rawname = filename.substr(0, lastindex);

        auto outfile = ofstream(rawname+"_SF"+to_string(SF)+".csv");



        while (std::getline(inputfile, line))
        {
            if ( nline == 0){
                auto headers =  split(line, separater[0]);
                int iter = 0;
                for ( auto header: headers ){
                    ordered_attrs.push_back(header);
                    outfile.write(header.c_str(),header.size());
                    if( iter != headers.size()-1){
                        outfile.write(separater.c_str(),1);
                    }
                    else{
                        outfile.write("\n", 1);
                    }
                    iter++;
                }
            }
            else{
                auto values =  split(line,separater[0]);
                for ( int sf = 0 ; sf < SF ; sf++) {
                    for ( int i =  0 ; i < types.size(); i++) {

                        if( values[i] != "")
                        {
                            string genval = genValue(types[ordered_attrs[i]], sf,  SF, values[i]);
                            outfile.write(genval.c_str(), genval.size());
                        }
                        if( i != values.size()-1){
                            outfile.write(separater.c_str(),1);
                        }
                        else{
                            outfile.write("\n", 1);
                        }
                        outfile.flush();
                    }
                }
            }
            nline++;
        }
        outfile.flush();
    }

    void scaleBipartiteEdge(string filename, int SF, string separater=","){

        std::ifstream inputfile(filename);
        if(inputfile.fail()){
            return  ;
        }
        string line = "";
        int nline = 0 ;


	size_t lastindex = filename.find_last_of(".");
	string rawname = filename.substr(0, lastindex); 
        
	auto outfile = ofstream(rawname+"_SF"+to_string(SF)+".csv");

        while (std::getline(inputfile, line))
        {
            if ( nline == 0){
                auto headers =  split(line, separater[0]);
                int iter = 0;
                for ( auto header : headers ){


                    ordered_attrs.push_back(header);

                    outfile.write(header.c_str(),header.size());
                    if( iter != headers.size()-1){
                        outfile.write(separater.c_str(),1);
                    }
                    else{
                        outfile.write("\n", 1);
                    }
                    iter++;
                }
            }
            else{
                auto values =  split(line,separater[0]);
                for ( int sf_dest = 0 ; sf_dest < SF  ; sf_dest++ ) {
                    for (int sf_source = 0; sf_source < SF; sf_source++) {
                        for (int i = 0; i < types.size(); i++) {

                            if (i == 0) { //SOURCE
                                if (values[i] != "") {
                                    string genval = genValue(types[ordered_attrs[i]], sf_source, SF, values[i]);
                                    outfile.write(genval.c_str(), genval.size());
                                }
                            } else if (i == 1) { //DESTINATIOn
                                if (values[i] != "") {
                                    string genval = genValue(types[ordered_attrs[i]], sf_dest, SF, values[i]);
                                    outfile.write(genval.c_str(), genval.size());
                                }
                            } else {
                                if (values[i] != "") {
                                    string genval = genValue(types[ordered_attrs[i]], sf_source, SF, values[i]);
                                    outfile.write(genval.c_str(), genval.size());
                                }
                            }

                            if (i != values.size() - 1) {
                                outfile.write(separater.c_str(), 1);
                            } else {
                                outfile.write("\n", 1);
                            }
                            outfile.flush();
                        }
                    }
                }
            }
            nline++;
        }
        outfile.flush();
    }


    void scaleJson(string filename, int SF){

        std::ifstream inputfile(filename);
        if(inputfile.fail()){
            return  ;
        }
        string line = "";
        int nline = 0 ;

        size_t lastindex = filename.find_last_of(".");
        string rawname = filename.substr(0, lastindex);

        auto outfile = ofstream(rawname+"_SF"+to_string(SF)+".json");

        while (std::getline(inputfile, line))
        {
            json doc;
            stringstream stream;
            stream.str(line);
            stream >> doc;
            for ( int sf = 0 ; sf < SF ; sf++) {
                json x = doc;

                for (auto type : types){
                    auto attrs = split(type.first,'.');
                    if( attrs.size() == 1){
                        if (  x.contains(attrs[0])) {
                            string gval = genValue(type.second, sf, SF, getValue(x[attrs[0]]));
                            if (type.second.find("int") != std::string::npos) {
                                x[attrs[0]] = atoll(gval.c_str());
                            } else if (type.second.find("double") != std::string::npos) {
                                x[attrs[0]] = atof(gval.c_str());
                            } else {
                                x[attrs[0]] = gval;
                            }
                        }

//                        if  (x[attrs[0]].is_null())    {std::cout << "is null\n";}
//                        if  (x[attrs[0]].is_boolean()) {std::cout << "is bool\n";}
//                        if  (x[attrs[0]].is_number())  {std::cout << "is numb\n";}
//                        if  (x[attrs[0]].is_object())  {std::cout << "is obj\n";}
//                        if (x[attrs[0]].is_array())   {std::cout << "is array\n";}
//                        if (x[attrs[0]].is_string())  {std::cout << "is string\n";}
                    }
                    else if (attrs.size() == 2) {
                        if (x.contains(attrs[0])) {
                            if (x[attrs[0]].is_array()) {
                                vector<json> vec;
                                for (auto elem :  x[attrs[0]]) {

                                    if (elem.contains(attrs[1])) {
                                        string gval = genValue(type.second, sf, SF, getValue(elem[attrs[1]]));
                                        if (type.second.find("int") != std::string::npos) {
                                            elem[attrs[1]] = atoll(gval.c_str());
                                        } else if (type.second.find("double") != std::string::npos) {
                                            elem[attrs[1]] = atof(gval.c_str());
                                        } else {
                                            elem[attrs[1]] = gval;
                                        }
                                        vec.push_back(elem);
                                    }
                                }
                                x[attrs[0]] = vec;
                            } else {
                                if (x.contains(attrs[0]) && x[attrs[0]].contains(attrs[1])) {
                                    string gval = genValue(type.second, sf, SF, getValue(x[attrs[0]][attrs[1]]));
                                    if (type.second.find("int") != std::string::npos) {
                                        x[attrs[0]][attrs[1]] = atoll(gval.c_str());
                                    } else if (type.second.find("double") != std::string::npos) {
                                        x[attrs[0]][attrs[1]] = atof(gval.c_str());
                                    } else {
                                        x[attrs[0]][attrs[1]] = gval;
                                    }
                                }
                            }
                        }
                    }
                }
                outfile<<  x << endl;
             }
        }
        outfile.flush();
    }


    void AdjustProductAndOrder(string ProductFilePath, string OrderFilePath){

        std::ifstream inputfile(ProductFilePath);
        if(inputfile.fail()){
            return  ;
        }
        string line = "";
        int nline = 0 ;
        int id_pos = 0 ;
        int title_pos = 0 ;
        int price_pos = 0 ;

        map<string, string> id2title;
        map<string, double> id2price;

        while (std::getline(inputfile, line))
        {
            if ( nline == 0){
                auto headers =  split(line, ',');
                int iter = 0;
                for ( auto header : headers ){
                    if (header == "product_id" ) {
                        id_pos = iter;
                    }
                    if (header == "title" ) {
                        title_pos = iter;
                    }
                    if( header == "price")  {
                        price_pos = iter;
                    }
                    iter++;
                }
            }
            else {
                auto values = split(line, ',');
                id2title[values[id_pos]] = values[title_pos];
                id2price[values[id_pos]] = atof(values[price_pos].c_str());
            }
            nline++;
        }


        std::ifstream inputfile2(OrderFilePath);
        if(inputfile2.fail()){
            return  ;
        }

        line = "";
	
       	size_t lastindex = OrderFilePath.find_last_of(".");
 	string raw_name = OrderFilePath.substr(0, lastindex);

        auto outfile = ofstream(raw_name+"_modified.json");

        while (std::getline(inputfile2, line))
        {
            json doc;
            stringstream stream;
            stream.str(line);
            stream >> doc;
            json x = doc;

            double total_price = 0;

            vector<json> vec;
            for (auto elem :  x["order_line"]){
                elem["price"] = id2price[elem["product_id"]];

                auto titlem = id2title[elem["product_id"]];
                boost::erase_all(titlem, "\"");

                elem["title"] = titlem;
                vec.push_back(elem);
                total_price += elem["price"].get<double>();
            }
            x["order_line"] =vec;
            x["total_price"] = total_price;
             outfile<<  x << endl;
        }

        outfile.flush();

    }

    DataScaler(){

    }
    ~DataScaler(){

    }
};


#endif //M2BENCH_AO_DATASCALER_H
