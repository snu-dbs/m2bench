//
// Created by mxmdb on 21. 5. 3..
//
#include <iostream>
#include "include/Connection/mongodb_connector.h"

#include <mongocxx/client.hpp>

#include <bsoncxx/builder/stream/document.hpp>
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;


void mongodb_example(){
        std::cout << "\n"
                     "==================\n"
                     "Run MongoDB ...\n"
                     "==================\n" << std::endl;

        std::cout << "[MONGODB] Query Example1\n" << std::endl;


        auto mongodb = new mongodb_connector("mxmdb");

        auto db = mongodb-> db;

        {
            auto collection = db["invoice"];

            bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result = collection.find_one({});
            if(maybe_result) {
                std::cout << bsoncxx::to_json(*maybe_result) << "\n";
            }
            std:: cout << "\n\n" << std::endl;
        }

        std::cout << "[MONGODB] Query Example2\n" << std::endl;
        {
            using bsoncxx::builder::basic::kvp;
            using bsoncxx::builder::basic::make_document;

            auto collection = db["invoice"];

            /**
             * Find invoices of which the orderline contains the brand = "Onda_(sportswear)"
             *
             * Invoice = { ~
             *            "Orderline" : [{ ~,
             *                  "brand" : "Onda_(sportswear)",
             *                  ~
             *                  }],
             *           }
             *
             */

            auto cursor = collection.find(make_document(kvp("Orderline.productId",810)));


            int limit = 5;
            int ndocs = 0;
            for( auto doc : cursor){
                std::cout << bsoncxx::to_json(doc) << std::endl;
                ndocs++;
                {
                    bsoncxx::document::element stored_element{doc["PersonId"]};
                    if (stored_element) {
                        std::cout << "Examining PersonId " << stored_element.get_int64() << std::endl;
                        std::cout << "Type of PersonId " << bsoncxx::to_string(stored_element.type()) << std::endl;
                    }
                }
                {
                    bsoncxx::document::element stored_element{doc["Orderline"]};
                    if (stored_element) {
                        std::cout << "Examining Orderline " << bsoncxx::to_json(stored_element.get_array().value) << std::endl;
                    }
                }

                {
                    auto orderlines =  doc["Orderline"].get_array().value;
                    for ( auto& orderline : orderlines ){
                        auto subdoc = orderline.get_document().value;
                        bsoncxx::document::element stored_element{subdoc["price"]};
                        if (stored_element) {
                            double price = 0;
                            if( "int32" == bsoncxx::to_string(stored_element.type())) price = stored_element.get_int32().value;
                            if( "int64" == bsoncxx::to_string(stored_element.type())) price = stored_element.get_int64().value;
                            if( "double" == bsoncxx::to_string(stored_element.type())) price = stored_element.get_double().value;
                            std::cout << "Examining price " << price  << std::endl;
                        }
                    }

                }

                if( ndocs > limit){ break; }
            }

        }

        delete mongodb;

};