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
    cout << "Scale Customer ..." << endl;
    auto ScalingCustomer = DataScaler();
    ScalingCustomer.readSchema("/home/mxmdb/CLionProjects/m2bench_ao/schema/customer_schema.json");
    ScalingCustomer.scaleTable("/home/mxmdb/m2bench/data/ecommerce/table/Customer.csv", SF, "|");
    cout << "Done ..." << endl;

    cout << "Scale Product ..." << endl;
    auto ScalingProduct = DataScaler();
    ScalingProduct.readSchema("/home/mxmdb/CLionProjects/m2bench_ao/schema/product_schema.json");
    ScalingProduct.scaleTable("/home/mxmdb/m2bench/data/ecommerce/table/Product.csv", SF, ",");
    cout << "Done ..." << endl;

    cout << "Scale Order ..." << endl;

    string order_path = "/home/mxmdb/m2bench/data/ecommerce/json/order.json";
    auto ScalingOrder = DataScaler();
    ScalingOrder.readSchema("/home/mxmdb/CLionProjects/m2bench_ao/schema/order_schema.json");
    ScalingOrder.scaleJson(order_path, SF);
    cout << "Done ..." << endl;

    cout << "Adjust Product and Order" << endl;
    auto order_res = ScalingOrder.split(order_path, '.')[0]+"_SF"+to_string(SF)+".json";
    auto order_res_modified = ScalingOrder.split(order_path, '.')[0]+"_SF"+to_string(SF)+"_modified.json";

    auto Adjusting = DataScaler();
    Adjusting.AdjustProductAndOrder("/home/mxmdb/m2bench/data/ecommerce/table/Product_SF"+to_string(SF)+".csv",
                                    order_res);

    cout << "Replace ..." << endl;
    system(("mv "+order_res_modified+" "+order_res).c_str());
    cout << "Done ..." << endl;




    cout << "Scale Review ..." << endl;

    auto ScalingReview = DataScaler();
    ScalingReview.readSchema("/home/mxmdb/CLionProjects/m2bench_ao/schema/review_schema.json");
    ScalingReview.scaleJson("/home/mxmdb/m2bench/data/ecommerce/json/review.json", SF);
    cout << "Done ..." << endl;

    cout << "Scale Patient ..." << endl;

    auto ScalingPatient = DataScaler();
    ScalingPatient.readSchema("/home/mxmdb/CLionProjects/m2bench_ao/schema/patient_schema.json");
    ScalingPatient.scaleTable("/home/mxmdb/m2bench/data/healthcare/table/Patient.csv", SF, ",");
    cout << "Done ..." << endl;

    cout << "Scale Prescription ..." << endl;

    auto ScalingPrescription = DataScaler();
    ScalingPrescription.readSchema("/home/mxmdb/CLionProjects/m2bench_ao/schema/prescription_schema.json");
    ScalingPrescription.scaleTable("/home/mxmdb/m2bench/data/healthcare/table/Prescrlsiption.csv", SF, ",");
    cout << "Done ..." << endl;

    cout << "Scale Diagnosis ..." << endl;
    auto ScalingDiagnosis = DataScaler();
    ScalingDiagnosis.readSchema("/home/mxmdb/CLionProjects/m2bench_ao/schema/diagnosis_schema.json");
    ScalingDiagnosis.scaleTable("/home/mxmdb/m2bench/data/healthcare/table/Diagnosis.csv", SF, ",");




}

int main(int argc, char *argv[]) {

//    scalingData(2);


//    cout << "TEST" << endl;
//    string curdate = "2021-06-01";
//       timer(T2);

//        timer(T1, curdate);


//    string T10_ts1 = "2020-06-01 00:00:00";
//    string T10_ts2 = "2020-06-01 02:00:00";
//    timer(T10, T10_ts1, T10_ts2);

//    int earthquake_X = 41862;
//    timer(T11, earthquake_X);

//    string T12_ts1 = "2020-09-17 00:00:00";
//    string T12_ts2 = "2020-09-17 01:00:00";
//    timer(T12,T12_ts1,T12_ts2);

//    if(argc == 2 && strcmp( argv[1],"T10") == 0){
//        cout<<"T10 start"<<endl;
//        timer(T10, T10_ts1, T10_ts2);
//    }
//    else if (argc == 2 && strcmp( argv[1],"T11") == 0){
//        cout<<"T11 start"<<endl;
//        timer(T11, earthquake_X);
//    }
//    else if (argc == 2 && strcmp( argv[1],"T12") == 0){
//        cout<<"T12 start"<<endl;
//        timer(T12,T12_ts1,T12_ts2);
//    }

//    else timer(T11, earthquake_X);

//    string T12_ts1 = "2020-09-17 00:00:00";
//    string T12_ts2 = "2020-09-17 01:00:00";
//            timer(T12,T12_ts1,T12_ts2);
//    timer(T2);


//    if(argc == 2 && strcmp( argv[1],"T1") == 0){
//        timer(T1, curdate);
//    }
//    else if (argc == 2 && strcmp( argv[1],"T2") == 0){
//        timer(T2);
//    }
//    else if (argc == 2 && strcmp( argv[1],"T6") == 0){
//        timer(T6, 9);
//    }
//    else if (argc == 2 && strcmp( argv[1],"T16") == 0){
//        timer(T16, ts);
//    }
    //timer(T6, 9);
    //
     // if(argc == 2 && strcmp( argv[1],"T7") == 0){
     //     timer(T7, 9);
     // }
     // else if (argc == 2 && strcmp( argv[1],"T8") == 0){
     //     timer(T8, 9);
     // }
     // else if (argc == 2 && strcmp( argv[1],"T9") == 0){
     //     timer(T9, 9);
     // }
//         timer(T8,9);
//    long ts = 1600182000+10800*3.5;
//    timer(T16, ts);

<<<<<<< HEAD
//    string curdate3 = "2018-07-07";//"2021-12-25";
//    string pid = "B007SYGLZO";
//    string curdate = "2021-06-01";
//    if(argc == 2 && strcmp( argv[1],"T3") == 0){
//        timer(T3, curdate3);
//    }
//    else if (argc == 2 && strcmp( argv[1],"T4") == 0){
//    timer(T4,10000,10);
//    }
//    else if (argc == 2 && strcmp( argv[1],"T5") == 0){
//        timer(T5,pid, curdate);
//    }
//    //timer(T3,curdate3);
=======
    //string curdate3 = "2018-07-07";//"2021-12-25";
    //string pid = "B007SYGLZO";
    //string curdate = "2021-06-01";
    //if(argc == 2 && strcmp( argv[1],"T3") == 0){
    //    timer(T3, curdate3);
    //}
    //else if (argc == 2 && strcmp( argv[1],"T4") == 0){
//    timer(T4,10000,10);
    //}
    //else if (argc == 2 && strcmp( argv[1],"T5") == 0){
    //    timer(T5,pid, curdate);
    //}
    //timer(T3,curdate3);
>>>>>>> e9426e831660e6a6629a1b7b8a806ebfd55b506a

      return 0;
}

