#include <iostream>
#include <string>
#include "DataScaler.h"

using namespace std;


void scalingData(int SF, string  path_to_dataset, string path_to_schema ){

    cout << "1. Scale up Ecommerce Datasets ..." << endl;


    cout << "Scale Customer ..." << endl;

    auto ScalingCustomer = DataScaler();
    ScalingCustomer.readSchema(path_to_schema+"schema/customer_schema.json");
    ScalingCustomer.scaleTable(path_to_dataset+"ecommerce/table/Customer.csv", SF, "|");

    cout << "Scale Product ..." << endl;
    auto ScalingProduct = DataScaler();
    ScalingProduct.readSchema(path_to_schema+"schema/product_schema.json");
    ScalingProduct.scaleTable(path_to_dataset+"ecommerce/table/Product.csv", SF, ",");

    cout << "Scale Order ..." << endl;
    string order_path = path_to_dataset+"ecommerce/json/order.json";
    auto ScalingOrder = DataScaler();
    ScalingOrder.readSchema(path_to_schema+"schema/order_schema.json");
    ScalingOrder.scaleJson(order_path, SF);


    cout << "Adjust Product and Order" << endl;
    size_t lastindex = order_path.find_last_of(".");
    string raw_name = order_path.substr(0, lastindex);

    auto order_res = raw_name+"_SF"+to_string(SF)+".json";
    auto order_res_modified = raw_name+"_SF"+to_string(SF)+"_modified.json";

    auto Adjusting = DataScaler();
    Adjusting.AdjustProductAndOrder(path_to_dataset+"ecommerce/table/Product_SF"+to_string(SF)+".csv",
                                    order_res);

    cout << "Replace ..." << endl;
    system(("mv "+order_res_modified+" "+order_res).c_str());

    cout << "Scale Review ..." << endl;

    auto ScalingReview = DataScaler();
    ScalingReview.readSchema(path_to_schema+"schema/review_schema.json");
    ScalingReview.scaleJson(path_to_dataset+"ecommerce/json/review.json", SF);


    cout << "Scale Person ..." << endl;

    auto ScalingPerson = DataScaler();
    ScalingPerson.readSchema(path_to_schema+"schema/person_schema.json");
    ScalingPerson.scaleTable(path_to_dataset+"ecommerce/property_graph/person_node.csv", SF, "|");

    cout << "Scale InterestedIn ..." << endl;
    auto ScalingInterestedIn = DataScaler();
    ScalingInterestedIn.readSchema(path_to_schema+"schema/interested_schema.json");
    ScalingInterestedIn.scaleBipartiteEdge(path_to_dataset+"ecommerce/property_graph/person_interestedIn_tag.csv", SF, ",");

    cout << "Ecommerce Done ...\n\n" << endl;


    cout << "2. Scale up Healthcare Datasets ..." << endl;


    cout << "Scale Patient ..." << endl;
    auto ScalingPatient = DataScaler();
    ScalingPatient.readSchema(path_to_schema+"schema/patient_schema.json");
    ScalingPatient.scaleTable(path_to_dataset+"healthcare/table/Patient.csv", SF, ",");

    cout << "Scale Prescription ..." << endl;

    auto ScalingPrescription = DataScaler();
    ScalingPrescription.readSchema(path_to_schema+"schema/prescription_schema.json");
    ScalingPrescription.scaleTable(path_to_dataset+"healthcare/table/Prescription.csv", SF, ",");

    cout << "Scale Diagnosis ..." << endl;
    auto ScalingDiagnosis = DataScaler();
    ScalingDiagnosis.readSchema(path_to_schema+"schema/diagnosis_schema.json");
    ScalingDiagnosis.scaleTable(path_to_dataset+"healthcare/table/Diagnosis.csv", SF, ",");


    cout << "Healthcare Done ...\n\n" << endl;


}

int main(int argc, char *argv[]) {
{
    if (argc != 4)
    {
        std::cout << "Run ./scaler [ScaleFactor] [Path2Dataset] [Path2Shema]" << std::endl;
        std::cout << "ex) ./scaler 2 ~/m2bench/Datasets/  ~/m2bench/Datasets/DataScaler/" << std::endl;
        return 0;
    }
    else{
        scalingData(atoi(argv[1]), argv[2], argv[3]);
    }
}

    return 0;
}
