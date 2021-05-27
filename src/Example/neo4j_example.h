//
// Created by mxmdb on 21. 5. 3..
//

#include <iostream>
#include "include/Connection/neo4j_connector.h"

void neo4j_example() {
    std::cout << "\n"
                 "==================\n"
                 "Run Neo4J...\n"
                 "==================\n" << std::endl;

    auto neo4j = new neo4j_connector();

    std::cout << "[NEO4J] Query Example1\n" << std::endl;
    {


        std::string pmatch = "p=(n:Person)-[:knows]-(m)";
        std::string pwhere = "m.firstName=\"Bruna\"";
        std::string preturn = "n.firstName";

        std::string query_str = ("MATCH " + pmatch + " WHERE " + pwhere + " RETURN " + preturn + " LIMIT 5");
        auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
        char buffer[5000];


        while (1) {
            neo4j_result_t *result = neo4j_fetch_next(results);
            if (result == NULL) break;

            int pos = 0;
            while (1) {
                auto n = neo4j_result_field(result, pos);
                if (!neo4j_is_null(n)) {
                    std::string str = neo4j_tostring(n, buffer, sizeof(buffer));
                    cout << str << "\t";
                } else {
                    break;
                }
                pos++;
            }
            cout << endl;

        }
        std::cout << "FINISHED\n\n" << std::endl;
    }

    std::cout << "[NEO4J] Query Example2\n" << std::endl;
    {

        std::string pmatch = "p=(n:Person)-[r:knows]-(m)";
        std::string preturn = "n.firstName, count(r)";

        std::string query_str = ("MATCH " + pmatch + " RETURN " + preturn + " LIMIT 5");
        auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
        char buffer[5000];


        while (1) {
            neo4j_result_t *result = neo4j_fetch_next(results);
            if (result == NULL) break;

            int pos = 0;
            while (1) {
                auto n = neo4j_result_field(result, pos);
                if (!neo4j_is_null(n)) {
                    std::string str = neo4j_tostring(n, buffer, sizeof(buffer));
                    cout << str << "\t";
                } else {
                    break;
                }
                pos++;
            }
            cout << endl;
        }


        std::cout << "FINISHED\n\n" << std::endl;
    }

}