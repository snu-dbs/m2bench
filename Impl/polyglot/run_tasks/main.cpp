#include <iostream>
//#include "src/Example/neo4j_example.h"
//#include "src/Example/mysql_example.h"
//#include "src/Example/mongodb_example.h"
#include "src/Scenarios/PolyglotDB/Healthcare_Tasks.h"
#include "src/Scenarios/PolyglotDB/Ecommerce_Tasks.h"
#include "src/Scenarios/PolyglotDB/Disaster_Tasks.h"

#include <chrono>
#include <nlohmann/json.hpp>
#include "src/Example/scidb.h"
#include <string>
#include <Generator/DataScaler.h>
#include <src/Scenarios/PolyglotDB/Latency_Check.h>
#include <string.h>
using json = nlohmann::json;         // for convenience
using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;


template <typename T, typename O>
void timer( void (*f)(T,T,O,O), T x, T y, O a, O b)
{
    cout << "======================= TASK START =========================" << endl;
    auto t1 = high_resolution_clock::now();
    (*f)(x, y, a, b);
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    cout << "ELAPSED TIME: " << ms_int.count() << " ms" << endl;
    cout << "======================= TASK COMPLETE ======================\n" << endl;

}

template <typename T, typename O>
void timer( void (*f)(T,O), T x, O y)
{
    cout << "======================= TASK START =========================" << endl;
    auto t1 = high_resolution_clock::now();
    (*f)(x,y);
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    cout << "ELAPSED TIME: " << ms_int.count() << " ms" << endl;
    cout << "======================= TASK COMPLETE ======================\n" << endl;

}

template <typename T>
void timer( void (*f)(T), T x){
    cout << "======================= TASK START =========================" << endl;
    auto t1 = high_resolution_clock::now();
    (*f)(x);
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    cout << "ELAPSED TIME: " << ms_int.count() << " ms" << endl;
    cout << "======================= TASK COMPLETE ======================\n" << endl;

}

void timer( void (*f)()){
    cout << "======================= TASK START =========================" << endl;
    auto t1 = high_resolution_clock::now();
    (*f)();
    auto t2 = high_resolution_clock::now();
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    cout << "ELAPSED TIME: " << ms_int.count() << " ms" << endl;
    cout << "======================= TASK COMPLETE ======================\n" << endl;

}


void scalingData(int SF){
//    cout << "Scale Customer ..." << endl;
//    auto ScalingCustomer = DataScaler();
//    ScalingCustomer.readSchema("/home/mxmdb/CLionProjects/m2bench_ao/schema/customer_schema.json");
//    ScalingCustomer.scaleTable("/home/mxmdb/m2bench/data/ecommerce/table/Customer.csv", SF, "|");
//    cout << "Done ..." << endl;
//
//    cout << "Scale Product ..." << endl;
//    auto ScalingProduct = DataScaler();
//    ScalingProduct.readSchema("/home/mxmdb/CLionProjects/m2bench_ao/schema/product_schema.json");
//    ScalingProduct.scaleTable("/home/mxmdb/m2bench/data/ecommerce/table/Product.csv", SF, ",");
//    cout << "Done ..." << endl;
//
//    cout << "Scale Order ..." << endl;
//
//    string order_path = "/home/mxmdb/m2bench/data/ecommerce/json/order.json";
//    auto ScalingOrder = DataScaler();
//    ScalingOrder.readSchema("/home/mxmdb/CLionProjects/m2bench_ao/schema/order_schema.json");
//    ScalingOrder.scaleJson(order_path, SF);
//    cout << "Done ..." << endl;
////
//    cout << "Adjust Product and Order" << endl;
//    auto order_res = ScalingOrder.split(order_path, '.')[0]+"_SF"+to_string(SF)+".json";
//    auto order_res_modified = ScalingOrder.split(order_path, '.')[0]+"_SF"+to_string(SF)+"_modified.json";
////
//    auto Adjusting = DataScaler();
//    Adjusting.AdjustProductAndOrder("/home/mxmdb/m2bench/data/ecommerce/table/Product_SF"+to_string(SF)+".csv",
//                                    order_res);
//
//    cout << "Replace ..." << endl;
//    system(("mv "+order_res_modified+" "+order_res).c_str());
//    cout << "Done ..." << endl;
//
//
//
//    cout << "Scale Review ..." << endl;
//
//    auto ScalingReview = DataScaler();
//    ScalingReview.readSchema("/home/mxmdb/CLionProjects/m2bench_ao/schema/review_schema.json");
//    ScalingReview.scaleJson("/home/mxmdb/m2bench/data/ecommerce/json/review.json", SF);
//    cout << "Done ..." << endl;
//
//    cout << "Scale Patient ..." << endl;
//
//    auto ScalingPatient = DataScaler();
//    ScalingPatient.readSchema("/home/mxmdb/CLionProjects/m2bench_ao/schema/patient_schema.json");
//    ScalingPatient.scaleTable("/home/mxmdb/m2bench/data/healthcare/table/Patient.csv", SF, ",");
//    cout << "Done ..." << endl;
//
//    cout << "Scale Prescription ..." << endl;
//
//    auto ScalingPrescription = DataScaler();
//    ScalingPrescription.readSchema("/home/mxmdb/CLionProjects/m2bench_ao/schema/prescription_schema.json");
//    ScalingPrescription.scaleTable("/home/mxmdb/m2bench/data/healthcare/table/Prescription.csv", SF, ",");
//    cout << "Done ..." << endl;
//
//    cout << "Scale Diagnosis ..." << endl;
//    auto ScalingDiagnosis = DataScaler();
//    ScalingDiagnosis.readSchema("/home/mxmdb/CLionProjects/m2bench_ao/schema/diagnosis_schema.json");
//    ScalingDiagnosis.scaleTable("/home/mxmdb/m2bench/data/healthcare/table/Diagnosis.csv", SF, ",");
//    //
//    cout << "Scale Person ..." << endl;
//
//    auto ScalingPerson = DataScaler();
//    ScalingPerson.readSchema("/home/mxmdb/CLionProjects/m2bench_ao/schema/person_schema.json");
//    ScalingPerson.scaleTable("/home/mxmdb/m2bench/data/ecommerce/property_graph/person_node.csv", SF, "|");
//    cout << "Done ..." << endl;

//    cout << "Scale InterestedIn ..." << endl;
//    auto ScalingInterestedIn = DataScaler();
//    ScalingInterestedIn.readSchema("/home/mxmdb/CLionProjects/m2bench_ao/schema/interested_schema.json");
//    ScalingInterestedIn.scaleBipartiteEdge("/home/mxmdb/m2bench/data/ecommerce/property_graph/person_interestedIn_tag.csv", SF, ",");


//    scalingData(2);
//    scalingData(5);
//    scalingData(10);
}

int main(int argc, char *argv[]) {


    cout << "POLYGLOT TEST" << endl;
    string curdate = "2021-06-01";
    timer(T1, curdate);

    timer(T2);

    string curdate3 = "2018-07-07";
    timer(T3, curdate3);

    timer(T4,10000,10);

    string pid = "B007SYGLZO";
    timer(T5,pid, curdate);



    int patient_id = 9
    timer(T6, patient_id);

    timer(T7, patient_id);

    timer(T8, patient_id);

    timer(T9, patient_id);



    string T10_ts1 = "2020-06-01 00:00:00";
    string T10_ts2 = "2020-06-01 02:00:00";
    timer(T10, T10_ts1, T10_ts2);

    int earthquake_X = 41862;
    timer(T11, earthquake_X);


    string T12_ts1 = "2020-09-17 00:00:00";
    string T12_ts2 = "2020-09-17 01:00:00";
    timer(T12,T12_ts1,T12_ts2);



    long ts = 1600182000+10800*3.5;
    timer(T16, ts);

    return 0;
}

