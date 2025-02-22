/**
 * Created: May 3, 2021
 * Updated: Feburary 2025
 */

#include <nlohmann/json.hpp>
#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include "Connection/mysql_connector.h"
#include "Connection/mongodb_connector.h"
#include "Connection/ScidbConnection.h"

using Json = nlohmann::json;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

#define SCIDB_HOST_DISASTER "127.0.0.1"
#define BUFFER 1000

std::tuple<double, double> STcentroid(Json multipolygon)
{
    double lon = 0.0;
    double lat = 0.0;
    int nrow = 0;

    for (auto val1 : multipolygon)
    {
        for (auto val2 : val1)
        {
            for (auto val3 : val2)
            {
                lon += val3[0].get<double>();
                lat += val3[1].get<double>();

                nrow++;
            }
        }
    }

    return std::make_tuple(lon / nrow, lat / nrow);
}

int ST_ClosestObject_RoadNode(mongocxx::collection mapCentroidCollection, double candidate_lat, double candidate_lon)
{
    auto nnQBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value nnQ = nnQBuilder
                                   << "properties.type"
                                   << "roadnode"
                                   << "centroid" << bsoncxx::builder::stream::open_document
                                   << "$near" << bsoncxx::builder::stream::open_document
                                   << "type"
                                   << "Point"
                                   << "coordinates" << bsoncxx::builder::stream::open_array
                                   << candidate_lon << candidate_lat
                                   << bsoncxx::builder::stream::close_array
                                   << bsoncxx::builder::stream::close_document
                                   << bsoncxx::builder::stream::close_document
                                   << bsoncxx::builder::stream::finalize;
    auto doc = mapCentroidCollection.find_one(nnQ.view(), mongocxx::options::find{}.limit(1));

    if (doc)
        return doc->view()["site_id"].get_int32();

    return -1;
}

int ST_ClosestObject_Map_building_centroid(mongocxx::collection mapCentroidCollection, double lat, double lon)
{
    auto nnQBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value nnQ = nnQBuilder
                                   << "properties.type"
                                   << "building"
                                   << "centroid" << bsoncxx::builder::stream::open_document
                                   << "$near" << bsoncxx::builder::stream::open_document
                                   << "$geometry" << bsoncxx::builder::stream::open_document
                                   << "type"
                                   << "Point"
                                   << "coordinates" << bsoncxx::builder::stream::open_array
                                   << lon << lat
                                   << bsoncxx::builder::stream::close_array
                                   << bsoncxx::builder::stream::close_document
                                   << bsoncxx::builder::stream::close_document
                                   << bsoncxx::builder::stream::close_document
                                   << bsoncxx::builder::stream::finalize;

    auto doc = mapCentroidCollection.find_one(nnQ.view(), mongocxx::options::find{}.limit(1));

    return doc->view()["site_id"].get_int32();
}

/**
 * [Task 14] Sources of Fine Dust.
 *
 * Analyze fine dust hotspots by date between time Z1 and Z2.
 * Print the nearest building with a time of the hotspot.
 * Use window aggregation with a size of 5. (Document, Array) -> Document
 *
 */
void T14(int z1, int z2)
{
    auto start_mongo = high_resolution_clock::now();
    mongodb_connector mongodb("Disaster");
    auto mapCentroidCollection = mongodb.db["Site_centroid"];
    auto end_mongo = high_resolution_clock::now();
    auto time_mongo = duration_cast<milliseconds>(end_mongo - start_mongo);

    auto start_scidb = high_resolution_clock::now();
    unique_ptr<ScidbConnection> scidb(new ScidbConnection(SCIDB_HOST_DISASTER + string(":8080")));

    // Query A and B
    // 8 is magic number for dataset
    scidb->exec("store(redimension(apply(window(between(Finedust, " + to_string(z1) + ", 0, 0, " + to_string(z2) + ", 522, 522), 0, 0, 2, 2, 2, 2, avg(pm10)), date, timestamp/8), "
                                                                                                                   "<pm10_avg: double>[date=0:*:0:?; timestamp=0:*:0:?; latitude=0:*:0:?; longitude=0:*:0:?]), t14t1)");

    // Query C
    ScidbSchema t2Schema;
    t2Schema.dims.push_back(ScidbDim("$n", 0, INT32_MAX, 0, 1000000));
    t2Schema.attrs.push_back(ScidbAttr("pm10_avg_max", DOUBLE));
    t2Schema.attrs.push_back(ScidbAttr("date", INT64));

    ScidbSchema maxSchema;
    maxSchema.dims.push_back(ScidbDim("i", 0, INT32_MAX, 0, 1000000));
    maxSchema.attrs.push_back(ScidbAttr("pm10_avg", DOUBLE));
    maxSchema.attrs.push_back(ScidbAttr("latitude", INT64));
    maxSchema.attrs.push_back(ScidbAttr("longitude", INT64));
    maxSchema.attrs.push_back(ScidbAttr("timestamp", INT64));
    auto end_scidb = high_resolution_clock::now();
    auto time_scidb = duration_cast<milliseconds>(end_scidb - start_scidb);

    auto start_comm = high_resolution_clock::now();
    auto t2arr = scidb->download("sort(redimension(aggregate(t14t1, max(pm10_avg), date), "
                                 "<pm10_avg_max: double, date: int64>[i=0:*:0:1000]), date)",
                                 t2Schema);
    auto t2arrVal = t2arr->readcell();
    auto end_comm = high_resolution_clock::now();
    auto time_comm = duration_cast<milliseconds>(end_comm - start_comm);

    // std::ofstream csv_file("/tmp/t14.csv");
    // csv_file << "date,timestamp,site_id\n";

    start_comm = high_resolution_clock::now();
    auto time_loop = 0;
    int nrow = 0;
    while (!t2arrVal.empty())
    {
        start_scidb = high_resolution_clock::now();
        long long date = get<long long>(t2arrVal.at(2));
        double maxVal = get<double>(t2arrVal.at(1));
        end_scidb = high_resolution_clock::now();
        time_scidb += duration_cast<milliseconds>(end_scidb - start_scidb);
        time_loop += duration_cast<milliseconds>(end_scidb - start_scidb);

        // Get location of value
        auto maxArr = scidb->download("sort(redimension(filter(t14t1, abs(pm10_avg - " + to_string(maxVal) + ") < 1e-6 and timestamp / 8 = " + to_string(date) + "), "
                                                                                                                                                                 "<pm10_avg:double, latitude:int64, longitude:int64, timestamp:int64>[i=0:*:0:1000]), pm10_avg, timestamp, latitude, longitude)",
                                      maxSchema);
        auto maxArrVal = maxArr->readcell();

        if (maxArrVal.empty())
            throw std::runtime_error("Equality check for floating point failed!");

        start_mongo = high_resolution_clock::now();
        auto closestValue = ST_ClosestObject_Map_building_centroid(mapCentroidCollection,
                                                                   34.011898718557454 + static_cast<double>(get<long long>(maxArrVal.at(2))) * 0.000172998,
                                                                   -118.34501002237936 + static_cast<double>(get<long long>(maxArrVal.at(3))) * 0.000216636);
        end_mongo = high_resolution_clock::now();
        time_mongo += duration_cast<milliseconds>(end_mongo - start_mongo);
        time_loop += duration_cast<milliseconds>(end_mongo - start_mongo);

        // csv_file << date << "," << get<long long>(maxArrVal.at(4)) << "," << to_string(closestValue) << "\n";

        t2arrVal = t2arr->readcell();
        nrow++;
    }
    end_comm = high_resolution_clock::now();
    time_comm += duration_cast<milliseconds>(end_comm - start_comm - time_loop);

    /* save result matrix to csv */
    // csv_file.close();

    start_scidb = high_resolution_clock::now();
    scidb->exec("remove(t14t1)");
    end_scidb = high_resolution_clock::now();
    time_scidb += duration_cast<milliseconds>(end_scidb - start_scidb);

    cout << "[TASK 14]: TOTAL " << nrow << " ROWS ARE REPORTED" << endl
         << endl;

    cout << "MongoDB: " << time_mongo.count() << " ms" << endl;
    cout << "SciDB: " << time_scidb.count() << " ms" << endl;
    cout << "Communication: " << time_comm.count() << " ms" << endl
         << endl;
}

/**
 * [Task15] Fine Dust Cleaning Vehicles.
 *
 * Recommend the route from the current coordinates by analyzing the hotspot between time Z1 and Z2.
 * Use window aggregation with a size of 5. (Graph, Document, Array) -> Relational
 */
void T15(int z1, int z2, double lon, double lat)
{
    auto start_mongo = high_resolution_clock::now();
    mongodb_connector mongodb("Disaster");
    auto mapCentroidCollection = mongodb.db["Site_centroid"];
    auto end_mongo = high_resolution_clock::now();
    auto time_mongo = duration_cast<milliseconds>(end_mongo - start_mongo);

    auto start_scidb = high_resolution_clock::now();
    unique_ptr<ScidbConnection> scidb(new ScidbConnection(SCIDB_HOST_DISASTER + string(":8080")));

    // Query A and B
    ScidbSchema hotspotSchema;
    hotspotSchema.dims.push_back(ScidbDim("i", 0, INT32_MAX, 0, 1000000));
    hotspotSchema.attrs.push_back(ScidbAttr("pm10_avg", DOUBLE));
    hotspotSchema.attrs.push_back(ScidbAttr("latitude", INT64));
    hotspotSchema.attrs.push_back(ScidbAttr("longitude", INT64));
    auto end_scidb = high_resolution_clock::now();
    auto time_scidb = duration_cast<milliseconds>(end_scidb - start_scidb);

    auto start_comm = high_resolution_clock::now();
    auto hotspot = scidb->download("limit(sort(redimension(apply(window(aggregate(between(Finedust, " + to_string(z1) + ", 0, 0, " + to_string(z2) + ", 522, 522), sum(pm10), count(pm10), latitude, longitude), "
                                                                                                                                                     "2, 2, 2, 2, sum(pm10_sum), sum(pm10_count)), "
                                                                                                                                                     "pm10_avg, pm10_sum_sum / pm10_count_sum), "
                                                                                                                                                     "<pm10_avg:double, latitude:int64, longitude:int64>[i=0:*:0:100000000]), pm10_avg desc), 1)",
                                   hotspotSchema);

    auto hotspotCells = hotspot->readcell();
    auto end_comm = high_resolution_clock::now();
    auto time_comm = duration_cast<milliseconds>(end_comm - start_comm);

    start_scidb = high_resolution_clock::now();
    double targetLat = 34.011898718557454 + static_cast<double>(get<long long>(hotspotCells.at(2))) * 0.000172998;
    double targetLon = -118.34501002237936 + static_cast<double>(get<long long>(hotspotCells.at(3))) * 0.000216636;
    end_scidb = high_resolution_clock::now();
    time_scidb += duration_cast<milliseconds>(end_scidb - start_scidb);

    start_mongo = high_resolution_clock::now();
    int current = ST_ClosestObject_RoadNode(mapCentroidCollection, lat, lon);
    int target = ST_ClosestObject_RoadNode(mapCentroidCollection, targetLat, targetLon);
    end_mongo = high_resolution_clock::now();
    time_mongo += duration_cast<milliseconds>(end_mongo - start_mongo);

    /* save result matrix to csv */
    // std::ofstream csv_file("/tmp/t15.csv");
    // csv_file << "int64\n" << current << "\n" << target << "\n";
    // csv_file.close();
    cout << current << ", " << target << endl;

    cout << "[TASK 15]: TASK COMPLETED" << endl
         << endl;

    cout << "MongoDB: " << time_mongo.count() << " ms" << endl;
    cout << "SciDB: " << time_scidb.count() << " ms" << endl;
    cout << "Communication: " << time_comm.count() << " ms" << endl
         << endl;
}

/**
 *  [Task16] Fine Dust Backtesting ([D, A]=> D).
 *  For a given timestamp Z, hindcast the pm10 values of the schools. (Z is a number between min and max of the timestamp dimension.)
 *
 *  Z1 = (Z/TimeInterval)*TimeInterval
 *  Z2 = {(Z+TimeInterval-1)/TimeInterval}*TimeInterval
 *
 *  A: SELECT avg(pm10) FROM FineDust
 *      WHERE timestamp >= Z1  and timestamp <= Z2 group by lat, lon
 *
 *  B: SELECT Map.properties.osm_id AS id, location
 *      FROM Map, A
 *      WHERE
 *          WITHIN(Box(lat, lon, lat+e1, lon+e2), ST_Centroid(Map.geometry))
 *          Map.properties.building = 'school' //Document
 */
void T16(int z1, int z2)
{
    auto start_mongo = high_resolution_clock::now();
    mongodb_connector mongodb("Disaster");
    auto map = mongodb.db["Site"];
    auto end_mongo = high_resolution_clock::now();
    auto time_mongo = duration_cast<milliseconds>(end_mongo - start_mongo);

    double arrayinfo_lat_offset = 34.01189870;
    double arrayinfo_lat_grid_interval = 0.000172998;

    double arrayinfo_lon_offset = -118.3450100223;
    double arrayinfo_lon_grid_interval = 0.000216636;

    double lat_max = arrayinfo_lat_grid_interval * 522 + arrayinfo_lat_offset;
    double lon_max = arrayinfo_lon_grid_interval * 522 + arrayinfo_lon_offset;

    auto start_scidb = high_resolution_clock::now();
    unique_ptr<ScidbConnection> conn(new ScidbConnection(SCIDB_HOST_DISASTER + string(":8080")));

    conn->exec("remove(finedust_temp)");
    conn->exec("store(aggregate(between(Finedust," + to_string(z1) + ",null,null," + to_string(z2) + ",null,null), avg(pm10), latitude, longitude), finedust_temp)");
    auto end_scidb = high_resolution_clock::now();
    auto time_scidb = duration_cast<milliseconds>(end_scidb - start_scidb);

    start_mongo = high_resolution_clock::now();
    mongocxx::pipeline stages;
    stages.match(make_document(kvp("properties.type", "building")));
    stages.match(make_document(kvp("properties.description", "school")));
    stages.project(make_document(
        kvp("building_id", "$_id"),
        kvp("site_id", "$site_id"),
        kvp("coordinates", "$geometry.coordinates"),
        kvp("_id", 0)));

    mongocxx::options::aggregate options;
    options.allow_disk_use(true);
    auto cursor = map.aggregate(stages, options);
    end_mongo = high_resolution_clock::now();
    time_mongo = duration_cast<milliseconds>(end_mongo - start_mongo);

    start_scidb = high_resolution_clock::now();
    ScidbSchema schema;
    schema.dims.push_back(ScidbDim("latitude", 0, INT32_MAX, 0, 1000000));
    schema.dims.push_back(ScidbDim("longitude", 0, INT32_MAX, 0, 1000000));
    schema.attrs.push_back(ScidbAttr("pm10", FLOAT));
    end_scidb = high_resolution_clock::now();
    time_scidb += duration_cast<milliseconds>(end_scidb - start_scidb);

    // std::ofstream csv_file("/tmp/t16.csv");
    // csv_file << "site_id,pm10\n";

    auto start_comm = high_resolution_clock::now();
    auto time_loop = 0;
    int nrow = 0;
    for (auto school : cursor)
    {
        start_mongo = high_resolution_clock::now();
        auto json = Json::parse(bsoncxx::to_json(school));
        auto centroid = STcentroid(json["coordinates"]);
        auto site_id = json["site_id"];

        auto school_lat = get<1>(centroid);
        auto school_lon = get<0>(centroid);
        end_mongo = high_resolution_clock::now();
        time_mongo += duration_cast<milliseconds>(end_mongo - start_mongo);
        time_loop += duration_cast<milliseconds>(end_mongo - start_mongo);

        if (school_lon <= lon_max && school_lon >= arrayinfo_lon_offset &&
            school_lat <= lat_max && school_lat >= arrayinfo_lat_offset)
        {
            int school_lon_norm = (school_lon - arrayinfo_lon_offset) / arrayinfo_lon_grid_interval;
            int school_lat_norm = (school_lat - arrayinfo_lat_offset) / arrayinfo_lat_grid_interval;

            start_scidb = high_resolution_clock::now();
            string query = "between(finedust_temp," + to_string(school_lat_norm) + "," + to_string(school_lon_norm) + "," + to_string(school_lat_norm) + "," + to_string(school_lon_norm) + ")";
            end_scidb = high_resolution_clock::now();
            time_scidb += duration_cast<milliseconds>(end_scidb - start_scidb);
            time_loop += duration_cast<milliseconds>(end_scidb - start_scidb);

            auto download = conn->download(query, schema);
            auto cell = download->readcell();
            while (cell.size() != 0)
            {
                start_scidb = high_resolution_clock::now();
                double lat = get<int>(cell.at(0));
                double lon = get<int>(cell.at(1));
                float pm10 = get<float>(cell.at(2));
                end_scidb = high_resolution_clock::now();
                time_scidb += duration_cast<milliseconds>(end_scidb - start_scidb);
                time_loop += duration_cast<milliseconds>(end_scidb - start_scidb);

                double cell_lat = lat * arrayinfo_lat_grid_interval + arrayinfo_lat_offset;
                double cell_lon = lon * arrayinfo_lon_grid_interval + arrayinfo_lon_offset;

                // csv_file << site_id << "," << pm10 << "\n";

                nrow++;
                cell = download->readcell();
            }
        }
    }
    auto end_comm = high_resolution_clock::now();
    auto time_comm += duration_cast<milliseconds>(end_comm - start_comm - time_loop);

    /* save result matrix to csv */
    // csv_file.close();

    cout << "[TASK 16]: TOTAL " << nrow << " ROWS ARE REPORTED" << endl
         << endl;

    cout << "MongoDB: " << time_mongo.count() << " ms" << endl;
    cout << "SciDB: " << time_scidb.count() << " ms" << endl;
    cout << "Communication: " << time_comm.count() << " ms" << endl
         << endl;
}
