#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <limits>
#include <cctype>
#include <iomanip>
#include <utility>

#include "crow_all.h" // Crow single header

using namespace std;

const double INF = 1e18;

// Enum for vehicle types
enum VehicleType { CAR = 0, BIKE = 1, TRAIN = 2, PLANE = 3 };

// Vehicle information structure
struct VehicleInfo {
    string name;
    double speed;
    double costPerKm;

    VehicleInfo() : name(""), speed(0), costPerKm(0) {}
    VehicleInfo(string n, double s, double c) : name(n), speed(s), costPerKm(c) {}
};

// Road/Edge structure
struct Road {
    string destination;
    double distance;
    int trafficLevel;
    double toll;
    VehicleType vehicle;
    bool isOpen;

    Road(string dest, double dist, VehicleType veh, int traffic = 5, double t = 0.0)
        : destination(dest), distance(dist), vehicle(veh), trafficLevel(traffic), toll(t), isOpen(true) {}
};

// Route result structure
struct RouteResult {
    vector<string> path;
    double totalDistance;
    double totalTime;
    double totalCost;
    bool found;

    RouteResult() : totalDistance(0), totalTime(0), totalCost(0), found(false) {}
};

// TrafficGraph class
class TrafficGraph {
private:
    map<string, vector<Road>> adjList;
    map<VehicleType, VehicleInfo> vehicles;

    VehicleType stringToVehicle(string s) {
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        if (s == "car") return CAR;
        if (s == "bike") return BIKE;
        if (s == "train") return TRAIN;
        if (s == "plane") return PLANE;
        return CAR;
    }

public:
    TrafficGraph() {
        vehicles[CAR] = VehicleInfo("Car", 80.0, 0.15);
        vehicles[BIKE] = VehicleInfo("Bike", 60.0, 0.05);
        vehicles[TRAIN] = VehicleInfo("Train", 100.0, 0.10);
        vehicles[PLANE] = VehicleInfo("Plane", 800.0, 0.50);
    }

    static bool isValidCityName(const std::string &name) {
        if (name.empty()) return false;

        for (char c : name) {
            if (!std::isalpha(static_cast<unsigned char>(c)) && c != ' ') {
                return false;
            }
        }
        return true;
    }


    string normalize(string s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        size_t end = s.find_last_not_of(" \t\r\n");
        if (start == string::npos) return "";
        s = s.substr(start, end - start + 1);
        for (size_t i = 0; i < s.length(); i++) {
            if (s[i] == ' ') s[i] = '_';
            s[i] = tolower(s[i]);
        }
        return s;
    }

    void addCity(const string& city) {
        string normCity = normalize(city);
        if(!cityExists(normCity)) {
            adjList[normCity] = {}; // Add empty adjacency list
        }
    }


    void addRoad(string city1, string city2, double distance, VehicleType vehicle, int traffic = 5, double toll = 0.0) {
        city1 = normalize(city1);
        city2 = normalize(city2);
        adjList[city1].push_back(Road(city2, distance, vehicle, traffic, toll));
        adjList[city2].push_back(Road(city1, distance, vehicle, traffic, toll));
    }

    // Close road and detect if already closed
    bool closeRoad(string city1, string city2, bool &wasAlreadyClosed) {
        city1 = normalize(city1);
        city2 = normalize(city2);
        bool found = false;
        wasAlreadyClosed = true; // assume already closed

        // Close road from city1 → city2
        for (auto &road : adjList[city1]) {
            if (road.destination == city2) {
                if (road.isOpen) {        // only change if it was open
                    road.isOpen = false;
                    wasAlreadyClosed = false; // we actually closed it
                }
                found = true;
            }
        }

        // Close road from city2 → city1
        for (auto &road : adjList[city2]) {
            if (road.destination == city1) {
                if (road.isOpen) {
                    road.isOpen = false;
                    wasAlreadyClosed = false;
                }
                found = true;
            }
        }

        return found; // true if road exists
    }



    bool reopenRoad(string city1, string city2, bool &wasAlreadyOpen) {
        city1 = normalize(city1);
        city2 = normalize(city2);
        bool found = false;
        wasAlreadyOpen = true; // assume already open

        // Reopen road from city1 → city2
        for (auto &road : adjList[city1]) {
            if (road.destination == city2) {
                if (!road.isOpen) {       // only change if it was closed
                    road.isOpen = true;
                    wasAlreadyOpen = false; // we changed it
                }
                found = true;
            }
        }

        // Reopen road from city2 → city1
        for (auto &road : adjList[city2]) {
            if (road.destination == city1) {
                if (!road.isOpen) {
                    road.isOpen = true;
                    wasAlreadyOpen = false;
                }
                found = true;
            }
        }

        return found; // true if road exists
    }


    // Struct to hold closed road info
    struct ClosedRoadInfo {
        std::string city1;
        std::string city2;
        std::string vehicle; // vehicle name
    };

    // Method to get all closed roads
    std::vector<ClosedRoadInfo> getClosedRoads() {
        std::vector<ClosedRoadInfo> closed;

        for (const auto& p : adjList) {
            const std::string& city1 = p.first;
            for (const auto& road : p.second) {
                // Only list roads in one direction to avoid duplicates
                if (!road.isOpen && city1 < road.destination) {
                    std::string vehicleName;
                    switch (road.vehicle) {
                        case CAR: vehicleName = "Car"; break;
                        case BIKE: vehicleName = "Bike"; break;
                        case TRAIN: vehicleName = "Train"; break;
                        case PLANE: vehicleName = "Plane"; break;
                    }
                    closed.push_back({city1, road.destination, vehicleName});
                }
            }
        }
        return closed;
    }


    bool loadFromFile(string filename) {
        ifstream file(filename);
        if (!file) return false;

        string line;
        while (getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            stringstream ss(line);
            string c1, c2, vehStr;
            double dist;
            ss >> c1 >> c2 >> dist >> vehStr;
            addRoad(c1, c2, dist, stringToVehicle(vehStr));
        }
        file.close();
        return true;
    }

    bool cityExists(string city) {
        return adjList.find(normalize(city)) != adjList.end();
    }

    RouteResult findShortestPath(string start, string end, VehicleType vehicle) {
        start = normalize(start);
        end = normalize(end);
        RouteResult result;

        if (adjList.find(start) == adjList.end() || adjList.find(end) == adjList.end())
            return result;

        map<string, double> dist;
        map<string, string> parent;
        for (auto &p : adjList) dist[p.first] = INF;
        dist[start] = 0;

        priority_queue<pair<double, string>, vector<pair<double, string>>, greater<>> pq;
        pq.push({0, start});

        while (!pq.empty()) {
            auto [d, u] = pq.top(); pq.pop();
            if (d > dist[u]) continue;
            if (u == end) break;

            for (auto &road : adjList[u]) {
                if (!road.isOpen || road.vehicle != vehicle) continue;
                string v = road.destination;
                double newDist = dist[u] + road.distance;
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    parent[v] = u;
                    pq.push({newDist, v});
                }
            }
        }

        if (dist[end] == INF) return result;

        string at = end;
        while (!at.empty()) {
            result.path.push_back(at);
            if (at == start) break;
            at = parent[at];
        }
        reverse(result.path.begin(), result.path.end());

        result.totalDistance = dist[end];
        result.totalTime = dist[end] / vehicles[vehicle].speed;
        result.totalCost = dist[end] * vehicles[vehicle].costPerKm;
        result.found = true;

        return result;
    }

    // At the end of public section
    const std::map<std::string, std::vector<Road>>& getAdjList() const {
        return adjList;
    }
};

// RouteInfo struct (outside the class)
struct RouteInfo {
    std::string from, to;
    double distance;
    int vehicle;
    bool isOpen;
};

// Free function to get cities
std::vector<std::string> getCities(const TrafficGraph& graph) {
    std::vector<std::string> cities;
    for (const auto& p : graph.getAdjList())
        cities.push_back(p.first);
    return cities;
}

// Free function to get all routes
std::vector<RouteInfo> getAllRoutes(const TrafficGraph& graph) {
    std::vector<RouteInfo> routes;
    for (const auto& p : graph.getAdjList()) {
        std::string city = p.first;
        for (const auto& r : p.second) {
            if (city < r.destination) { // avoid duplicates
                routes.push_back({city, r.destination, r.distance, static_cast<int>(r.vehicle), r.isOpen});
            }
        }
    }
    return routes;
}


int main() {
    TrafficGraph graph;

    cout << "\nInitializing Traffic Management System...\n";

    if (!graph.loadFromFile("routes.txt")) {
        cout << "Failed to load routes.txt\n";
        return 1;
    }

    crow::SimpleApp app;

    CROW_ROUTE(app, "/shortest/<string>/<string>/<int>")([&graph](string start, string end, int vehicleType){
    crow::json::wvalue res;
    VehicleType vehicle = static_cast<VehicleType>(vehicleType);
    RouteResult result = graph.findShortestPath(start, end, vehicle);

    res["found"] = result.found;
    auto &pathList = res["path"];
    int idx = 0;
    for (auto &city : result.path)
        pathList[idx++] = city;

    res["totalDistance"] = result.totalDistance;
    res["totalTime"] = result.totalTime;
    res["totalCost"] = result.totalCost;

    crow::response r(res);
    r.add_header("Access-Control-Allow-Origin", "*"); // <-- add this line
    r.add_header("Content-Type", "application/json");
    return r;
});

    // --- Add a new city ---
    CROW_ROUTE(app, "/add_city/<string>")
  ([&graph](const std::string& cityName) {
      crow::json::wvalue res;

      // Normalize input
      std::string normCity = graph.normalize(cityName);

      // Validation: only letters and spaces
      if (!TrafficGraph::isValidCityName(cityName)) {
          res["success"] = false;
          res["message"] = "Invalid city name!";
      }
      // Check if city already exists
      else if (graph.cityExists(normCity)) {
          res["success"] = false;
          res["message"] = "City already exists!";
      }
      // Add new city
      else {
          graph.addCity(normCity);
          res["success"] = true;
          res["message"] = "City added successfully!";
      }

      crow::response r(res);
      r.add_header("Access-Control-Allow-Origin", "*");
      r.add_header("Content-Type", "application/json");
      return r; // ✅ must return crow::response
  });




    // --- Add new route ---
    CROW_ROUTE(app, "/add_route/<string>/<string>/<double>/<string>")
    ([&graph](string city1, string city2, double distance, string vehicleStr){

        crow::json::wvalue res;

        VehicleType vehicle;
        string v = vehicleStr;
        transform(v.begin(), v.end(), v.begin(), ::tolower);

        if (v == "car") vehicle = CAR;
        else if (v == "bike") vehicle = BIKE;
        else if (v == "train") vehicle = TRAIN;
        else if (v == "plane") vehicle = PLANE;
        else {
            res["success"] = false;
            res["message"] = "Invalid vehicle type!";
            crow::response r(res);
            r.add_header("Access-Control-Allow-Origin", "*");
            return r;
        }

        string n1 = graph.normalize(city1);
        string n2 = graph.normalize(city2);

        if (!graph.cityExists(n1) || !graph.cityExists(n2)) {
            res["success"] = false;
            res["message"] = "One or both cities do not exist!";
        } else {
            graph.addRoad(n1, n2, distance, vehicle);
            res["success"] = true;
            res["message"] = "Route added successfully!";
        }

        crow::response r(res);
        r.add_header("Access-Control-Allow-Origin", "*");
        return r;
    });






    // --- List all cities ---
    CROW_ROUTE(app, "/cities")([&graph](){
        crow::json::wvalue res;
        auto cities = getCities(graph);

        for (size_t i = 0; i < cities.size(); i++)
            res[i] = cities[i];

        crow::response r(res);
        r.add_header("Access-Control-Allow-Origin", "*");
        r.add_header("Content-Type", "application/json");
        return r;
    });


    // --- List all routes ---
    CROW_ROUTE(app, "/routes")([&graph](){
    crow::json::wvalue res;
    auto routes = getAllRoutes(graph);

    for (size_t i = 0; i < routes.size(); i++) {
        res[i]["from"] = routes[i].from;
        res[i]["to"] = routes[i].to;
        res[i]["distance"] = routes[i].distance;

        string vehicleName;
        switch (routes[i].vehicle) {
            case 0: vehicleName = "Car"; break;
            case 1: vehicleName = "Bike"; break;
            case 2: vehicleName = "Train"; break;
            case 3: vehicleName = "Plane"; break;
        }
        res[i]["vehicle"] = vehicleName;
        res[i]["isOpen"] = routes[i].isOpen; // <-- fix
    }

    crow::response r(res);
    r.add_header("Access-Control-Allow-Origin", "*");
    r.add_header("Content-Type", "application/json");
    return r;
});


    // ----close a road----//
    CROW_ROUTE(app, "/closeRoad").methods("POST"_method)([&graph](const crow::request& req){
    auto body = crow::json::load(req.body);
    crow::json::wvalue res;
    if (!body) return crow::response(400, "Invalid JSON");

    std::string city1 = body["city1"].s();
    std::string city2 = body["city2"].s();

    if (!graph.cityExists(city1) || !graph.cityExists(city2)) {
        res["status"] = "error";
        res["msg"] = "One or both cities do not exist!";
    } else {
        bool alreadyClosed;
        bool exists = graph.closeRoad(city1, city2, alreadyClosed);
        if (!exists) {
            res["status"] = "error";
            res["msg"] = "Road not found between specified cities";
        } else if (alreadyClosed) {
            res["status"] = "info";
            res["msg"] = "Road between " + city1 + " and " + city2 + " is already closed";
        } else {
            res["status"] = "success";
            res["msg"] = "Road between " + city1 + " and " + city2 + " closed successfully";
        }
    }

    crow::response r(res);
    r.add_header("Access-Control-Allow-Origin","*");
    r.add_header("Content-Type","application/json");
    return r;
});

       //------ Re Open Road------
    CROW_ROUTE(app, "/reopenRoad").methods("POST"_method)([&graph](const crow::request& req){
    auto body = crow::json::load(req.body);
    crow::json::wvalue res;
    if (!body) return crow::response(400, "Invalid JSON");

    std::string city1 = body["city1"].s();
    std::string city2 = body["city2"].s();

    if (!graph.cityExists(city1) || !graph.cityExists(city2)) {
        res["status"] = "error";
        res["msg"] = "One or both cities do not exist!";
    } else {
        bool alreadyOpen;
        bool exists = graph.reopenRoad(city1, city2, alreadyOpen);
        if (!exists) {
            res["status"] = "error";
            res["msg"] = "Road not found between specified cities";
        } else if (alreadyOpen) {
            res["status"] = "info";
            res["msg"] = "Road between " + city1 + " and " + city2 + " is already open";
        } else {
            res["status"] = "success";
            res["msg"] = "Road between " + city1 + " and " + city2 + " reopened successfully";
        }
    }

    crow::response r(res);
    r.add_header("Access-Control-Allow-Origin","*");
    r.add_header("Content-Type","application/json");
    return r;
});



    // --- View Closed Roads ---
    CROW_ROUTE(app, "/closedRoads")([&graph]() {
        auto closed = graph.getClosedRoads();
        crow::json::wvalue res;

        for (size_t i = 0; i < closed.size(); i++) {
            res[i]["roadName"] = closed[i].city1 + " ↔ " + closed[i].city2;
            res[i]["vehicle"] = closed[i].vehicle;
            res[i]["status"] = "Closed";
        }

        crow::response r(res);
        r.add_header("Access-Control-Allow-Origin", "*");
        r.add_header("Content-Type", "application/json");
        return r;
    });

    CROW_ROUTE(app, "/")([]() {
    std::ifstream file("index.html");
    if (!file.is_open())
        return crow::response(404, "index.html not found");

    std::stringstream buffer;
    buffer << file.rdbuf();
    return crow::response(buffer.str());
});


    // // Serve static files from the same directory
    // app.route_dynamic("/<path>")([](const crow::request& req, std::string path){
    //     std::ifstream file(path);
    //     if(!file.is_open()) return crow::response(404);
    //     std::stringstream buffer;
    //     buffer << file.rdbuf();
    //     return crow::response(buffer.str());
    // });
    // Serve static files from the same directory
    app.route_dynamic("/<path>")([](const crow::request& req, std::string path){
        std::ifstream file(path, std::ios::binary);
        if(!file.is_open()) return crow::response(404);

        std::stringstream buffer;
        buffer << file.rdbuf();

        crow::response res(buffer.str());

        // Set appropriate Content-Type based on file extension
        if (path.find(".mp4") != std::string::npos) {
            res.add_header("Content-Type", "video/mp4");
        } else if (path.find(".css") != std::string::npos) {
            res.add_header("Content-Type", "text/css");
        } else if (path.find(".js") != std::string::npos) {
            res.add_header("Content-Type", "application/javascript");
        } else if (path.find(".html") != std::string::npos) {
            res.add_header("Content-Type", "text/html");
        } else if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos) {
            res.add_header("Content-Type", "image/jpeg");
        } else if (path.find(".png") != std::string::npos) {
            res.add_header("Content-Type", "image/png");
        }

        return res;
    });

    cout << "Server started at http://127.0.0.1:18080\n";
    app.port(18080).multithreaded().run();

    return 0;
}
