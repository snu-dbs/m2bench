//
// Created by mxmdb on 21. 5. 3..
//
#include "include/Connection/mysql_connector.h"
#include "include/Connection/mongodb_connector.h"
#include <nlohmann/json.hpp>
#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <Connection/ScidbConnection.h>
#include <math.h>
#include <time.h>

#include <boost/geometry.hpp>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

//using bsoncxx::builder::stream::document;
//using bsoncxx::builder::stream::open_array;
//using bsoncxx::builder::stream::value;

using Json = nlohmann::json;

#define SCIDB_HOST_DISASTER    "192.168.0.1"

double distance(double lat, double lon, double lat2, double lon2){
    return sqrt(( lat - lat2)*(lat - lat2) + (lon - lon2)*(lon - lon2));
}

const double pi = acos(-1);
double deg2rad(double deg) {
    return deg / 180 * pi;
}

double distance_haversine(double lat1, double lon1, double lat2, double lon2) {
    return 2 * 6378 * asin(sqrt(pow(sin(deg2rad((lat2 - lat1) / 2)), 2) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * pow(sin(deg2rad((lon2 - lon1) / 2)), 2)));
}

// Deprecated
std::tuple<double, double> ST_Centroid(bsoncxx::types::b_document geometry) {
    double cLon, cLat;
    auto geojsonType = geometry.value["type"].get_utf8().value.to_string();
    if (geojsonType.compare("MultiPolygon") == 0) {
        auto p = geometry.value["coordinates"].get_array().value;
        // calculating centroid
        double lonsum = 0, latsum = 0;
        int count = 0;
        for (auto pit = p.begin(); pit != p.end(); pit++) {
            auto lss = pit->get_array().value;
            for (auto lssit = lss.begin(); lssit != lss.end(); lssit++) {
                auto ls = lssit->get_array().value;
                for (auto lsit = ls.begin(); lsit != ls.end(); lsit++) {
                    auto po = lsit->get_array().value;
                    lonsum += po[0].get_double();
                    latsum += po[1].get_double();
                    count++;
                }
            }
        }
        cLon = lonsum / count;
        cLat = latsum / count;
    } else {
        cLon = geometry.value["coordinates"].get_array().value[0].get_double();
        cLat = geometry.value["coordinates"].get_array().value[1].get_double();
    }

    return std::make_tuple(cLon, cLat);
}

namespace bg = boost::geometry;

std::tuple<double, double> ST_Centroid_bd_test(string geom) {
    typedef bg::model::d2::point_xy<double> point_type;
    typedef bg::model::polygon<point_type> polygon_type;
    typedef bg::model::multi_polygon<polygon_type> mpolygon_type;

    mpolygon_type mpoly;
    bg::read_wkt(geom, mpoly);

    point_type p;
    bg::centroid(mpoly, p);

    // std::cout << "centroid: " << bg::dsv(p) << std::endl;
    return make_tuple(p.x(), p.y());
}

std::tuple<double, double> ST_Centroid_bd(Json multipolygon) {
    typedef bg::model::d2::point_xy<double> point_type;
    typedef bg::model::polygon<point_type> polygon_type;
    typedef bg::model::multi_polygon<polygon_type> mpolygon_type;

    mpolygon_type mpoly;

    // I implemented this like the following way (making wkt)
    //      since building boost mpoly may has complicate rules.
    stringstream ss;
    ss << fixed;
    ss << "MULTIPOLYGON(";
    for (auto polygon : multipolygon) {
        ss << "(";
        for (auto ring : polygon) {
            ss << "(";
            for (auto point : ring) {
                ss << setprecision(7) << point[0].get<double>() << " " << setprecision(7) << point[1].get<double>() << ",";
            }
            ss.seekp(-1, ss.cur);
            ss << ")";
        }
        ss << ")";
    }
    ss << ")";
    // std::cout << ss.str()  << std::endl;
    bg::read_wkt(ss.str(), mpoly);

    point_type p;
    bg::centroid(mpoly, p);

    // std::cout << "centroid: " << bg::dsv(p) << std::endl;
    return make_tuple(p.x(), p.y());
}


std::tuple<double, double> STcentroid(Json multipolygon){
    double lon = 0.0;
    double lat = 0.0;
    int nrow = 0;

    for ( auto val1 : multipolygon){
        for (auto val2 : val1){
            for( auto val3 : val2){
                lon += val3[0].get<double>();
                lat += val3[1].get<double>();
                nrow++;
            }
        }
    }
    return std::make_tuple(lon/nrow,lat/nrow);
}


/*
 * To find the closest centroid from the coordinates, we need to fetch the data from MongoDB.
 */
int ST_ClosestObject_Map_building_centroid(mongocxx::collection mapCentroidCollection, double lat, double lon) {
    auto nnQBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value nnQ = nnQBuilder
            << "properties.type" << "building"
            << "centroid" << bsoncxx::builder::stream::open_document
            << "$near" << bsoncxx::builder::stream::open_document
            << "$geometry" << bsoncxx::builder::stream::open_document
            << "type" << "Point"
            << "coordinates" << bsoncxx::builder::stream::open_array
            << lon << lat
            << bsoncxx::builder::stream::close_array
            << bsoncxx::builder::stream::close_document
            // << "$maxDistance" << 1000                               // Magic Number
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;
    auto doc = mapCentroidCollection.find_one(nnQ.view(), mongocxx::options::find{}.limit(1));
    return doc->view()["site_id"].get_int32();
}


bsoncxx::stdx::optional<bsoncxx::document::value> ST_ClosestObject_Map_building(mongocxx::collection mapCollection, double lat, double lon) {
    auto nnQBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value nnQ = nnQBuilder
            << "properties.type" << "building"
            << "geometry" << bsoncxx::builder::stream::open_document
            << "$near" << bsoncxx::builder::stream::open_document
            << "type" << "Point"
            << "coordinates" << bsoncxx::builder::stream::open_array
            << lon << lat
            << bsoncxx::builder::stream::close_array
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;
    cout << bsoncxx::to_json(nnQ) << endl;
    auto docs = mapCollection.find_one(nnQ.view(), mongocxx::options::find{}.limit(1));
    return docs;
}

int ST_ClosestObject_RoadNode(mongocxx::collection mapCollection, shared_ptr<neo4j_connector> neo4j, double candidate_lat, double candidate_lon) {
    // find closest site document
    auto nnQBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value nnQ = nnQBuilder
            << "properties.type" << "roadnode"
            << "geometry" << bsoncxx::builder::stream::open_document
            << "$near" << bsoncxx::builder::stream::open_document
            << "type" << "Point"
            << "coordinates" << bsoncxx::builder::stream::open_array
            << candidate_lon << candidate_lat
            << bsoncxx::builder::stream::close_array
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;
    // cout << bsoncxx::to_json(nnQ) << endl;
    auto docs = mapCollection.find_one(nnQ.view(), mongocxx::options::find{}.limit(1));

    // find roadnode_id
    std::string pmatch = "(n:Roadnode {site_id: " + to_string(docs->view()["site_id"].get_int32().value) + "})";
    std::string preturn = "n.roadnode_id";
    std::string query_str = ("MATCH " + pmatch + " RETURN " + preturn );

    auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);

    neo4j_result_t *result = neo4j_fetch_next(results);
    if (result == NULL) assert("no result!!");

    return neo4j_int_value(neo4j_result_field(result, 0));
}


void create_Site_centroid_collection() {

    auto mongodb = mongodb_connector("Disaster");
    auto Site = mongodb.db["Site"];
    auto Site_centroid = mongodb.db["Site_centroid"];
    std::vector<bsoncxx::document::value> buffer;

    mongocxx::options::find option = mongocxx::options::find{}.no_cursor_timeout(true);
    auto docs0 = Site.find({});

    for (auto doc : docs0) {
        double cLat, cLon;
        auto geoJson = Json::parse(bsoncxx::to_json(doc["geometry"].get_document()));
        if (geoJson["type"] == "MultiPolygon") {
            auto centroid = ST_Centroid_bd(geoJson["coordinates"]);
            cLon = get<0>(centroid), cLat = get<1>(centroid);
        } else {
            cLon = geoJson["coordinates"][0], cLat = geoJson["coordinates"][1];
        }

        auto builder = bsoncxx::builder::stream::document{};
        if(doc["site_id"].get_int32() < 9646403) builder << "type" << doc["type"].get_utf8(); // site_id >9646403: roadnodes
        builder << "properties" << doc["properties"].get_document();
        builder << "site_id" << doc["site_id"].get_int32();
        auto val = builder << "centroid" << bsoncxx::builder::stream::open_document
                           << "type" << "Point"
                           << "coordinates" << bsoncxx::builder::stream::open_array
                           << cLon << cLat << bsoncxx::builder::stream::close_array
                           << bsoncxx::builder::stream::close_document
                           << bsoncxx::builder::stream::finalize;

        buffer.push_back(val);

        if (buffer.size() >= 1000) {
            Site_centroid.insert_many(buffer);
            buffer.clear();
        }
    }
    if (!buffer.empty()) {
        Site_centroid.insert_many(buffer);
        buffer.clear();
    }
}

/**
 * [Task 10] Road Network Filtering ([R, D, G]=> G)
 * For the earthquakes which occurred between time Z1 and Z2, find the road network subgraph within 5km from the earthquakes' location.
 *
 * A = SELECT n1, r, n2 AS subgraph FROM Earthquake, Site, RoadNode
 *     WHERE (n1:RoadNode) - [r:Road] -> (n2:RoadNode) AND ST_Distance(Site.geometry, Earthquake.coordinates) <= 5km
 *     AND Earthquake.time >= Z1 AND Earthquake.time < Z2 AND RoadNode.site_id = Site.site_id //Graph
*/
void T10(string Z1, string Z2){

    Z1 = "\'" + Z1 + "\'";
    Z2 = "\'" + Z2 + "\'";

    auto mysql = mysql_connector();
    auto Earthquake = mysql.mysess->getSchema("Disaster").getTable("Earthquake");
    auto mongodb = mongodb_connector("Disaster");
    auto Site = mongodb.db["Site"];
    auto neo4j = new neo4j_connector();

    std::string where = "time >= STR_TO_DATE(" + Z1 +", '%Y-%m-%d %H:%i:%s') AND time < STR_TO_DATE(" + Z2 + ", '%Y-%m-%d %H:%i:%s')";
    auto matched_earthquakes = Earthquake.select("earthquake_id","ST_X(coordinates)", "ST_Y(coordinates)").where(where).execute();

    int res_count=0;
    for ( auto eqk: matched_earthquakes) {
        int eqk_id = eqk[0];
        double eqk_lat = eqk[1].get<double>(), eqk_lon = eqk[2].get<double>();
        auto roadnode_sites = Site.find(
                make_document(kvp("geometry",
                                  make_document(kvp("$nearSphere",make_document(kvp(
                                          "$geometry", make_document(kvp("type","Point"), kvp("coordinates", make_array(eqk_lon,eqk_lat)))),
                                                                                kvp("$minDistance" , 0), kvp("$maxDistance", 5000))
                                  ))),
                              kvp("properties.type", "roadnode")));
        int count = 0 ;

        for (auto vertex: roadnode_sites) {
            count ++;
            auto json = Json::parse(bsoncxx::to_json(vertex));
            auto sid = to_string(json["site_id"]);

            auto t1 = high_resolution_clock::now();
            std::string graph_query =
                    "MATCH (n:Roadnode)-[r:Road]->(m:Roadnode)"
                    "WHERE n.site_id = " + sid
                    + " RETURN n, r, m";
            auto results = neo4j_run(neo4j->conn, graph_query.c_str(), neo4j_null);
            while (1) {
                neo4j_result_t *result = neo4j_fetch_next(results);
                if (result == NULL) break;

                int pos = 0;
                while (1) {
                    auto n = neo4j_result_field(result, pos);
                    if (!neo4j_is_null(n)) {}
                    else break;
                    pos++;
                }
                res_count++;
            }
            auto t2 = high_resolution_clock::now();
//            cout <<  graph_query<< endl;
//            cout <<  duration_cast<microseconds>(t2 - t1).count() << endl;

        }
        cout << count << endl;
    }
    cout << "TASK10 TOTAL " << res_count << " ROWS ARE REPORTED" << endl;
}


/**
  *  [Task 11] Closest Shelter ([R, D, G]=> R)
  *  For a given earthquake information X, find the cost of the shortest path for each GPS coordinate and Shelter pair.
  *  (GPS coordinates are limited by 1 hour and 10km from the X. Shelters are limited by 15km from the X.)
  *
  *  A = SELECT GPS.gps_id, ST_ClosestObject(Site, roadnode, GPS.coordinates) AS roadnode_id FROM GPS, Site, RoadNode
  *      WHERE GPS.time >= X.time AND GPS.time < X.time + 1 hour AND ST_Distance(GPS.coordinates, X.coordinates) <= 10km
  *      AND RoadNode.site_id = Site.site_id //Relational
  *
  *  B = SELECT t.shelter_id, ST_ClosestObject(Site, roadnode, ST_Centroid(t.geometry)) AS roadnode_id
  *      FROM RoadNode, Site, (SELECT Shelter.shelter_id, Site.geometry FROM Site, Shelter WHERE ST_Distance(ST_Centroid(Site.geometry), X.coordinates) <= 15km AND Shelter.site_id = Site.site_id) AS t
  *      WHERE RoadNode.site_id = Site.site_id  //Relational
  *
  *  C = SELECT A.gps_id, B.shelter_id, ShortestPath(RoadNode, startNode:A.roadnode_id, endNode:B.roadnode_id) AS cost
  *      FROM A, B, RoadNode //Relational
  */
void T11(int X_id) {
    auto mysql = mysql_connector();
    auto neo4j = neo4j_connector();
    auto mongodb = mongodb_connector("Disaster");
    auto Site = mongodb.db["Site"];
    auto Site_centroid = mongodb.db["Site_centroid"];

    int buffer = 0;
    mysql.mysess->sql("use Disaster").execute();
    mysql.mysess->sql("CREATE TEMPORARY TABLE Gps_list(gps_id INT, gps_roadnode_id INT)").execute();
    mysql.mysess->sql("CREATE TEMPORARY TABLE Shelter_list(shelter_id INT, shelter_roadnode_id INT)").execute();

    auto Earthquake = mysql.mysess->getSchema("Disaster").getTable("Earthquake");
    mysql.mysess->sql("CREATE TEMPORARY TABLE IF NOT EXISTS eqk_X AS (SELECT time, coordinates FROM Earthquake WHERE earthquake_id=" + to_string(X_id) +")").execute();

    /* step A: find gps */
    std::string select = "Gps.gps_id, ST_Y(Gps.coordinates), ST_X(Gps.coordinates) ";
    std::string from = "Gps, eqk_X ";
    std::string where = "Gps.time >= eqk_X.time AND Gps.time < date_add(eqk_X.time, interval 1 hour)"
                        " AND ST_Distance_Sphere("
                        "ST_GeomFromText(CONCAT('POINT (', ST_X(Gps.coordinates), ' ', ST_Y(Gps.coordinates), ')'), 4326), "
                        "ST_GeomFromText(CONCAT('POINT (', ST_X(eqk_X.coordinates), ' ', ST_Y(eqk_X.coordinates), ')'), 4326)"
                        ") <= 10000";
    auto filtered_gps = mysql.mysess->sql("SELECT " + select + "FROM " + from + "WHERE " + where).execute();

    auto insert2Gps_list = mysql.mysess->getSchema("Disaster").getTable("Gps_list").insert("gps_id", "gps_roadnode_id");
    for (auto gps: filtered_gps) {
        int gps_id = gps[0].get<int>();
        double gps_lon = gps[1].get<double>(), gps_lat = gps[2].get<double>();

        mongocxx::pipeline p{};
        p.geo_near(make_document(
                kvp("near", make_document(kvp("type", "Point"), kvp("coordinates", make_array(gps_lon, gps_lat)))),
                kvp("key", "geometry"),
                kvp("distanceField", "dist.calculated"),
                kvp("query", make_document(kvp("properties.type", "roadnode"))),
                kvp("spherical", "true")
                   )
        );
        p.limit(1);
        auto gps_node = Site.aggregate(p, mongocxx::options::aggregate{});
        int gps_roadnode_id;
        for (auto cursor: gps_node) {
            auto json = Json::parse(bsoncxx::to_json(cursor));
            gps_roadnode_id = json["site_id"];

            buffer++;
            insert2Gps_list.values(gps_id, gps_roadnode_id);
            if(buffer >= BUFFER) {
                insert2Gps_list.execute();
                insert2Gps_list = mysql.mysess->getSchema("Disaster").getTable("Gps_list").insert("gps_id", "gps_roadnode_id");
                buffer = 0;
            }
        }
    }
    if(buffer != 0) insert2Gps_list.execute();
    buffer =0;

    /* step B: find shelters */
    //create TEMP TABLE Shelter_temp (shelter with geometry)
    mysql.mysess->sql("CREATE TEMPORARY TABLE Shelter_temp(shelter_id INT, site_id INT, lon double, lat double)").execute();
    auto insert2Shelter_temp = mysql.mysess->getSchema("Disaster").getTable("Shelter_temp").insert("shelter_id", "site_id", "lon", "lat");
    auto shelter = mysql.mysess->sql("SELECT shelter_id, site_id FROM Shelter").execute();
    for (auto row: shelter) {
        int shelter_id = row[0].get<int>(), site_id = row[1].get<int>();
        auto shelter_geom = Site_centroid.find_one(make_document(kvp("site_id", site_id)));
        auto json = Json::parse(bsoncxx::to_json(*shelter_geom));
        auto centroid = json["centroid"]["coordinates"];

        //insert to Shelter_temp
        insert2Shelter_temp.values(shelter_id, site_id, centroid[0].get<double>(), centroid[1].get<double>());
        buffer++;
        if (buffer >= BUFFER) {
            insert2Shelter_temp.execute();
            insert2Shelter_temp = mysql.mysess->getSchema("Disaster").getTable("Shelter_temp").insert("shelter_id", "site_id", "lon", "lat");
            buffer = 0;
        }
    }
    if (buffer != 0) insert2Shelter_temp.execute();
    buffer = 0;
    mysql.mysess->sql("CREATE INDEX shelter_id_idx ON Shelter_temp(shelter_id)").execute();
    mysql.mysess->sql("CREATE INDEX lon_idx ON Shelter_temp(lon)").execute();
    mysql.mysess->sql("CREATE INDEX lat_idx ON Shelter_temp(lat)").execute();

    select = " Shelter_temp.shelter_id, Shelter_temp.lon, Shelter_temp.lat ";
    from = " eqk_X, Shelter_temp ";
    where = " ST_Distance_Sphere("
            "ST_GeomFromText(CONCAT('POINT (', Shelter_temp.lat, ' ', Shelter_temp.lon, ')'), 4326), "
            "ST_GeomFromText(CONCAT('POINT (', ST_X(eqk_X.coordinates), ' ', ST_Y(eqk_X.coordinates), ')'), 4326)"
            ") <= 15000";
    auto filtered_shelter = mysql.mysess->sql("SELECT" + select + "FROM" + from + "WHERE" + where).execute();

    auto insert2Shelter_list = mysql.mysess->getSchema("Disaster").getTable("Shelter_list").insert("shelter_id", "shelter_roadnode_id");
    for(auto shelter: filtered_shelter){
        int shelter_id = shelter[0].get<int>();
        double shelter_lon = shelter[1].get<double>(), shelter_lat = shelter[2].get<double>();

        mongocxx::pipeline p{};
        p.geo_near(make_document(
                kvp("near", make_document(kvp("type", "Point"), kvp("coordinates", make_array(shelter_lon, shelter_lat)))),
                kvp("key", "geometry"),
                kvp("distanceField", "dist.calculated"),
                kvp("query", make_document(kvp("properties.type", "roadnode"))),
                kvp("spherical", "true")
                   )
        );
        p.limit(1);
        auto shelter_node = Site.aggregate(p, mongocxx::options::aggregate{});
        int shelter_roadnode_id;
        for (auto cursor: shelter_node) {
            auto json = Json::parse(bsoncxx::to_json(cursor));
            shelter_roadnode_id = json["site_id"];

            insert2Shelter_list.values(shelter_id, shelter_roadnode_id);
            buffer++;
            if (buffer >= BUFFER) {
                insert2Shelter_list.execute();
                insert2Shelter_list = mysql.mysess->getSchema("Disaster").getTable("Shelter_list").insert("shelter_id", "shelter_roadnode_id");
                buffer = 0;
            }
        }
    }
    if(buffer != 0) insert2Shelter_list.execute();

    /* step C: find shortest path */
    int res_count = 0;
    auto gps_list = mysql.mysess->sql("SELECT * FROM Gps_list").execute();
    for(auto row1: gps_list){
        int gps_id = row1[0].get<int>();
        int gps_roadnode_id = row1[1].get<int>();

        auto shelter_list = mysql.mysess->sql("SELECT * FROM Shelter_list").execute();
        for(auto row2: shelter_list) {
            int shelter_id = row2[0].get<int>();
            int shelter_roadnode_id = row2[1].get<int>();

            std::string pmatch =
                    "(gps:Roadnode {site_id:" + to_string(gps_roadnode_id) + "}), (shelter:Roadnode {site_id:" + to_string(shelter_roadnode_id) + "})";
            std::string pcall = "gds.shortestPath.dijkstra.stream('road_network', { sourceNode: gps, targetNode: shelter, relationshipWeightProperty: 'distance'})";
            std::string pyield = "sourceNode, targetNode, totalCost";
            std::string preturn = "totalCost";

            std::string graph_query = ("MATCH " + pmatch + " CALL " + pcall + " YIELD " + pyield + "  RETURN " +  preturn);
            auto graph_results = neo4j_run(neo4j.conn, graph_query.c_str(), neo4j_null);
            neo4j_result_t *graph_result = neo4j_fetch_next(graph_results);
            if (graph_result == NULL) continue;
            auto cost = neo4j_float_value(neo4j_result_field(graph_result, 0));
//            cout << gps_id << "   " << shelter_id << ", Distance: "<< cost << endl;
            res_count++;
        }
    }
    cout << "TASK11 TOTAL " << res_count << " ROWS ARE REPORTED" << endl;
}


/**
 * [Task 12] New Shelter ([R, D, G]=> R)
 * For the shelter of which the number of GPS coordinates are the most within 5km from the shelter between time Z1 and Z2,
 * find five closest buildings from the shelter. The buildings are limited by 1km from the shelter.
 *
 * A = SELECT Shelter.shelter_id, Site.geometry
 *     FROM Shelter, Site, (SELECT Shelter.shelter_id, COUNT(GPS.gps_id) AS cnt FROM Shelter, GPS, Site
 *                          WHERE ST_Distance(GPS.coordinates, ST_Centroid(Site.geometry)) <= 5km AND Site.site_id = Shelter.site_id
 *                          AND GPS.time >= Z1 AND GPS.time < Z2 GROUP BY Shelter.id ORDER BY cnt DESC LIMIT 1) AS t
 *     WHERE Shelter.shelter_id = t.shelter_id AND Site.site_id = Shelter.site_id  //Relational
 *
 * B = SELECT A.shelter_id, ST_ClosestObject(Site, roadnode, ST_centroid(A.geometry)) AS roadnode_id
 *     FROM A, RoadNode, Site
 *     WHERE Site.site_id = RoadNode.site_id  //Relational
 *
 * C = SELECT t.site_id, ST_ClosestObject(Site, roadnode, ST_Centroid(t.geometry)) AS roadnode_id
 *     FROM Site, RoadNode, (SELECT Site.site_id, Site.geometry FROM Site, A
 *                           WHERE ST_Distance(Site.geometry, ST_centroid(A.geometry)) <= 1km AND Site.properties.type = 'building') AS t
 *     WHERE RoadNode.site_id = Site.site_id  //Relational
 *
 * D = SELECT C.site_id, ShortestPath(RoadNode, startNode:B.roadnode_id, endNode:C.roadnode_id) AS cost
 *     FROM B, C, RoadNode
 *     ORDER BY cost LIMIT 5  //Relational
 */
void T12(string Z1, string Z2) {

    auto mysql = mysql_connector();
    auto neo4j = neo4j_connector();
    auto mongodb = mongodb_connector("Disaster");
    auto Site = mongodb.db["Site"];
    auto Site_centroid = mongodb.db["Site_centroid"];

    int buffer = 0;
    mysql.mysess->sql("use Disaster").execute();
    mysql.mysess->sql("CREATE TEMPORARY TABLE Shelter_temp(shelter_id INT, site_id INT, lon double, lat double)").execute();
    auto insert2Shelter_temp = mysql.mysess->getSchema("Disaster").getTable("Shelter_temp").insert("shelter_id", "site_id", "lon", "lat");
    auto shelter = mysql.mysess->sql("SELECT shelter_id, site_id FROM Shelter").execute();
    for (auto row: shelter) {
        int shelter_id = row[0].get<int>(), site_id = row[1].get<int>();
        auto shelter_geom = Site_centroid.find_one(make_document(kvp("site_id", site_id), kvp("properties.type", "building"), kvp("properties.description", "hospital")));
        if(shelter_geom){
            auto json = Json::parse(bsoncxx::to_json(*shelter_geom));
            auto centroid = json["centroid"]["coordinates"];

            //insert to temp table
            insert2Shelter_temp.values(shelter_id, site_id, centroid[0].get<double>(), centroid[1].get<double>());
            buffer++;
            if (buffer >= BUFFER) {
                insert2Shelter_temp.execute();
                insert2Shelter_temp = mysql.mysess->getSchema("Disaster").getTable("Shelter_temp").insert("shelter_id", "site_id", "lon", "lat");
                buffer = 0;
            }
        }
    }
    if (buffer != 0) insert2Shelter_temp.execute();
    buffer = 0;
    mysql.mysess->sql("CREATE INDEX shelter_id_idx ON Shelter_temp(shelter_id)").execute();
    mysql.mysess->sql("CREATE INDEX lon_idx ON Shelter_temp(lon)").execute();
    mysql.mysess->sql("CREATE INDEX lat_idx ON Shelter_temp(lat)").execute();

    std::string select = " Shelter_temp.shelter_id AS shelter_id, Shelter_temp.lon AS shelter_lon, Shelter_temp.lat AS shelter_lat, COUNT(Gps.gps_id) AS numGps ";
    std::string from = " Gps, Shelter_temp ";
    Z1 = "\'" + Z1 + "\'";
    Z2 = "\'" + Z2 + "\'";
    std::string where = " Gps.time >= STR_TO_DATE(" + Z1 +", '%Y-%m-%d %H:%i:%s') AND Gps.time < STR_TO_DATE(" + Z2 + ", '%Y-%m-%d %H:%i:%s') AND ST_Distance_Sphere("
                                                                                                                      "ST_GeomFromText(CONCAT('POINT (', Shelter_temp.lat, ' ', Shelter_temp.lon, ')'), 4326), "
                                                                                                                      "ST_GeomFromText(CONCAT('POINT (', ST_X(coordinates), ' ', ST_Y(coordinates), ')'), 4326)"
                                                                                                                      ") <= 5000";
    std::string others = " GROUP BY Shelter_temp.shelter_id, Shelter_temp.lon, Shelter_temp.lat ORDER BY numGps DESC LIMIT 1";
    auto target_shelter = mysql.mysess->sql("SELECT" + select + "FROM" + from + "WHERE" + where + others).execute().fetchOne();
    int target_shelter_id = target_shelter[0].get<int>();
    double target_shelter_lon = target_shelter[1].get<double>(), target_shelter_lat = target_shelter[2].get<double>();

    mongocxx::pipeline p{};
    p.geo_near(make_document(
            kvp("near", make_document(kvp("type", "Point"), kvp("coordinates", make_array(target_shelter_lon, target_shelter_lat)))),
            kvp("key", "geometry"),
            kvp("distanceField", "dist.calculated"),
            kvp("query", make_document(kvp("properties.type", "roadnode"))),
            kvp("spherical", "true")
               )
    );
    p.limit(1);
    auto shelter_node = Site.aggregate(p, mongocxx::options::aggregate{});
    int shelter_roadnode_id;
    for (auto cursor: shelter_node) {
        auto json = Json::parse(bsoncxx::to_json(cursor));
        shelter_roadnode_id = json["site_id"];
    }

    /* Next, find buildings. */
    mysql.mysess->sql("CREATE TEMPORARY TABLE Building_list(building_id INT, building_roadnode_id INT)").execute();
    auto insert2Building_list = mysql.mysess->getSchema("Disaster").getTable("Building_list").insert("building_id", "building_roadnode_id");
    auto buildings = Site_centroid.find(
            make_document(kvp("centroid",
                              make_document(kvp("$nearSphere",make_document(kvp(
                                      "$geometry", make_document(kvp("type","Point"), kvp("coordinates", make_array(target_shelter_lon, target_shelter_lat)))),
                                                                            kvp("$minDistance" , 0), kvp("$maxDistance", 1000))
                              ))),
                          kvp("properties.type", "building"), kvp("properties.description", "school")));

    for (auto cursor: buildings) {
        auto json = Json::parse(bsoncxx::to_json(cursor));
        int building_id = json["site_id"];
        auto centroid = json["centroid"]["coordinates"];

        //For each building, find closest roadnode.
        mongocxx::pipeline p{};
        p.geo_near(make_document(
                kvp("near", make_document(kvp("type", "Point"), kvp("coordinates", make_array(centroid[0].get<double>(), centroid[1].get<double>())))),
                kvp("key", "geometry"),
                kvp("distanceField", "dist.calculated"),
                kvp("query", make_document(kvp("properties.type", "roadnode"))),
                kvp("spherical", "true")
                   )
        );
        p.limit(1);

        auto building_node = Site.aggregate(p, mongocxx::options::aggregate{});
        for (auto b: building_node) {
            auto b_json = Json::parse(bsoncxx::to_json(b));
            int building_roadnode_id = b_json["site_id"];

            insert2Building_list.values(building_id, building_roadnode_id);
            buffer++;
            if (buffer >= BUFFER) {
                insert2Building_list.execute();
                insert2Building_list = mysql.mysess->getSchema("Disaster").getTable("Building_list").insert("building_id", "building_roadnode_id");
                buffer = 0;
            }
        }
    }
    if (buffer != 0) insert2Building_list.execute();
    buffer = 0;

    /*calculate shortest path and find top 5 new buildings for shelter. */
    int res_count=0;
    mysql.mysess->sql("CREATE TEMPORARY TABLE Shortest_path(building_id INT, distance FLOAT)").execute();
    auto insert2Shortest_path = mysql.mysess->getSchema("Disaster").getTable("Shortest_path").insert("building_id", "distance");

    auto building_list = mysql.mysess->getSchema("Disaster").getTable("Building_list").select("*").execute();
    for (auto row : building_list) {
        int building_id = row[0].get<int>();
        int roadnode_id = row[1].get<int>();

        std::string pmatch =
                "(shelter:Roadnode {site_id:" + to_string(shelter_roadnode_id) + "}), (building:Roadnode {site_id:" + to_string(roadnode_id) + "})";
        std::string pcall = "gds.shortestPath.dijkstra.stream('road_network', { sourceNode: shelter, targetNode: building, relationshipWeightProperty: 'distance'})";
        std::string pyield = "sourceNode, targetNode, totalCost";
        std::string preturn = "totalCost";
        std::string graph_query = ("MATCH " + pmatch + " CALL " + pcall + " YIELD " + pyield + "  RETURN " +  preturn);
        auto graph_results = neo4j_run(neo4j.conn, graph_query.c_str(), neo4j_null);
        neo4j_result_t *graph_result = neo4j_fetch_next(graph_results);
        if (graph_result == NULL) continue;
        auto cost = neo4j_float_value(neo4j_result_field(graph_result, 0));

        insert2Shortest_path.values(building_id, cost);
        buffer++;
        if (buffer >= BUFFER) {
            insert2Shortest_path.execute();
            insert2Shortest_path = mysql.mysess->getSchema("Disaster").getTable("Shortest_path").insert("building_id", "distance");
            buffer = 0;
        }
    }
    if (buffer != 0) insert2Shortest_path.execute();

    auto top5 = mysql.mysess->getSchema("Disaster").getTable("Shortest_path").select("building_id").orderBy("distance", "building_id").limit(5).execute();
    for (auto row : top5) {
        res_count++;
//        cout<< "building_id: "<< row[0].get<int>()<<endl;
    }
    cout << "TASK12 TOTAL " << res_count << " ROWS ARE REPORTED" << endl;
}

void deprecated_T12(string ts1, string ts2){

    auto mysql = mysql_connector();
    auto mongodb = mongodb_connector("Disaster");
    auto map = mongodb.db["map"];

    mysql.mysess->sql("use Disaster").execute();
    string query = " SELECT Shelter.id, ST_X(Shelter.coordinates), ST_Y(Shelter.coordinates), COUNT(Gps.id) AS cnt "
                   " FROM Shelter, Gps "
                   " WHERE MBRContains(ST_BUFFER(Gps.coordinates, 0.07, ST_Buffer_Strategy('point_square')), Shelter.coordinates)"
                   " AND ST_Distance(ST_GeomFromText(ST_AsText(Shelter.coordinates),4236), ST_GeomFromText(ST_AsText(Gps.coordinates),4236), 'metre')/1000 <= 1 "
                   " AND Gps.time >= '"+ts1+"' and Gps.time <= '"+ts2+"' "
                                                                      " GROUP BY Shelter.id, Shelter.coordinates ORDER BY cnt DESC LIMIT 1";


    cout << query << endl;

    auto rows = mysql.mysess->sql(query).execute();

    int shelter_id = 0;
    double shelter_lat = 0;
    double shelter_lon = 0;
    for(auto row : rows){
        shelter_id = row[0].get<int>();
        shelter_lat = row[1].get<double>();
        shelter_lon = row[2].get<double>();
    }

    cout << shelter_id << " " << shelter_lat << " " << shelter_lon ;
//    int shelter_id = 6533;
//    double shelter_lat = 34.060447;
//    double shelter_lon = -118.277207;

//    shared_ptr<neo4j_connector> neo4j(new neo4j_connector());
//
//    std::string pmatch = "(n:Roadnode)";
//    std::string preturn = "n.roadnode_id, n.latitude, n.longitude";
//
//    std::string query_str = ("MATCH " + pmatch + " RETURN " + preturn );
//    cout << query_str << endl;
//    auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
//
//    double min_distance = 100;
//    int shelter_node_id = 0;
//    int shelter_node_lat = 0;
//    int shelter_node_lon = 0;
//
//    while (1) {
//        neo4j_result_t *result = neo4j_fetch_next(results);
//        if (result == NULL) break;
//
//        auto node_id =  neo4j_int_value(neo4j_result_field(result, 0));
//        auto node_lat = neo4j_float_value(neo4j_result_field(result, 1));
//        auto node_lon = neo4j_float_value(neo4j_result_field(result, 2));
//
//        double dist = distance(shelter_lat,shelter_lon,node_lat,node_lon);
//        if( dist <= min_distance){
//            min_distance = dist;
//            shelter_node_id = node_id;
//            shelter_node_lat = node_lat;
//            shelter_node_lon = node_lon;
//        }
//    }
//
//
//    auto buildings = map.find(make_document(kvp("geometry", make_document(kvp("$near",
//                                                                              make_document(kvp("$geometry", make_document(kvp("type","Point"),
//                                                                                                                           kvp("coordinates", make_array(shelter_lon,shelter_lat)))),
//                                                                                            kvp("$minDistance" , 0),
//                                                                                            kvp("$maxDistance", 200))
//                                            ))),
//                                            kvp("properties.building", make_document(kvp("$exists", true)))));
//
//    int nrow = 0;
//    for (auto building : buildings){
//
//        auto json = Json::parse(bsoncxx::to_json(building));
//        auto centroid = STcentroid(json["geometry"]["coordinates"]);
//
//
//        double candidate_lon = get<0>(centroid);
//        double candidate_lat = get<1>(centroid);
//
//        int candidate_node_id = 0;
//        double candidate_node_lat = 0;
//        double candidate_node_lon = 0;
//
//        auto crnt = ST_ClosestObject_RoadNode(neo4j, candidate_lat, candidate_lon);
//        candidate_node_id = get<0>(crnt); candidate_node_lat = get<1>(crnt); candidate_node_lon = get<2>(crnt);
//
////        cout << candidate_node_id << "\t" << candidate_node_lat <<"\t" << candidate_node_lon <<"\t" << min_distance << endl;
//
//        std::string pmatch2 = "(shelter:Roadnode {roadnode_id:"+to_string(shelter_node_id)+"}), (candidate {roadnode_id: "+to_string(candidate_node_id)+"})";
//        std::string pcall2 = "gds.shortestPath.dijkstra.stream('road_network', { sourceNode:shelter , targetNode: candidate, relationshipWeightProperty: 'distance'})";
//        std::string pyield2 = "sourceNode, targetNode, totalCost";
//        std::string preturn2 = "gds.util.asNode(sourceNode).roadnode_id, gds.util.asNode(targetNode).roadnode_id, totalCost";
//
//        std::string query_str2 = ("MATCH " + pmatch2 + " CALL " + pcall2 + " YIELD " + pyield2 + "  RETURN " + preturn2 );
////        cout <<query_str2<< endl;
//
//        auto results2 = neo4j_run(neo4j->conn, query_str2.c_str(), neo4j_null);
//
//        while (1) {
//            neo4j_result_t *result = neo4j_fetch_next(results2);
//            if (result == NULL) break;
//
//            auto shelter_node_id_res =  neo4j_int_value(neo4j_result_field(result, 0));
//            auto candidate_node_id_res = neo4j_int_value(neo4j_result_field(result, 1));
//            auto cost = neo4j_float_value(neo4j_result_field(result, 2));
//
//            cout << "NodeId: " << shelter_node_id_res << "\tNodeId: " << candidate_node_id_res << "\tDistance: " << cost << endl;
//        }
//        nrow++;
//    }
//    cout << "[TASK12]: TOTAL " << nrow << " ROWS ARE REPORTED" << endl;

}

mongocxx::collection constructCentroidCollection(mongocxx::database mdb, mongocxx::collection mapCollection, string name, bool wSiteid) {
    auto mapCentroidCollection = mdb[name];       // for saving temp centroid data

    // Optimization
    std::vector<bsoncxx::document::value> buffer;

    // Site -> Site_controid: conversion multipolygon to point
    mongocxx::options::find option = mongocxx::options::find{}.no_cursor_timeout(true);         // If not set, cursor is broken while the query running.
    auto docs0 = mapCollection.find(bsoncxx::builder::stream::document{}
                                            << "properties.type" << "building"
                                            << bsoncxx::builder::stream::finalize, option);

    for (auto doc : docs0) {
        // Calculating centroid
        double cLat, cLon;
        // Note that the Site only has documents which geometry type are MultiPolygon or Point.
        auto geoJson = Json::parse(bsoncxx::to_json(doc["geometry"].get_document()));
        if (geoJson["type"] == "MultiPolygon") {
            auto centroid = ST_Centroid_bd(geoJson["coordinates"]);
            cLon = get<0>(centroid), cLat = get<1>(centroid);
        } else {
            cLon = geoJson["coordinates"][0], cLat = geoJson["coordinates"][1];
        }

        auto builder = bsoncxx::builder::stream::document{};
        builder << "desc" << doc["properties"].get_document().value["description"].get_utf8();
        if (wSiteid) builder << "site_id" << doc["site_id"].get_int32();
        auto val = builder << "centroid" << bsoncxx::builder::stream::open_document
                           << "type" << "Point"
                           << "coordinates" << bsoncxx::builder::stream::open_array
                           << cLon << cLat << bsoncxx::builder::stream::close_array
                           << bsoncxx::builder::stream::close_document
                           << bsoncxx::builder::stream::finalize;
        buffer.push_back(val);

        if (buffer.size() >= 1000) {
            mapCentroidCollection.insert_many(buffer);
            buffer.clear();
        }

    }

    // clear buffer
    if (!buffer.empty()) {
        mapCentroidCollection.insert_many(buffer);
        buffer.clear();
    }

    // on-the-fly index
    auto index_specification = document{} << "centroid" << "2dsphere" << finalize;
    mapCentroidCollection.create_index(std::move(index_specification));

    return mapCentroidCollection;
}

void boost_centroid_test() {
    unique_ptr<mongodb_connector> mongodb(new mongodb_connector("Disaster"));
    auto mdb = mongodb->db;
    auto mapCollection = mdb["Site"];

    // Site -> Site_controid: conversion multipolygon to point
    mongocxx::options::find option = mongocxx::options::find{}.no_cursor_timeout(true);         // If not set, cursor is broken while the query running.
    auto docs0 = mapCollection.find(bsoncxx::builder::stream::document{}
                                            << "site_id" << bsoncxx::builder::stream::open_document
                                            << "$in" << bsoncxx::builder::stream::open_array
                                            << 8460056 << 8458588
                                            << bsoncxx::builder::stream::close_array
                                            << bsoncxx::builder::stream::close_document
                                            << bsoncxx::builder::stream::finalize, option);

    for (auto doc : docs0) {
        // Calculating centroid
        double cLat, cLon;
        // Note that the Site only has documents which geometry type are MultiPolygon or Point.
        auto geoJson = Json::parse(bsoncxx::to_json(doc["geometry"].get_document()));
        if (geoJson["type"] == "MultiPolygon") {
            auto centroid = ST_Centroid_bd(geoJson["coordinates"]);
            cLon = get<0>(centroid), cLat = get<1>(centroid);
        } else {
            cLon = geoJson["coordinates"][0], cLat = geoJson["coordinates"][1];
        }

        cout << bsoncxx::to_json(doc) << " , " << cLon << " , " << cLat << endl;
    }
}


/*
 * [Task 13] Damage Statistics.
 *
 * For the earthquakes of which magnitude is greater than 4.5, find the building statistics.
 * The buildings are limited by 30km from the earthquake location. (Relational, Document) -> Document
 */
void T13() {
    size_t nrows = 0;
    unique_ptr<mysql_connector> mysql(new mysql_connector());
    auto earthquakeTable = mysql->mysess->getSchema("Disaster").getTable("Earthquake");

    // MySQL default setting is not allowing case-sensative queries so I added COLLATE to the table.
    mysql->mysess->sql("use Disaster").execute();
    mysql->mysess->sql("CREATE TEMPORARY TABLE Site_group(description CHAR(255)) COLLATE 'utf8mb4_bin'").execute();

    auto siteGroupTable = mysql->mysess->getSchema("Disaster").getTable("Site_group");

    unique_ptr<mongodb_connector> mongodb(new mongodb_connector("Disaster"));
    auto mdb = mongodb->db;
    auto mapCentroidCollection = mdb["Site_centroid"];

    // Optimization
    auto insList = siteGroupTable.insert("description");
    auto bufSize = 0;

    // Fetching Earthquake with magnitude >= 4.5
    auto rows1 = earthquakeTable.select("ST_X(coordinates)", "ST_Y(coordinates)")
            .where("magnitude >= 4.5").execute();
    for (auto row : rows1) {
        double lat = row[0].get<double>(), lon = row[1].get<double>();
        auto docq1Builder = bsoncxx::builder::stream::document{};
        bsoncxx::document::value docq1 = docq1Builder
                << "properties.type" << "building"
                << "centroid" << bsoncxx::builder::stream::open_document
                << "$geoWithin" << bsoncxx::builder::stream::open_document
                << "$centerSphere" << bsoncxx::builder::stream::open_array
                << bsoncxx::builder::stream::open_array
                << lon << lat
                << bsoncxx::builder::stream::close_array
                << (double) 30 / 6378.1
                << bsoncxx::builder::stream::close_array
                << bsoncxx::builder::stream::close_document
                << bsoncxx::builder::stream::close_document
                << bsoncxx::builder::stream::finalize;
        // std::cout << bsoncxx::to_json(docq1) << std::endl;
        auto docs1 = mapCentroidCollection.find(docq1.view(),
                                                mongocxx::options::find{}.projection(bsoncxx::builder::stream::document{}
                                                                                     << "_id" << 0
                                                                                     << "properties.description" << 1
                                                                                     << bsoncxx::builder::stream::finalize));

        for (auto doc : docs1) {
            insList.values(doc["properties"].get_document().value["description"].get_utf8().value.to_string());
            bufSize++;

            if (bufSize >= BUFFER) {
                insList.execute();
                insList = siteGroupTable.insert("description");
                bufSize = 0;
            }
        }
    }

    // clear buffer
    if (bufSize > 0) {
        insList.execute();
        insList = siteGroupTable.insert("description");
        bufSize = 0;
    }

    // group by and projection
    auto rows2 = siteGroupTable.select("description", "COUNT(*)").groupBy("description").execute();
    for (auto row : rows2) {
        string desc = row[0].get<string>();
        int count = row[1].get<int>();

        // std::cout << desc << " " << count << endl;
        nrows++;
    }

    std::cout << "[TASK13]: TOTAL " << nrows << " ROWS ARE REPORTED" << endl;
}


/* 
 * [Task 14] Sources of Fine Dust.
 *
 * Analyze fine dust hotspot by date between time Z1 and Z2.
 * Print the nearest building with time of the hotspot.
 * Use window aggregation with a size of 5. (Document, Array) -> Document
 *
 */
void T14(int z1, int z2) {
    unique_ptr<ScidbConnection> scidb(new ScidbConnection(SCIDB_HOST_DISASTER + ":8080"));
    unique_ptr<mongodb_connector> mongodb(new mongodb_connector("Disaster"));
    auto mdb = mongodb->db;
    auto mapCentroidCollection = mdb["Site_centroid"];

    size_t nrows = 0;

    // Query A and B
    // 8 is magic number for dataset
    scidb->exec("store(redimension(apply(window(between(finedust, " + to_string(z1) + ", 0, 0, " + to_string(z2) + ", 522, 522), 0, 0, 2, 2, 2, 2, avg(pm10)), date, timestamp/8), <pm10_avg: double>[date=0:*:0:?; timestamp=0:*:0:?; latitude=0:*:0:?; longitude=0:*:0:?]), t14t1)");

    // Query C
    ScidbSchema t2Schema;
    t2Schema.dims.push_back(ScidbDim("$n", 0, INT32_MAX, 0, 1000000));
    t2Schema.attrs.push_back(ScidbAttr("pm10_avg_max", DOUBLE)); t2Schema.attrs.push_back(ScidbAttr("date", INT64));

    ScidbSchema maxSchema;
    maxSchema.dims.push_back(ScidbDim("i", 0, INT32_MAX, 0, 1000000));
    maxSchema.attrs.push_back(ScidbAttr("pm10_avg", DOUBLE));
    maxSchema.attrs.push_back(ScidbAttr("latitude", INT64));
    maxSchema.attrs.push_back(ScidbAttr("longitude", INT64));
    maxSchema.attrs.push_back(ScidbAttr("timestamp", INT64));

    auto t2arr = scidb->download("sort(redimension(aggregate(t14t1, max(pm10_avg), date), <pm10_avg_max: double, date: int64>[i=0:*:0:1000]), date)", t2Schema);
    auto t2arrVal = t2arr->readcell();
    while (t2arrVal.size() != 0) {
        // cout << get<double>(t2arrVal.at(1)) << " " << get<long long>(t2arrVal.at(2)) << endl;
        long long date = get<long long>(t2arrVal.at(2));
        double maxVal = get<double>(t2arrVal.at(1));

        // get location of value
        // If you have a better solution, please improve it.
        // Another way to do this is that replace the query for t2arr to cross_join (t14t1 and max aggregated one), but it is slow a little bit.
        auto maxArr = scidb->download("sort(redimension(filter(t14t1, abs(pm10_avg - " + to_string(maxVal) + ") < 1 and timestamp / 8 = " + to_string(date) + "), <pm10_avg:double, latitude:int64, longitude: int64, timestamp: int64>[i=0:*:0:1000]), pm10_avg)", maxSchema);
        auto maxArrVal = maxArr->readcell();
        if (maxArrVal.size() == 0) throw std::runtime_error("equality check for floating point is failed!");

        auto closestValue = ST_ClosestObject_Map_building_centroid(mapCentroidCollection,
            34.011898718557454 + (double) get<long long>(maxArrVal.at(2)) * 0.000172998,
            -118.34501002237936 + (double) get<long long>(maxArrVal.at(3)) * 0.000216636);
        // cout << date << " " << get<long long>(maxArrVal.at(4)) << " " << to_string(closestValue) << " " << endl;

        t2arrVal = t2arr->readcell();
        nrows++;
    }

    scidb->exec("remove(t14t1)");

    cout << "[TASK14]: TOTAL " << nrows << " ROWS ARE REPORTED" << endl;
}


/*
 * [Task15] Fine Dust Cleaning Vehicles.
 *
 * Recommend the route from the current coordinates by analyzing the hotspot between time Z1 and Z2.
 * Use window aggregation with a size of 5. (Graph, Document, Array) -> Relational
 *
 */
void T15(int z1, int z2, double lon, double lat) {
    size_t nrows = 0;

    unique_ptr<ScidbConnection> scidb(new ScidbConnection(SCIDB_HOST_DISASTER + ":8080"));
    shared_ptr<neo4j_connector> neo4j(new neo4j_connector());
    unique_ptr<mongodb_connector> mongodb(new mongodb_connector("Disaster"));
    auto mdb = mongodb->db;
    auto mapCollection = mdb["Site"];

    // Query A and B
    int max_win_oneside = z2 - z1;

    ScidbSchema hotspotSchema;
    hotspotSchema.dims.push_back(ScidbDim("i", 0, INT32_MAX, 0, 1000000));
    hotspotSchema.attrs.push_back(ScidbAttr("pm10_avg", DOUBLE));
    hotspotSchema.attrs.push_back(ScidbAttr("latitude", INT64));
    hotspotSchema.attrs.push_back(ScidbAttr("longitude", INT64));

    // cout << to_string(z1) << "," << to_string(z2) << "," << to_string(max_win_oneside) << endl;
    auto hotspot = scidb->download("limit(sort(redimension(apply(window(aggregate(between(finedust, " + to_string(z1) + ", 0, 0, " + to_string(z2) + ", 522, 522), sum(pm10), count(pm10), latitude, longitude), 2, 2, 2, 2, sum(pm10_sum), sum(pm10_count)), pm10_avg, pm10_sum_sum / pm10_count_sum), <pm10_avg:double, latitude:int64, longitude: int64>[i=0:*:0:100000000]), pm10_avg desc), 1)", hotspotSchema);
    auto hotspotCells = hotspot->readcell();
    auto current = ST_ClosestObject_RoadNode(mapCollection, neo4j, lat, lon);    // Current coord
    int target = -1;
    while (hotspotCells.size() != 0) {
        auto targetLat = 34.011898718557454 + (double) get<long long>(hotspotCells.at(2)) * 0.000172998;
        auto targetLon = -118.34501002237936 + (double) get<long long>(hotspotCells.at(3)) * 0.000216636;
        // cout << targetLat << targetLon << (double) get<double>(hotspotCells.at(1)) << endl;

        target = ST_ClosestObject_RoadNode(mapCollection, neo4j, targetLat, targetLon);
        break;
    }

    std::string neoq = "MATCH (source:Roadnode {roadnode_id: " + to_string(current) + "}), (target:Roadnode {roadnode_id: " + to_string(target) + "}) \
        CALL gds.shortestPath.dijkstra.stream('road_network', { \
            sourceNode: source, \
            targetNode: target, \
            relationshipWeightProperty: 'distance' \
        }) \
        YIELD index, sourceNode, targetNode, totalCost, nodeIds, costs, path \
        RETURN \
            index, \
            sourceNode, \
            targetNode, \
            totalCost, \
            costs, \
            nodes(path) as path \
        LIMIT 1";

    // std::cout << neoq << std::endl;

    auto neor = neo4j_run(neo4j->conn, neoq.c_str(), neo4j_null);
    auto neof = neo4j_fetch_next(neor);
    if (neof == NULL) std::cout << "No result!!!" << std::endl;
    else {
        auto nSource =  neo4j_int_value(neo4j_result_field(neof, 1));
        auto nTarget = neo4j_int_value(neo4j_result_field(neof, 2));
        auto nTotalCost = neo4j_float_value(neo4j_result_field(neof, 3));
        char buf[65536];
        std::string nPath = neo4j_tostring(neo4j_result_field(neof, 5), buf, sizeof(buf));

        // cout << to_string(nSource) << ", " << to_string(nTarget) << ", " << to_string(nTotalCost) << ", " << nPath << endl;
        nrows++;
    }

    cout << "[TASK15]: TOTAL " << nrows << " ROWS ARE REPORTED" << endl;
}



/**
 *  [Task16] Fine Dust Backtesting ([D, A]=> D).
 *  For a given timestamp Z, hindcast the pm10 values of the schools. (The Z is teh number between min, max of the timestamp dimension.)
 *
 *  Z1 = (Z/TimeInterval)*TimeInterval
 *  Z2 = {(Z+TimeInterval-1)/TimeInterval}*TimeInterval
 *
 *  A: SELECT avg(pm10) FROM FineDust
 *      WHERE timestamp >= Z1  and timestamp <= Z2 group by lat, lon
 *
 *  B: SELECT Map.properties.osm_id AS id, location,
 *      FROM Map, A
 *      WHERE
 *          WithIN(Box(lat, lon, lat+e1, lon+e2), ST_Centroid(Map.geometry))
 *          Map.properties.building = 'school' //Document
 *
 */
void T16(long timestamp) {


    auto mongodb = mongodb_connector("Disaster");
    auto map = mongodb.db["Site"];

    int arrayinfo_time_offset =1600182000;
    int arrayinfo_time_grid_interval = 10800;

    double arrayinfo_lat_offset = 34.01189870;
    double arrayinfo_lat_grid_interval =  0.000172998;

    double arrayinfo_lon_offset = -118.3450100223;
    double arrayinfo_lon_grid_interval = 0.000216636;

    double lat_max = arrayinfo_lat_grid_interval*522+arrayinfo_lat_offset;
    double lon_max = arrayinfo_lon_grid_interval*522+arrayinfo_lon_offset;

    int normZ1 = (timestamp - arrayinfo_time_offset)/arrayinfo_time_grid_interval;
    int normZ2 = (timestamp - arrayinfo_time_offset + arrayinfo_time_grid_interval -1)/arrayinfo_time_grid_interval;


    cout << "Z1:" <<normZ1 << ", Z2:" << normZ2 << endl;

    unique_ptr<ScidbConnection> conn(new ScidbConnection(SCIDB_HOST_DISASTER + ":8080"));

    //  aggregate(between(finedust, 0,null, null,1,null,null), avg(pm10), latitude, longitude)
    conn->exec("remove(finedust_temp)");
    conn->exec("store(aggregate(between(finedust,"+to_string(normZ1)+",null, null,"+to_string(normZ2)+",null,null), avg(pm10), latitude, longitude),finedust_temp)");


    mongocxx::pipeline p{};
    p.match(make_document(kvp("properties.type","building")));
    p.match(make_document(kvp("properties.description","school")));
    p.project(make_document(kvp("building_id", "$_id"),
                            kvp("coordinates", "$geometry.coordinates"),
                            kvp("_id", 0)));

    mongocxx::options::aggregate options;
    options.allow_disk_use(true);


    int nrow = 0;
    ScidbSchema schema;
    schema.dims.push_back(ScidbDim("latitude", 0, INT32_MAX, 0, 1000000));
    schema.dims.push_back(ScidbDim("longitude", 0, INT32_MAX, 0, 1000000));
    schema.attrs.push_back(ScidbAttr("pm10", FLOAT));
    auto cursor = map.aggregate(p,options);


    int nschool = 0;
    for ( auto school : cursor) {

        nschool++;
        auto json = Json::parse(bsoncxx::to_json(school));
        auto centroid = STcentroid(json["coordinates"]);
        auto school_lat = get<1>(centroid);
        auto school_lon = get<0>(centroid);

        if( school_lon <= lon_max && school_lon >= arrayinfo_lon_offset
            && school_lat <= lat_max && school_lat >= arrayinfo_lat_offset
                ){

            auto cell1 = high_resolution_clock::now();
            int school_lon_norm = (school_lon-arrayinfo_lon_offset)/arrayinfo_lon_grid_interval;
            int school_lat_norm = (school_lat-arrayinfo_lat_offset)/arrayinfo_lat_grid_interval;


            string query = "between(finedust_temp,"
                                                 +to_string(school_lat_norm)
                                                 +","+to_string(school_lon_norm)
                                                 +","+to_string(school_lat_norm)
                                                 +","+to_string(school_lon_norm)+ ")";

            auto download = conn->download(query,schema);
            auto cell = download->readcell();
            while( cell.size()!= 0){
                double lat  = get<int>(cell.at(0)) ;
                double lon  = get<int>(cell.at(1)) ;
                float pm10  = get<float>(cell.at(2)) ;
                double cell_lat = lat*arrayinfo_lat_grid_interval+arrayinfo_lat_offset;
                double cell_lon = lon*arrayinfo_lon_grid_interval+arrayinfo_lon_offset;

                nrow++;
//                cout << query << endl;
                cell = download->readcell();
            }
            auto cell2 = high_resolution_clock::now();

//            cout << duration_cast<microseconds>(cell2 - cell1).count()  << endl;


        }
    }



//    cout << "[TASK16]: TOTAL " << nrow << " ROWS ARE REPORTED" << endl;
    cout << "[TASK16]: TOTAL " << nrow << " ROWS ARE REPORTED" << endl;


}

////
//// Created by mxmdb on 21. 5. 3..
////
//#include "include/Connection/mysql_connector.h"
//#include "include/Connection/mongodb_connector.h"
//#include <nlohmann/json.hpp>
//#include <mongocxx/client.hpp>
//#include <bsoncxx/builder/stream/document.hpp>
//#include <Connection/ScidbConnection.h>
//#include <math.h>
//#include <time.h>
//
//#include <boost/geometry.hpp>
//
//using bsoncxx::builder::basic::kvp;
//using bsoncxx::builder::basic::make_array;
//using bsoncxx::builder::basic::make_document;
//
////using bsoncxx::builder::stream::document;
////using bsoncxx::builder::stream::open_array;
////using bsoncxx::builder::stream::value;
//
//using Json = nlohmann::json;
//
//double distance(double lat, double lon, double lat2, double lon2){
//    return sqrt(( lat - lat2)*(lat - lat2) + (lon - lon2)*(lon - lon2));
//}
//
//const double pi = acos(-1);
//double deg2rad(double deg) {
//    return deg / 180 * pi;
//}
//
//double distance_haversine(double lat1, double lon1, double lat2, double lon2) {
//    return 2 * 6378 * asin(sqrt(pow(sin(deg2rad((lat2 - lat1) / 2)), 2) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * pow(sin(deg2rad((lon2 - lon1) / 2)), 2)));
//}
//
//// Deprecated
//std::tuple<double, double> ST_Centroid(bsoncxx::types::b_document geometry) {
//    double cLon, cLat;
//    auto geojsonType = geometry.value["type"].get_utf8().value.to_string();
//    if (geojsonType.compare("MultiPolygon") == 0) {
//        auto p = geometry.value["coordinates"].get_array().value;
//        // calculating centroid
//        double lonsum = 0, latsum = 0;
//        int count = 0;
//        for (auto pit = p.begin(); pit != p.end(); pit++) {
//            auto lss = pit->get_array().value;
//            for (auto lssit = lss.begin(); lssit != lss.end(); lssit++) {
//                auto ls = lssit->get_array().value;
//                for (auto lsit = ls.begin(); lsit != ls.end(); lsit++) {
//                    auto po = lsit->get_array().value;
//                    lonsum += po[0].get_double();
//                    latsum += po[1].get_double();
//                    count++;
//                }
//            }
//        }
//        cLon = lonsum / count;
//        cLat = latsum / count;
//    } else {
//        cLon = geometry.value["coordinates"].get_array().value[0].get_double();
//        cLat = geometry.value["coordinates"].get_array().value[1].get_double();
//    }
//
//    return std::make_tuple(cLon, cLat);
//}
//
//namespace bg = boost::geometry;
//
//std::tuple<double, double> ST_Centroid_bd_test(string geom) {
//    typedef bg::model::d2::point_xy<double> point_type;
//    typedef bg::model::polygon<point_type> polygon_type;
//    typedef bg::model::multi_polygon<polygon_type> mpolygon_type;
//
//    mpolygon_type mpoly;
//    bg::read_wkt(geom, mpoly);
//
//    point_type p;
//    bg::centroid(mpoly, p);
//
//    // std::cout << "centroid: " << bg::dsv(p) << std::endl;
//    return make_tuple(p.x(), p.y());
//}
//
//std::tuple<double, double> ST_Centroid_bd(Json multipolygon) {
//    typedef bg::model::d2::point_xy<double> point_type;
//    typedef bg::model::polygon<point_type> polygon_type;
//    typedef bg::model::multi_polygon<polygon_type> mpolygon_type;
//
//    mpolygon_type mpoly;
//
//    // I implemented this like the following way (making wkt)
//    //      since building boost mpoly may has complicate rules.
//    stringstream ss;
//    ss << fixed;
//    ss << "MULTIPOLYGON(";
//    for (auto polygon : multipolygon) {
//        ss << "(";
//        for (auto ring : polygon) {
//            ss << "(";
//            for (auto point : ring) {
//                ss << setprecision(7) << point[0].get<double>() << " " << setprecision(7) << point[1].get<double>() << ",";
//            }
//            ss.seekp(-1, ss.cur);
//            ss << ")";
//        }
//        ss << ")";
//    }
//    ss << ")";
//    // std::cout << ss.str()  << std::endl;
//    bg::read_wkt(ss.str(), mpoly);
//
//    point_type p;
//    bg::centroid(mpoly, p);
//
//    // std::cout << "centroid: " << bg::dsv(p) << std::endl;
//    return make_tuple(p.x(), p.y());
//}
//
//
//std::tuple<double, double> STcentroid(Json multipolygon){
//    double lon = 0.0;
//    double lat = 0.0;
//    int nrow = 0;
//
//    for ( auto val1 : multipolygon){
//        for (auto val2 : val1){
//            for( auto val3 : val2){
//                lon += val3[0].get<double>();
//                lat += val3[1].get<double>();
//                nrow++;
//            }
//        }
//    }
//    return std::make_tuple(lon/nrow,lat/nrow);
//}
//
//
///*
// * To find the closest centroid from the coordinates, we need to fetch the data from MongoDB.
// */
//int ST_ClosestObject_Map_building_centroid(mongocxx::collection mapCentroidCollection, double lat, double lon) {
//    auto nnQBuilder = bsoncxx::builder::stream::document{};
//    bsoncxx::document::value nnQ = nnQBuilder
//            << "centroid" << bsoncxx::builder::stream::open_document
//            << "$near" << bsoncxx::builder::stream::open_document
//            << "$geometry" << bsoncxx::builder::stream::open_document
//            << "type" << "Point"
//            << "coordinates" << bsoncxx::builder::stream::open_array
//            << lon << lat
//            << bsoncxx::builder::stream::close_array
//            << bsoncxx::builder::stream::close_document
//            // << "$maxDistance" << 1000                               // Magic Number
//            << bsoncxx::builder::stream::close_document
//            << bsoncxx::builder::stream::close_document
//            << bsoncxx::builder::stream::finalize;
//    auto doc = mapCentroidCollection.find_one(nnQ.view(), mongocxx::options::find{}.limit(1));
//    return doc->view()["site_id"].get_int32();
//}
//
//
//bsoncxx::stdx::optional<bsoncxx::document::value> ST_ClosestObject_Map_building(mongocxx::collection mapCollection, double lat, double lon) {
//    auto nnQBuilder = bsoncxx::builder::stream::document{};
//    bsoncxx::document::value nnQ = nnQBuilder
//            << "properties.type" << "building"
//            << "geometry" << bsoncxx::builder::stream::open_document
//            << "$near" << bsoncxx::builder::stream::open_document
//            << "type" << "Point"
//            << "coordinates" << bsoncxx::builder::stream::open_array
//            << lon << lat
//            << bsoncxx::builder::stream::close_array
//            << bsoncxx::builder::stream::close_document
//            << bsoncxx::builder::stream::close_document
//            << bsoncxx::builder::stream::finalize;
//    cout << bsoncxx::to_json(nnQ) << endl;
//    auto docs = mapCollection.find_one(nnQ.view(), mongocxx::options::find{}.limit(1));
//    return docs;
//}
//
//int ST_ClosestObject_RoadNode(mongocxx::collection mapCollection, shared_ptr<neo4j_connector> neo4j, double candidate_lat, double candidate_lon) {
//    // find closest site document
//    auto nnQBuilder = bsoncxx::builder::stream::document{};
//    bsoncxx::document::value nnQ = nnQBuilder
//            << "properties.type" << "roadnode"
//            << "geometry" << bsoncxx::builder::stream::open_document
//            << "$near" << bsoncxx::builder::stream::open_document
//            << "type" << "Point"
//            << "coordinates" << bsoncxx::builder::stream::open_array
//            << candidate_lon << candidate_lat
//            << bsoncxx::builder::stream::close_array
//            << bsoncxx::builder::stream::close_document
//            << bsoncxx::builder::stream::close_document
//            << bsoncxx::builder::stream::finalize;
//    // cout << bsoncxx::to_json(nnQ) << endl;
//    auto docs = mapCollection.find_one(nnQ.view(), mongocxx::options::find{}.limit(1));
//
//    // find roadnode_id
//    std::string pmatch = "(n:Roadnode {site_id: " + to_string(docs->view()["site_id"].get_int32().value) + "})";
//    std::string preturn = "n.roadnode_id";
//    std::string query_str = ("MATCH " + pmatch + " RETURN " + preturn );
//
//    auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
//
//    neo4j_result_t *result = neo4j_fetch_next(results);
//    if (result == NULL) assert("no result!!");
//
//    return neo4j_int_value(neo4j_result_field(result, 0));
//}
//
//
///**
// * [Task 10] Road Network Filtering ([R, D, G]=> G)
// * For the earthquakes which occurred between time Z1 and Z2, find the road network subgraph within 5km from the earthquakes' location.
// *
// * A = SELECT n1, r, n2 AS subgraph FROM Earthquake, Site, RoadNode
// *     WHERE (n1:RoadNode) - [r:Road] -> (n2:RoadNode) AND ST_Distance(Site.geometry, Earthquake.coordinates) <= 5km
// *     AND Earthquake.time >= Z1 AND Earthquake.time < Z2 AND RoadNode.site_id = Site.site_id //Graph
//*/
//void T10(string Z1, string Z2){
//
//    Z1 = "\'" + Z1 + "\'";
//    Z2 = "\'" + Z2 + "\'";
//
//    auto mysql = mysql_connector();
//    auto Earthquake = mysql.mysess->getSchema("Disaster").getTable("Earthquake");
//    auto mongodb = mongodb_connector("Disaster");
//    auto Site = mongodb.db["Site"];
//    auto neo4j = new neo4j_connector();
//
//    std::string where = "time >= STR_TO_DATE(" + Z1 +", '%Y-%m-%d %H:%i:%s') AND time < STR_TO_DATE(" + Z2 + ", '%Y-%m-%d %H:%i:%s')";
//    auto matched_earthquakes = Earthquake.select("earthquake_id","ST_X(coordinates)", "ST_Y(coordinates)").where(where).execute();
//
//    int res_count=0;
//    for ( auto eqk: matched_earthquakes) {
//        int eqk_id = eqk[0];
//        double eqk_lat = eqk[1].get<double>(), eqk_lon = eqk[2].get<double>();
//        auto roadnode_sites = Site.find(
//                make_document(kvp("geometry",
//                                  make_document(kvp("$nearSphere",make_document(kvp(
//                                          "$geometry", make_document(kvp("type","Point"), kvp("coordinates", make_array(eqk_lon,eqk_lat)))),
//                                                                                kvp("$minDistance" , 0), kvp("$maxDistance", 5000))
//                                  ))),
//                              kvp("properties.type", "roadnode")));
//
//        for (auto vertex: roadnode_sites) {
//            auto json = Json::parse(bsoncxx::to_json(vertex));
//            std::string graph_query =
//                    "MATCH (n:Roadnode)-[r:Road]->(m:Roadnode)"
//                    "WHERE n.site_id = " + to_string(json["site_id"])
//                    + " RETURN n, r, m";
//            auto results = neo4j_run(neo4j->conn, graph_query.c_str(), neo4j_null);
//            while (1) {
//                neo4j_result_t *result = neo4j_fetch_next(results);
//                if (result == NULL) break;
//
//                int pos = 0;
//                while (1) {
//                    auto n = neo4j_result_field(result, pos);
//                    if (!neo4j_is_null(n)) {}
//                    else break;
//                    pos++;
//                }
//                res_count++;
//            }
//        }
//    }
//    cout << "TASK10 TOTAL " << res_count << " ROWS ARE REPORTED" << endl;
//}
//
//
///**
//  *  [Task 11] Closest Shelter ([R, D, G]=> R)
//  *  For a given earthquake information X, find the cost of the shortest path for each GPS coordinate and Shelter pair.
//  *  (GPS coordinates are limited by 1 hour and 10km from the X. Shelters are limited by 15km from the X.)
//  *
//  *  A = SELECT GPS.gps_id, ST_ClosestObject(Site, roadnode, GPS.coordinates) AS roadnode_id FROM GPS, Site, RoadNode
//  *      WHERE GPS.time >= X.time AND GPS.time < X.time + 1 hour AND ST_Distance(GPS.coordinates, X.coordinates) <= 10km
//  *      AND RoadNode.site_id = Site.site_id //Relational
//  *
//  *  B = SELECT t.shelter_id, ST_ClosestObject(Site, roadnode, ST_Centroid(t.geometry)) AS roadnode_id
//  *      FROM RoadNode, Site, (SELECT Shelter.shelter_id, Site.geometry FROM Site, Shelter WHERE ST_Distance(ST_Centroid(Site.geometry), X.coordinates) <= 15km AND Shelter.site_id = Site.site_id) AS t
//  *      WHERE RoadNode.site_id = Site.site_id  //Relational
//  *
//  *  C = SELECT A.gps_id, B.shelter_id, ShortestPath(RoadNode, startNode:A.roadnode_id, endNode:B.roadnode_id) AS cost
//  *      FROM A, B, RoadNode //Relational
//  */
//void T11(int X_id) {
//    auto mysql = mysql_connector();
//    auto neo4j = neo4j_connector();
//    auto mongodb = mongodb_connector("Disaster");
//    auto Site = mongodb.db["Site"];
//
//    int buffer = 0;
//    mysql.mysess->sql("use Disaster").execute();
//    mysql.mysess->sql("CREATE TEMPORARY TABLE Gps_list(gps_id INT, gps_node_id INT)").execute();
//    mysql.mysess->sql("CREATE TEMPORARY TABLE Shelter_list(shelter_id INT, shelter_node_id INT)").execute();
//
//    auto Earthquake = mysql.mysess->getSchema("Disaster").getTable("Earthquake");
//    mysql.mysess->sql("CREATE TEMPORARY TABLE IF NOT EXISTS eqk_X AS (SELECT time, coordinates FROM Earthquake WHERE earthquake_id=" + to_string(X_id) +")").execute();
//
//    /* step A: find gps */
//    std::string select = "Gps.gps_id, ST_Y(Gps.coordinates), ST_X(Gps.coordinates) ";
//    std::string from = "Gps, eqk_X ";
//    std::string where = "Gps.time >= eqk_X.time AND Gps.time < date_add(eqk_X.time, interval 1 hour)"
//                        " AND ST_Distance_Sphere("
//                        "ST_GeomFromText(CONCAT('POINT (', ST_X(Gps.coordinates), ' ', ST_Y(Gps.coordinates), ')'), 4326), "
//                        "ST_GeomFromText(CONCAT('POINT (', ST_X(eqk_X.coordinates), ' ', ST_Y(eqk_X.coordinates), ')'), 4326)"
//                        ") <= 10000";
//    auto filtered_gps = mysql.mysess->sql("SELECT " + select + "FROM " + from + "WHERE " + where).execute();
//
//    auto insert2Gps_list = mysql.mysess->getSchema("Disaster").getTable("Gps_list").insert("gps_id", "gps_node_id");
//    for (auto gps: filtered_gps) {
//        int gps_id = gps[0].get<int>();
//        double gps_lon = gps[1].get<double>(), gps_lat = gps[2].get<double>();
//
//        mongocxx::pipeline p{};
//        p.geo_near(make_document(
//                kvp("near", make_document(kvp("type", "Point"), kvp("coordinates", make_array(gps_lon, gps_lat)))),
//                kvp("key", "geometry"),
//                kvp("distanceField", "dist.calculated"),
//                kvp("query", make_document(kvp("properties.type", "roadnode"))),
//                kvp("spherical", "true")
//                   )
//        );
//        p.limit(1);
//        auto gps_node = Site.aggregate(p, mongocxx::options::aggregate{});
//        int gps_node_id;
//        for (auto cursor: gps_node) {
//            auto json = Json::parse(bsoncxx::to_json(cursor));
//            gps_node_id = json["site_id"];
//
//            buffer++;
//            insert2Gps_list.values(gps_id, gps_node_id);
//            if(buffer >= BUFFER) {
//                insert2Gps_list.execute();
//                insert2Gps_list = mysql.mysess->getSchema("Disaster").getTable("Gps_list").insert("gps_id", "gps_node_id");
//                buffer = 0;
//            }
//        }
//    }
//    if(buffer != 0) insert2Gps_list.execute();
//    buffer =0;
//
//    /* step B: find shelters */
//    //create TEMP TABLE Shelter_temp (shelter with geometry)
//    mysql.mysess->sql("CREATE TEMPORARY TABLE Shelter_temp(shelter_id INT, site_id INT, lon double, lat double)").execute();
//    auto insert2Shelter_temp = mysql.mysess->getSchema("Disaster").getTable("Shelter_temp").insert("shelter_id", "site_id", "lon", "lat");
//    auto shelter = mysql.mysess->sql("SELECT shelter_id, site_id FROM Shelter").execute();
//    for (auto row: shelter) {
//        int shelter_id = row[0].get<int>(), site_id = row[1].get<int>();
//        auto shelter_geom = Site.find_one(make_document(kvp("site_id", site_id)));
//        auto json = Json::parse(bsoncxx::to_json(*shelter_geom));
//        auto centroid = ST_Centroid_bd(json["geometry"]["coordinates"]);
//
//        //insert to Shelter_temp
//        insert2Shelter_temp.values(shelter_id, site_id, get<0>(centroid), get<1>(centroid));
//        buffer++;
//        if (buffer >= BUFFER) {
//            insert2Shelter_temp.execute();
//            insert2Shelter_temp = mysql.mysess->getSchema("Disaster").getTable("Shelter_temp").insert("shelter_id", "site_id", "lon", "lat");
//            buffer = 0;
//        }
//    }
//    if (buffer != 0) insert2Shelter_temp.execute();
//    buffer = 0;
//    mysql.mysess->sql("CREATE INDEX shelter_id_idx ON Shelter_temp(shelter_id)").execute();
//    mysql.mysess->sql("CREATE INDEX lon_idx ON Shelter_temp(lon)").execute();
//    mysql.mysess->sql("CREATE INDEX lat_idx ON Shelter_temp(lat)").execute();
//
//    select = " Shelter_temp.shelter_id, Shelter_temp.lon, Shelter_temp.lat ";
//    from = " eqk_X, Shelter_temp ";
//    where = " ST_Distance_Sphere("
//            "ST_GeomFromText(CONCAT('POINT (', Shelter_temp.lat, ' ', Shelter_temp.lon, ')'), 4326), "
//            "ST_GeomFromText(CONCAT('POINT (', ST_X(eqk_X.coordinates), ' ', ST_Y(eqk_X.coordinates), ')'), 4326)"
//            ") <= 15000";
//    auto filtered_shelter = mysql.mysess->sql("SELECT" + select + "FROM" + from + "WHERE" + where).execute();
//
//    auto insert2Shelter_list = mysql.mysess->getSchema("Disaster").getTable("Shelter_list").insert("shelter_id", "shelter_node_id");
//    for(auto shelter: filtered_shelter){
//        int shelter_id = shelter[0].get<int>();
//        double shelter_lon = shelter[1].get<double>(), shelter_lat = shelter[2].get<double>();
//
//        mongocxx::pipeline p{};
//        p.geo_near(make_document(
//                kvp("near", make_document(kvp("type", "Point"), kvp("coordinates", make_array(shelter_lon, shelter_lat)))),
//                kvp("key", "geometry"),
//                kvp("distanceField", "dist.calculated"),
//                kvp("query", make_document(kvp("properties.type", "roadnode"))),
//                kvp("spherical", "true")
//                   )
//        );
//        p.limit(1);
//        auto shelter_node = Site.aggregate(p, mongocxx::options::aggregate{});
//        int shelter_node_id;
//        for (auto cursor: shelter_node) {
//            auto json = Json::parse(bsoncxx::to_json(cursor));
//            shelter_node_id = json["site_id"];
//
//            insert2Shelter_list.values(shelter_id, shelter_node_id);
//            buffer++;
//            if (buffer >= BUFFER) {
//                insert2Shelter_list.execute();
//                insert2Shelter_list = mysql.mysess->getSchema("Disaster").getTable("Shelter_list").insert("shelter_id",
//                                                                                                          "gps_node_id");
//                buffer = 0;
//            }
//        }
//    }
//    if(buffer != 0) insert2Shelter_list.execute();
//
//    /* step C: find shortest path */
//    int res_count = 0;
//    auto gps_list = mysql.mysess->sql("SELECT * FROM Gps_list").execute();
//    for(auto row1: gps_list){
//        int gps_id = row1[0].get<int>();
//        int gps_node_id = row1[1].get<int>();
//
//        auto shelter_list = mysql.mysess->sql("SELECT * FROM Shelter_list").execute();
//        for(auto row2: shelter_list) {
//            int shelter_id = row2[0].get<int>();
//            int shelter_node_id = row2[1].get<int>();
//
//            std::string pmatch =
//                    "(gps:Roadnode {site_id:" + to_string(gps_node_id) + "}), (shelter:Roadnode {site_id:" + to_string(shelter_node_id) + "})";
//            std::string pcall = "gds.shortestPath.dijkstra.stream('road_network', { sourceNode: gps, targetNode: shelter, relationshipWeightProperty: 'distance'})";
//            std::string pyield = "sourceNode, targetNode, totalCost";
//            std::string preturn = "totalCost";
//
//            std::string graph_query = ("MATCH " + pmatch + " CALL " + pcall + " YIELD " + pyield + "  RETURN " +  preturn);
//            auto graph_results = neo4j_run(neo4j.conn, graph_query.c_str(), neo4j_null);
//            neo4j_result_t *graph_result = neo4j_fetch_next(graph_results);
//            if (graph_result == NULL) continue;
//            auto cost = neo4j_float_value(neo4j_result_field(graph_result, 0));
//            //cout << gps_id << "   " << shelter_id << ", Distance: "<< cost << endl;
//            res_count++;
//        }
//    }
//    cout << "TASK11 TOTAL " << res_count << " ROWS ARE REPORTED" << endl;
//
//}
//
//
//
///**
// * [Task 12] New Shelter ([R, D, G]=> R)
// * For the shelter of which the number of GPS coordinates are the most within 5km from the shelter between time Z1 and Z2,
// * find five closest buildings from the shelter. The buildings are limited by 1km from the shelter.
// *
// * A = SELECT Shelter.shelter_id, Site.geometry
// *     FROM Shelter, Site, (SELECT Shelter.shelter_id, COUNT(GPS.gps_id) AS cnt FROM Shelter, GPS, Site
// *                          WHERE ST_Distance(GPS.coordinates, ST_Centroid(Site.geometry)) <= 5km AND Site.site_id = Shelter.site_id
// *                          AND GPS.time >= Z1 AND GPS.time < Z2 GROUP BY Shelter.id ORDER BY cnt DESC LIMIT 1) AS t
// *     WHERE Shelter.shelter_id = t.shelter_id AND Site.site_id = Shelter.site_id  //Relational
// *
// * B = SELECT A.shelter_id, ST_ClosestObject(Site, roadnode, ST_centroid(A.geometry)) AS roadnode_id
// *     FROM A, RoadNode, Site
// *     WHERE Site.site_id = RoadNode.site_id  //Relational
// *
// * C = SELECT t.site_id, ST_ClosestObject(Site, roadnode, ST_Centroid(t.geometry)) AS roadnode_id
// *     FROM Site, RoadNode, (SELECT Site.site_id, Site.geometry FROM Site, A
// *                           WHERE ST_Distance(Site.geometry, ST_centroid(A.geometry)) <= 1km AND Site.properties.type = 'building') AS t
// *     WHERE RoadNode.site_id = Site.site_id  //Relational
// *
// * D = SELECT C.site_id, ShortestPath(RoadNode, startNode:B.roadnode_id, endNode:C.roadnode_id) AS cost
// *     FROM B, C, RoadNode
// *     ORDER BY cost LIMIT 5  //Relational
// */
//void T12(string Z1, string Z2) {
//
//    auto mysql = mysql_connector();
//    auto neo4j = neo4j_connector();
//    auto mongodb = mongodb_connector("Disaster");
//    auto Site = mongodb.db["Site"];
//
//    int buffer = 0;
//    mysql.mysess->sql("use Disaster").execute();
//    mysql.mysess->sql("CREATE TEMPORARY TABLE Shelter_temp(shelter_id INT, site_id INT, lon double, lat double)").execute();
//    auto insert2Shelter_temp = mysql.mysess->getSchema("Disaster").getTable("Shelter_temp").insert("shelter_id", "site_id", "lon", "lat");
//    auto shelter = mysql.mysess->sql("SELECT shelter_id, site_id FROM Shelter").execute();
//    for (auto row: shelter) {
//        int shelter_id = row[0].get<int>(), site_id = row[1].get<int>();
//        auto shelter_geom = Site.find_one(make_document(kvp("site_id", site_id)));
//        auto json = Json::parse(bsoncxx::to_json(*shelter_geom));
//        auto centroid = ST_Centroid_bd(json["geometry"]["coordinates"]);
//
//        //insert to temp table
//        insert2Shelter_temp.values(shelter_id, site_id, get<0>(centroid), get<1>(centroid));
//        buffer++;
//        if (buffer >= BUFFER) {
//            insert2Shelter_temp.execute();
//            insert2Shelter_temp = mysql.mysess->getSchema("Disaster").getTable("Shelter_temp").insert("shelter_id", "site_id", "lon", "lat");
//            buffer = 0;
//        }
//    }
//    if (buffer != 0) insert2Shelter_temp.execute();
//    buffer = 0;
//    mysql.mysess->sql("CREATE INDEX shelter_id_idx ON Shelter_temp(shelter_id)").execute();
//    mysql.mysess->sql("CREATE INDEX lon_idx ON Shelter_temp(lon)").execute();
//    mysql.mysess->sql("CREATE INDEX lat_idx ON Shelter_temp(lat)").execute();
//
//    std::string select = " Shelter_temp.shelter_id AS shelter_id, Shelter_temp.lon AS shelter_lon, Shelter_temp.lat AS shelter_lat, COUNT(Gps.gps_id) AS numGps ";
//    std::string from = " Gps, Shelter_temp ";
//    Z1 = "\'" + Z1 + "\'";
//    Z2 = "\'" + Z2 + "\'";
//    std::string where = " Gps.time >= STR_TO_DATE(" + Z1 +", '%Y-%m-%d %H:%i:%s') AND Gps.time < STR_TO_DATE(" + Z2 + ", '%Y-%m-%d %H:%i:%s') AND ST_Distance_Sphere("
//                                                                                                                      "ST_GeomFromText(CONCAT('POINT (', Shelter_temp.lat, ' ', Shelter_temp.lon, ')'), 4326), "
//                                                                                                                      "ST_GeomFromText(CONCAT('POINT (', ST_X(coordinates), ' ', ST_Y(coordinates), ')'), 4326)"
//                                                                                                                      ") <= 5000";
//    std::string others = " GROUP BY Shelter_temp.shelter_id, Shelter_temp.lon, Shelter_temp.lat ORDER BY numGps DESC LIMIT 1";
//    auto target_shelter = mysql.mysess->sql("SELECT" + select + "FROM" + from + "WHERE" + where + others).execute().fetchOne();
//    int target_shelter_id = target_shelter[0].get<int>(); //2272
//    double target_shelter_lon = target_shelter[1].get<double>(), target_shelter_lat = target_shelter[2].get<double>();
//
//    mongocxx::pipeline p{};
//    p.geo_near(make_document(
//            kvp("near", make_document(kvp("type", "Point"), kvp("coordinates", make_array(target_shelter_lon, target_shelter_lat)))),
//            kvp("key", "geometry"),
//            kvp("distanceField", "dist.calculated"),
//            kvp("query", make_document(kvp("properties.type", "roadnode"))),
//            kvp("spherical", "true")
//               )
//    );
//    p.limit(1);
//    auto shelter_node = Site.aggregate(p, mongocxx::options::aggregate{});
//    int shelter_node_id;
//    for (auto cursor: shelter_node) {
//        auto json = Json::parse(bsoncxx::to_json(cursor));
//        shelter_node_id = json["site_id"];
//    }
//
//    /* Next, find buildings. */
//    mysql.mysess->sql("CREATE TEMPORARY TABLE Building_list(building_id INT, node_id INT)").execute();
//    auto insert2Building_list = mysql.mysess->getSchema("Disaster").getTable("Building_list").insert("building_id", "node_id");
//    auto buildings = Site.find(
//            make_document(kvp("geometry",
//                              make_document(kvp("$nearSphere",make_document(kvp(
//                                      "$geometry", make_document(kvp("type","Point"), kvp("coordinates", make_array(target_shelter_lon, target_shelter_lat)))),
//                                                                            kvp("$minDistance" , 0), kvp("$maxDistance", 1000))
//                              ))),
//                          kvp("properties.type", "building")));
//
//    for (auto cursor: buildings) {
//        auto json = Json::parse(bsoncxx::to_json(cursor));
//        int building_id = json["site_id"];
//        auto centroid = ST_Centroid_bd(json["geometry"]["coordinates"]);
//
//        //For each building, find closest roadnode.
//        mongocxx::pipeline p{};
//        p.geo_near(make_document(
//                kvp("near", make_document(kvp("type", "Point"), kvp("coordinates", make_array(get<0>(centroid), get<1>(centroid))))),
//                kvp("key", "geometry"),
//                kvp("distanceField", "dist.calculated"),
//                kvp("query", make_document(kvp("properties.type", "roadnode"))),
//                kvp("spherical", "true")
//                   )
//        );
//        p.limit(1);
//
//        auto building_node = Site.aggregate(p, mongocxx::options::aggregate{});
//        for (auto b: building_node) {
//            auto b_json = Json::parse(bsoncxx::to_json(b));
//            int building_node_id = b_json["site_id"];
//
//            insert2Building_list.values(building_id, building_node_id);
//            buffer++;
//            if (buffer >= BUFFER) {
//                insert2Building_list.execute();
//                insert2Building_list = mysql.mysess->getSchema("Disaster").getTable("Building_list").insert("building_id", "node_id");
//                buffer = 0;
//            }
//        }
//    }
//    if (buffer != 0) insert2Building_list.execute();
//    buffer = 0;
//
//    /*calculate shortest path and find top 5 new buildings for shelter. */
//    int res_count=0;
//    mysql.mysess->sql("CREATE TEMPORARY TABLE Shortest_path(building_id INT, distance FLOAT)").execute();
//    auto insert2Shortest_path = mysql.mysess->getSchema("Disaster").getTable("Shortest_path").insert("building_id", "distance");
//
//    auto building_list = mysql.mysess->getSchema("Disaster").getTable("Building_list").select("*").execute();
//    for (auto row : building_list) {
//        int building_id = row[0].get<int>();
//        int node_id = row[1].get<int>();
//
//        std::string pmatch =
//                "(shelter:Roadnode {site_id:" + to_string(shelter_node_id) + "}), (building:Roadnode {site_id:" + to_string(node_id) + "})";
//        std::string pcall = "gds.shortestPath.dijkstra.stream('road_network', { sourceNode: shelter, targetNode: building, relationshipWeightProperty: 'distance'})";
//        std::string pyield = "sourceNode, targetNode, totalCost";
//        std::string preturn = "totalCost";
//        std::string graph_query = ("MATCH " + pmatch + " CALL " + pcall + " YIELD " + pyield + "  RETURN " +  preturn);
//        auto graph_results = neo4j_run(neo4j.conn, graph_query.c_str(), neo4j_null);
//        neo4j_result_t *graph_result = neo4j_fetch_next(graph_results);
//        if (graph_result == NULL) continue;
//        auto cost = neo4j_float_value(neo4j_result_field(graph_result, 0));
//
//        insert2Shortest_path.values(building_id, cost);
//        buffer++;
//        if (buffer >= BUFFER) {
//            insert2Shortest_path.execute();
//            insert2Shortest_path = mysql.mysess->getSchema("Disaster").getTable("Shortest_path").insert("building_id", "distance");
//            buffer = 0;
//        }
//    }
//    if (buffer != 0) insert2Shortest_path.execute();
//
//    auto top5 = mysql.mysess->getSchema("Disaster").getTable("Shortest_path").select("building_id").orderBy("distance", "building_id").limit(5).execute();
//    for (auto row : top5) {
//        res_count++;
//        //cout<< "building_id: "<< row[0].get<int>()<<endl;
//    }
//    cout << "TASK12 TOTAL " << res_count << " ROWS ARE REPORTED" << endl;
//}
//void deprecated_T12(string ts1, string ts2){
//
//    auto mysql = mysql_connector();
//    auto mongodb = mongodb_connector("Disaster");
//    auto map = mongodb.db["map"];
//
//    mysql.mysess->sql("use Disaster").execute();
//    string query = " SELECT Shelter.id, ST_X(Shelter.coordinates), ST_Y(Shelter.coordinates), COUNT(Gps.id) AS cnt "
//                   " FROM Shelter, Gps "
//                   " WHERE MBRContains(ST_BUFFER(Gps.coordinates, 0.07, ST_Buffer_Strategy('point_square')), Shelter.coordinates)"
//                   " AND ST_Distance(ST_GeomFromText(ST_AsText(Shelter.coordinates),4236), ST_GeomFromText(ST_AsText(Gps.coordinates),4236), 'metre')/1000 <= 1 "
//                   " AND Gps.time >= '"+ts1+"' and Gps.time <= '"+ts2+"' "
//                                                                      " GROUP BY Shelter.id, Shelter.coordinates ORDER BY cnt DESC LIMIT 1";
//
//
//    cout << query << endl;
//
//    auto rows = mysql.mysess->sql(query).execute();
//
//    int shelter_id = 0;
//    double shelter_lat = 0;
//    double shelter_lon = 0;
//    for(auto row : rows){
//        shelter_id = row[0].get<int>();
//        shelter_lat = row[1].get<double>();
//        shelter_lon = row[2].get<double>();
//    }
//
//    cout << shelter_id << " " << shelter_lat << " " << shelter_lon ;
////    int shelter_id = 6533;
////    double shelter_lat = 34.060447;
////    double shelter_lon = -118.277207;
//
////    shared_ptr<neo4j_connector> neo4j(new neo4j_connector());
////
////    std::string pmatch = "(n:Roadnode)";
////    std::string preturn = "n.roadnode_id, n.latitude, n.longitude";
////
////    std::string query_str = ("MATCH " + pmatch + " RETURN " + preturn );
////    cout << query_str << endl;
////    auto results = neo4j_run(neo4j->conn, query_str.c_str(), neo4j_null);
////
////    double min_distance = 100;
////    int shelter_node_id = 0;
////    int shelter_node_lat = 0;
////    int shelter_node_lon = 0;
////
////    while (1) {
////        neo4j_result_t *result = neo4j_fetch_next(results);
////        if (result == NULL) break;
////
////        auto node_id =  neo4j_int_value(neo4j_result_field(result, 0));
////        auto node_lat = neo4j_float_value(neo4j_result_field(result, 1));
////        auto node_lon = neo4j_float_value(neo4j_result_field(result, 2));
////
////        double dist = distance(shelter_lat,shelter_lon,node_lat,node_lon);
////        if( dist <= min_distance){
////            min_distance = dist;
////            shelter_node_id = node_id;
////            shelter_node_lat = node_lat;
////            shelter_node_lon = node_lon;
////        }
////    }
////
////
////    auto buildings = map.find(make_document(kvp("geometry", make_document(kvp("$near",
////                                                                              make_document(kvp("$geometry", make_document(kvp("type","Point"),
////                                                                                                                           kvp("coordinates", make_array(shelter_lon,shelter_lat)))),
////                                                                                            kvp("$minDistance" , 0),
////                                                                                            kvp("$maxDistance", 200))
////                                            ))),
////                                            kvp("properties.building", make_document(kvp("$exists", true)))));
////
////    int nrow = 0;
////    for (auto building : buildings){
////
////        auto json = Json::parse(bsoncxx::to_json(building));
////        auto centroid = STcentroid(json["geometry"]["coordinates"]);
////
////
////        double candidate_lon = get<0>(centroid);
////        double candidate_lat = get<1>(centroid);
////
////        int candidate_node_id = 0;
////        double candidate_node_lat = 0;
////        double candidate_node_lon = 0;
////
////        auto crnt = ST_ClosestObject_RoadNode(neo4j, candidate_lat, candidate_lon);
////        candidate_node_id = get<0>(crnt); candidate_node_lat = get<1>(crnt); candidate_node_lon = get<2>(crnt);
////
//////        cout << candidate_node_id << "\t" << candidate_node_lat <<"\t" << candidate_node_lon <<"\t" << min_distance << endl;
////
////        std::string pmatch2 = "(shelter:Roadnode {roadnode_id:"+to_string(shelter_node_id)+"}), (candidate {roadnode_id: "+to_string(candidate_node_id)+"})";
////        std::string pcall2 = "gds.shortestPath.dijkstra.stream('road_network', { sourceNode:shelter , targetNode: candidate, relationshipWeightProperty: 'distance'})";
////        std::string pyield2 = "sourceNode, targetNode, totalCost";
////        std::string preturn2 = "gds.util.asNode(sourceNode).roadnode_id, gds.util.asNode(targetNode).roadnode_id, totalCost";
////
////        std::string query_str2 = ("MATCH " + pmatch2 + " CALL " + pcall2 + " YIELD " + pyield2 + "  RETURN " + preturn2 );
//////        cout <<query_str2<< endl;
////
////        auto results2 = neo4j_run(neo4j->conn, query_str2.c_str(), neo4j_null);
////
////        while (1) {
////            neo4j_result_t *result = neo4j_fetch_next(results2);
////            if (result == NULL) break;
////
////            auto shelter_node_id_res =  neo4j_int_value(neo4j_result_field(result, 0));
////            auto candidate_node_id_res = neo4j_int_value(neo4j_result_field(result, 1));
////            auto cost = neo4j_float_value(neo4j_result_field(result, 2));
////
////            cout << "NodeId: " << shelter_node_id_res << "\tNodeId: " << candidate_node_id_res << "\tDistance: " << cost << endl;
////        }
////        nrow++;
////    }
////    cout << "[TASK12]: TOTAL " << nrow << " ROWS ARE REPORTED" << endl;
//
//}
//
//mongocxx::collection constructCentroidCollection(mongocxx::database mdb, mongocxx::collection mapCollection, string name, bool wSiteid) {
//    auto mapCentroidCollection = mdb[name];       // for saving temp centroid data
//
//    // Optimization
//    std::vector<bsoncxx::document::value> buffer;
//
//    // Site -> Site_controid: conversion multipolygon to point
//    mongocxx::options::find option = mongocxx::options::find{}.no_cursor_timeout(true);         // If not set, cursor is broken while the query running.
//    auto docs0 = mapCollection.find(bsoncxx::builder::stream::document{}
//                                            << "properties.type" << "building"
//                                            << bsoncxx::builder::stream::finalize, option);
//
//    for (auto doc : docs0) {
//        // Calculating centroid
//        double cLat, cLon;
//        // Note that the Site only has documents which geometry type are MultiPolygon or Point.
//        auto geoJson = Json::parse(bsoncxx::to_json(doc["geometry"].get_document()));
//        if (geoJson["type"] == "MultiPolygon") {
//            auto centroid = ST_Centroid_bd(geoJson["coordinates"]);
//            cLon = get<0>(centroid), cLat = get<1>(centroid);
//        } else {
//            cLon = geoJson["coordinates"][0], cLat = geoJson["coordinates"][1];
//        }
//
//        auto builder = bsoncxx::builder::stream::document{};
//        builder << "desc" << doc["properties"].get_document().value["description"].get_utf8();
//        if (wSiteid) builder << "site_id" << doc["site_id"].get_int32();
//        auto val = builder << "centroid" << bsoncxx::builder::stream::open_document
//                           << "type" << "Point"
//                           << "coordinates" << bsoncxx::builder::stream::open_array
//                           << cLon << cLat << bsoncxx::builder::stream::close_array
//                           << bsoncxx::builder::stream::close_document
//                           << bsoncxx::builder::stream::finalize;
//        buffer.push_back(val);
//
//        if (buffer.size() >= 1000) {
//            mapCentroidCollection.insert_many(buffer);
//            buffer.clear();
//        }
//
//    }
//
//    // clear buffer
//    if (!buffer.empty()) {
//        mapCentroidCollection.insert_many(buffer);
//        buffer.clear();
//    }
//
//    // on-the-fly index
//    auto index_specification = document{} << "centroid" << "2dsphere" << finalize;
//    mapCentroidCollection.create_index(std::move(index_specification));
//
//    return mapCentroidCollection;
//}
//
//void boost_centroid_test() {
//    unique_ptr<mongodb_connector> mongodb(new mongodb_connector("Disaster"));
//    auto mdb = mongodb->db;
//    auto mapCollection = mdb["Site"];
//
//    // Site -> Site_controid: conversion multipolygon to point
//    mongocxx::options::find option = mongocxx::options::find{}.no_cursor_timeout(true);         // If not set, cursor is broken while the query running.
//    auto docs0 = mapCollection.find(bsoncxx::builder::stream::document{}
//                                            << "site_id" << bsoncxx::builder::stream::open_document
//                                            << "$in" << bsoncxx::builder::stream::open_array
//                                            << 8460056 << 8458588
//                                            << bsoncxx::builder::stream::close_array
//                                            << bsoncxx::builder::stream::close_document
//                                            << bsoncxx::builder::stream::finalize, option);
//
//    for (auto doc : docs0) {
//        // Calculating centroid
//        double cLat, cLon;
//        // Note that the Site only has documents which geometry type are MultiPolygon or Point.
//        auto geoJson = Json::parse(bsoncxx::to_json(doc["geometry"].get_document()));
//        if (geoJson["type"] == "MultiPolygon") {
//            auto centroid = ST_Centroid_bd(geoJson["coordinates"]);
//            cLon = get<0>(centroid), cLat = get<1>(centroid);
//        } else {
//            cLon = geoJson["coordinates"][0], cLat = geoJson["coordinates"][1];
//        }
//
//        cout << bsoncxx::to_json(doc) << " , " << cLon << " , " << cLat << endl;
//    }
//}
//
//
///*
// * [Task 13] Damage Statistics.
// *
// * For the earthquakes of which magnitude is greater than 4.5, find the building statistics.
// * The buildings are limited by 30km from the earthquake location. (Relational, Document) -> Document
// *
// * A =  SELECT Site.properties.description, COUNT(*)
// *      FROM Earthquake, Site WHERE ST_Distance(Site.geometry, Earthquake.coordinates) <= 30km
// *          AND Site.properties.type = 'building'
// *          AND Earthquake.magnitude >= 4.5
// *      GROUP BY Site.properties.description // Document
// *
// * @note I implemented this by nested loop (outer is relational and inner is document).
// * To use GROUP BY clause, T13 stores all building type to MongoDB and uses $group (of aggregate()) at the end.
// *
// * @note2 I used $nearSphere instead of $geoWithin for calculating distance
// * because $geoWithin requires the document within the $centerSphere entirely (i.e. 30km is applied to the maximum of distance).
// */
//void T13() {
//    size_t nrows = 0;
//    unique_ptr<mysql_connector> mysql(new mysql_connector());
//    auto earthquakeTable = mysql->mysess->getSchema("Disaster").getTable("Earthquake");
//
//    unique_ptr<mongodb_connector> mongodb(new mongodb_connector("Disaster"));
//    auto mdb = mongodb->db;
//    auto mapCollection = mdb["Site"];
//    auto mapGroupCollection = mdb["Site_group"];             // for group by
//    auto mapCentroidCollection = constructCentroidCollection(mdb, mapCollection, "Site_centroid", false);
//
//    // Optimization
//    std::vector<bsoncxx::document::value> buffer;
//
//    // Fetching Earthquake with magnitude >= 4.5
//    auto rows1 = earthquakeTable.select("ST_X(coordinates)", "ST_Y(coordinates)")
//            .where("magnitude >= 4.5").execute();
//    for (auto row : rows1) {
//        double lat = row[0].get<double>(), lon = row[1].get<double>();
//        auto docq1Builder = bsoncxx::builder::stream::document{};
//        bsoncxx::document::value docq1 = docq1Builder
//                << "centroid" << bsoncxx::builder::stream::open_document
//                << "$geoWithin" << bsoncxx::builder::stream::open_document
//                << "$centerSphere" << bsoncxx::builder::stream::open_array
//                << bsoncxx::builder::stream::open_array
//                << lon << lat
//                << bsoncxx::builder::stream::close_array
//                << (double) 30 / 6378.1
//                << bsoncxx::builder::stream::close_array
//                << bsoncxx::builder::stream::close_document
//                << bsoncxx::builder::stream::close_document
//                << bsoncxx::builder::stream::finalize;
//        // std::cout << bsoncxx::to_json(docq1) << std::endl;
//        auto docs1 = mapCentroidCollection.find(docq1.view(),
//                                                mongocxx::options::find{}.projection(bsoncxx::builder::stream::document{}
//                                                                                             << "_id" << 0
//                                                                                             << "desc" << 1
//                                                                                             << bsoncxx::builder::stream::finalize));
//
//        for (auto doc : docs1) {
//            buffer.push_back(bsoncxx::builder::stream::document{}
//                                     << "desc" << doc["desc"].get_utf8()
//                                     << bsoncxx::builder::stream::finalize);
//
//            if (buffer.size() >= 1000) {
//                mapGroupCollection.insert_many(buffer);
//                buffer.clear();
//            }
//        }
//    }
//
//    // clear buffer
//    if (!buffer.empty()) {
//        mapGroupCollection.insert_many(buffer);
//        buffer.clear();
//    }
//
//    // group by and projection
//    mongocxx::pipeline p{};
//    auto docq2Builder = bsoncxx::builder::stream::document{};
//    bsoncxx::document::value docq2 = docq2Builder
//            << "_id" << "$desc"
//            << "count" << bsoncxx::builder::stream::open_document
//            << "$sum" << 1
//            << bsoncxx::builder::stream::close_document
//            << bsoncxx::builder::stream::finalize;
//    p.group(docq2.view());
//    auto docs2 = mapGroupCollection.aggregate(p, mongocxx::options::aggregate{});
//    for (auto doc : docs2) {
//        // std::cout << bsoncxx::to_json(doc) << endl;
//        nrows++;
//    }
//
//    mapGroupCollection.drop();
//    mapCentroidCollection.drop();
//
//    std::cout << "[TASK13]: TOTAL " << nrows << " ROWS ARE REPORTED" << endl;
//}
//
///*
// * [Task 14] Sources of Fine Dust.
// *
// * Analyze fine dust hotspot by date between time Z1 and Z2.
// * Print the nearest building with time of the hotspot.
// * Use window aggregation with a size of 5. (Document, Array) -> Document
// *
// * A =  SELECT date, timestamp, latitude, longitude, AVG(pm10) AS pm10_avg
// *      FROM FineDust
// *      WHERE timestamp >= Z1
// *          AND timestamp <= Z2
// *      WINDOW 1, 5, 5 // Array
// * B =  REDIMENSION(A, <pm10_avg: float>[date=0:*, timestamp=0:*, latitude=0:*, longitude=0:*]) // Array
// * C =  SELECT t1.date, t1.timestamp, (t1.latitude, t1.longitude) AS coordinates
// *      FROM B AS t1, (
// *          SELECT date, MAX(pm10_avg) AS pm10_max
// *          FROM B GROUP BY date
// *      ) AS t2
// *      WHERE t1.pm10_avg = t2.pm10_max
// *          AND t1.date = t2.date // Array
// * D =  SELECT C.date, C.timestamp, ST_ClosestObject(Site, building, C.coordinates) AS site_id
// *      FROM C, Site
// *      ORDER BY C.date ASC // Document
// *
// * @note
// */
//void T14(int z1, int z2) {
//    unique_ptr<ScidbConnection> scidb(new ScidbConnection(SCIDB_HOST_DISASTER + ":8080"));
//    unique_ptr<mongodb_connector> mongodb(new mongodb_connector("Disaster"));
//    auto mdb = mongodb->db;
//    auto mapCollection = mdb["Site"];
//    auto mapCentroidCollection = constructCentroidCollection(mdb, mapCollection, "Site_centroid", true);
//
//    size_t nrows = 0;
//
//    // Query A and B
//    // 8 is magic number for dataset
//    scidb->exec("store(redimension(apply(window(between(finedust, " + to_string(z1) + ", 0, 0, " + to_string(z2) + ", 522, 522), 0, 0, 2, 2, 2, 2, avg(pm10)), date, timestamp/8), <pm10_avg: double>[date=0:*:0:?; timestamp=0:*:0:?; latitude=0:*:0:?; longitude=0:*:0:?]), t14t1)");
//
//    // Query C
//    ScidbSchema t2Schema;
//    t2Schema.dims.push_back(ScidbDim("$n", 0, INT32_MAX, 0, 1000000));
//    t2Schema.attrs.push_back(ScidbAttr("pm10_avg_max", DOUBLE)); t2Schema.attrs.push_back(ScidbAttr("date", INT64));
//
//    ScidbSchema maxSchema;
//    maxSchema.dims.push_back(ScidbDim("i", 0, INT32_MAX, 0, 1000000));
//    maxSchema.attrs.push_back(ScidbAttr("pm10_avg", DOUBLE));
//    maxSchema.attrs.push_back(ScidbAttr("latitude", INT64));
//    maxSchema.attrs.push_back(ScidbAttr("longitude", INT64));
//    maxSchema.attrs.push_back(ScidbAttr("timestamp", INT64));
//
//    auto t2arr = scidb->download("sort(redimension(aggregate(t14t1, max(pm10_avg), date), <pm10_avg_max: double, date: int64>[i=0:*:0:1000]), date)", t2Schema);
//    auto t2arrVal = t2arr->readcell();
//    while (t2arrVal.size() != 0) {
//        // cout << get<double>(t2arrVal.at(1)) << " " << get<long long>(t2arrVal.at(2)) << endl;
//        long long date = get<long long>(t2arrVal.at(2));
//        double maxVal = get<double>(t2arrVal.at(1));
//
//        // get location of value
//        // If you have a better solution, please improve it.
//        // Another way to do this is that replace the query for t2arr to cross_join (t14t1 and max aggregated one), but it is slow a little bit.
//        auto maxArr = scidb->download("sort(redimension(filter(t14t1, abs(pm10_avg - " + to_string(maxVal) + ") < 1 and timestamp / 8 = " + to_string(date) + "), <pm10_avg:double, latitude:int64, longitude: int64, timestamp: int64>[i=0:*:0:1000]), pm10_avg)", maxSchema);
//        auto maxArrVal = maxArr->readcell();
//        if (maxArrVal.size() == 0) throw std::runtime_error("equality check for floating point is failed!");
//
//        auto closestValue = ST_ClosestObject_Map_building_centroid(mapCentroidCollection,
//                                                                   34.011898718557454 + (double) get<long long>(maxArrVal.at(2)) * 0.000172998,
//                                                                   -118.34501002237936 + (double) get<long long>(maxArrVal.at(3)) * 0.000216636);
//        // cout << date << " " << get<long long>(maxArrVal.at(4)) << " " << to_string(closestValue) << " " << endl;
//
//        t2arrVal = t2arr->readcell();
//        nrows++;
//    }
//
//    scidb->exec("remove(t14t1)");
//    mapCentroidCollection.drop();
//
//    cout << "[TASK14]: TOTAL " << nrows << " ROWS ARE REPORTED" << endl;
//}
//
//
///*
// * [Task15] Fine Dust Cleaning Vehicles.
// *
// * Recommend the route from the current coordinates by analyzing the hotspot between time Z1 and Z2.
// * Use window aggregation with a size of 5. (Graph, Document, Array) -> Relational
// *
// * A =  SELECT (latitude, longitude) AS coordinates, AVG(pm10) AS pm10_avg
// *      FROM FineDust
// *      WHERE timestamp >= Z1
// *          AND timestamp <= Z2
// *      WINDOW *, 5, 5 // Array
// * B =  SELECT ShortestPath(RoadNode, startNode: ST_ClosestObject(Site, roadnode, current_coordinates), endNode: ST_ClosestObject(Site, roadnode, A.coordinates))
// *      FROM A, RoadNode, Site
// *      WHERE Site.site_id = RoadNode.site_id
// *      ORDER BY A.pm10_avg DESC
// *      LIMIT 1 // Relational
// */
//void T15(int z1, int z2, double lon, double lat) {
//    size_t nrows = 0;
//
//    unique_ptr<ScidbConnection> scidb(new ScidbConnection(SCIDB_HOST_DISASTER + ":8080"));
//    shared_ptr<neo4j_connector> neo4j(new neo4j_connector());
//    unique_ptr<mongodb_connector> mongodb(new mongodb_connector("Disaster"));
//    auto mdb = mongodb->db;
//    auto mapCollection = mdb["Site"];
//
//    // Query A and B
//    int max_win_oneside = z2 - z1;
//
//    ScidbSchema hotspotSchema;
//    hotspotSchema.dims.push_back(ScidbDim("i", 0, INT32_MAX, 0, 1000000));
//    hotspotSchema.attrs.push_back(ScidbAttr("pm10_avg", DOUBLE));
//    hotspotSchema.attrs.push_back(ScidbAttr("latitude", INT64));
//    hotspotSchema.attrs.push_back(ScidbAttr("longitude", INT64));
//
//    // cout << to_string(z1) << "," << to_string(z2) << "," << to_string(max_win_oneside) << endl;
//    auto hotspot = scidb->download("limit(sort(redimension(apply(window(aggregate(between(finedust, " + to_string(z1) + ", 0, 0, " + to_string(z2) + ", 522, 522), sum(pm10), count(pm10), latitude, longitude), 2, 2, 2, 2, sum(pm10_sum), sum(pm10_count)), pm10_avg, pm10_sum_sum / pm10_count_sum), <pm10_avg:double, latitude:int64, longitude: int64>[i=0:*:0:100000000]), pm10_avg desc), 1)", hotspotSchema);
//    auto hotspotCells = hotspot->readcell();
//    auto current = ST_ClosestObject_RoadNode(mapCollection, neo4j, lat, lon);    // Current coord
//    int target = -1;
//    while (hotspotCells.size() != 0) {
//        auto targetLat = 34.011898718557454 + (double) get<long long>(hotspotCells.at(2)) * 0.000172998;
//        auto targetLon = -118.34501002237936 + (double) get<long long>(hotspotCells.at(3)) * 0.000216636;
//        // cout << targetLat << targetLon << (double) get<double>(hotspotCells.at(1)) << endl;
//
//        target = ST_ClosestObject_RoadNode(mapCollection, neo4j, targetLat, targetLon);
//        break;
//    }
//
//    std::string neoq = "MATCH (source:Roadnode {roadnode_id: " + to_string(current) + "}), (target:Roadnode {roadnode_id: " + to_string(target) + "}) \
//        CALL gds.shortestPath.dijkstra.stream('road_network', { \
//            sourceNode: source, \
//            targetNode: target, \
//            relationshipWeightProperty: 'distance' \
//        }) \
//        YIELD index, sourceNode, targetNode, totalCost, nodeIds, costs, path \
//        RETURN \
//            index, \
//            sourceNode, \
//            targetNode, \
//            totalCost, \
//            costs, \
//            nodes(path) as path \
//        LIMIT 1";
//
//    // std::cout << neoq << std::endl;
//
//    auto neor = neo4j_run(neo4j->conn, neoq.c_str(), neo4j_null);
//    auto neof = neo4j_fetch_next(neor);
//    if (neof == NULL) std::cout << "No result!!!" << std::endl;
//    else {
//        auto nSource =  neo4j_int_value(neo4j_result_field(neof, 1));
//        auto nTarget = neo4j_int_value(neo4j_result_field(neof, 2));
//        auto nTotalCost = neo4j_float_value(neo4j_result_field(neof, 3));
//        char buf[65536];
//        std::string nPath = neo4j_tostring(neo4j_result_field(neof, 5), buf, sizeof(buf));
//
//        // cout << to_string(nSource) << ", " << to_string(nTarget) << ", " << to_string(nTotalCost) << ", " << nPath << endl;
//        nrows++;
//    }
//
//    cout << "[TASK15]: TOTAL " << nrows << " ROWS ARE REPORTED" << endl;
//}
//
//
//
///**
// *  [Task16] Fine Dust Backtesting ([D, A]=> D).
// *  For a given timestamp Z, hindcast the pm10 values of the schools. (The Z is teh number between min, max of the timestamp dimension.)
// *
// *  Z1 = (Z/TimeInterval)*TimeInterval
// *  Z2 = {(Z+TimeInterval-1)/TimeInterval}*TimeInterval
// *
// *  A: SELECT avg(pm10) FROM FineDust
// *      WHERE timestamp >= Z1  and timestamp <= Z2 group by lat, lon
// *
// *  B: SELECT Map.properties.osm_id AS id, location,
// *      FROM Map, A
// *      WHERE
// *          WithIN(Box(lat, lon, lat+e1, lon+e2), ST_Centroid(Map.geometry))
// *          Map.properties.building = 'school' //Document
// *
// */
//void T16(long timestamp) {
//
//
//    auto mongodb = mongodb_connector("Disaster");
//    auto map = mongodb.db["Site"];
//
//    int arrayinfo_time_offset =1600182000;
//    int arrayinfo_time_grid_interval = 10800;
//
//    double arrayinfo_lat_offset = 34.01189870;
//    double arrayinfo_lat_grid_interval =  0.000172998;
//
//    double arrayinfo_lon_offset = -118.3450100223;
//    double arrayinfo_lon_grid_interval = 0.000216636;
//
//    double lat_max = arrayinfo_lat_grid_interval*522+arrayinfo_lat_offset;
//    double lon_max = arrayinfo_lon_grid_interval*522+arrayinfo_lon_offset;
//
//    int normZ1 = (timestamp - arrayinfo_time_offset)/arrayinfo_time_grid_interval;
//    int normZ2 = (timestamp - arrayinfo_time_offset + arrayinfo_time_grid_interval -1)/arrayinfo_time_grid_interval;
//
//
//    cout << "Z1:" <<normZ1 << ", Z2:" << normZ2 << endl;
//
//    unique_ptr<ScidbConnection> conn(new ScidbConnection(SCIDB_HOST_DISASTER + ":8080"));
//
//    //  aggregate(between(finedust, 0,null, null,1,null,null), avg(pm10), latitude, longitude)
//    conn->exec("remove(finedust_temp)");
//    conn->exec("store(aggregate(between(finedust,"+to_string(normZ1)+",null, null,"+to_string(normZ2)+",null,null), avg(pm10), latitude, longitude),finedust_temp)");
//
//
//    mongocxx::pipeline p{};
//    p.match(make_document(kvp("properties.type","building")));
//    p.match(make_document(kvp("properties.description","school")));
//    p.project(make_document(kvp("building_id", "$_id"),
//                            kvp("coordinates", "$geometry.coordinates"),
//                            kvp("_id", 0)));
//
//    mongocxx::options::aggregate options;
//    options.allow_disk_use(true);
//
//
//    int nrow = 0;
//    ScidbSchema schema;
//    schema.dims.push_back(ScidbDim("latitude", 0, INT32_MAX, 0, 1000000));
//    schema.dims.push_back(ScidbDim("longitude", 0, INT32_MAX, 0, 1000000));
//    schema.attrs.push_back(ScidbAttr("pm10", FLOAT));
//    auto cursor = map.aggregate(p,options);
//
//
//    int nschool = 0;
//    for ( auto school : cursor) {
//
//        nschool++;
//        auto json = Json::parse(bsoncxx::to_json(school));
//        auto centroid = STcentroid(json["coordinates"]);
//        auto school_lat = get<1>(centroid);
//        auto school_lon = get<0>(centroid);
//
//        if( school_lon <= lon_max && school_lon >= arrayinfo_lon_offset
//            && school_lat <= lat_max && school_lat >= arrayinfo_lat_offset
//                ){
//            int school_lon_norm = (school_lon-arrayinfo_lon_offset)/arrayinfo_lon_grid_interval;
//            int school_lat_norm = (school_lat-arrayinfo_lat_offset)/arrayinfo_lat_grid_interval;
//
//            auto download = conn->download("between(finedust_temp,"
//                                           +to_string(school_lat_norm)
//                                           +","+to_string(school_lon_norm)
//                                           +","+to_string(school_lat_norm)
//                                           +","+to_string(school_lon_norm)+ ")",schema);
//
//            auto cell = download->readcell();
//            while( cell.size()!= 0){
//                double lat  = get<int>(cell.at(0)) ;
//                double lon  = get<int>(cell.at(1)) ;
//                float pm10  = get<float>(cell.at(2)) ;
//                double cell_lat = lat*arrayinfo_lat_grid_interval+arrayinfo_lat_offset;
//                double cell_lon = lon*arrayinfo_lon_grid_interval+arrayinfo_lon_offset;
//
//                nrow++;
//                cell = download->readcell();
//            }
//        }
//    }
//
//
//
////    cout << "[TASK16]: TOTAL " << nrow << " ROWS ARE REPORTED" << endl;
//    cout << "[TASK16]: TOTAL " << nrow << " ROWS ARE REPORTED" << endl;
//
//
//}
