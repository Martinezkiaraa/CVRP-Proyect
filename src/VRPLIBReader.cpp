#include "VRPLIBReader.h"
#include <fstream>
#include <stdexcept>
#include <cmath>
#include <sstream>

// Constructor: Initializes the reader and starts the parsing process.
VRPLIBReader::VRPLIBReader(const std::string& filePath) {
    parse(filePath);
}

// Main parsing method
void VRPLIBReader::parse(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file " + filePath);
    }

    std::string line;
    std::string current_section;

    while (getline(file, line)) {
        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;

        if (keyword == "NAME") {
            iss.ignore(2); // Ignore the space and ':'
            std::getline(iss, name);
        } else if (keyword == "DIMENSION") {
            iss.ignore(2);
            iss >> dimension;
        } else if (keyword == "CAPACITY") {
            iss.ignore(2);
            iss >> capacity;
        } else if (keyword == "VEHICLES") { // Optional tag
            iss.ignore(2);
            iss >> numVehicles;
        } else if (keyword == "NODE_COORD_SECTION") {
            current_section = "NODE_COORD_SECTION";
            nodes.reserve(dimension);
        } else if (keyword == "DEMAND_SECTION") {
            current_section = "DEMAND_SECTION";
            demands.resize(dimension + 1, 0); // Nodes are 1-indexed
        } else if (keyword == "DEPOT_SECTION") {
            current_section = "DEPOT_SECTION";
        } else if (keyword == "EOF") {
            break;
        } else if (!keyword.empty() && std::isdigit(keyword[0])) {
            // Data lines start with a number (the node ID)
            std::istringstream data_line_stream(line);
            if (current_section == "NODE_COORD_SECTION") {
                int id;
                double x, y;
                data_line_stream >> id >> x >> y;
                nodes.push_back({id, x, y});
            } else if (current_section == "DEMAND_SECTION") {
                int id, demand;
                data_line_stream >> id >> demand;
                if (id >= 1 && id <= dimension) {
                    demands[id] = demand;
                }
            } else if (current_section == "DEPOT_SECTION") {
                int id;
                data_line_stream >> id;
                if (id != -1) {
                    depotId = id;
                } else {
                    current_section = ""; // End of depot section
                }
            }
        }
    }
    file.close();

    // If numVehicles was not in the file, provide a default upper bound.
    if (numVehicles == 0) {
        numVehicles = dimension > 0 ? dimension - 1 : 0;
    }

    // After parsing all data, compute the distance matrix.
    computeDistanceMatrix();
}

// Computes the Euclidean distance matrix.
void VRPLIBReader::computeDistanceMatrix() {
    if (nodes.empty()) return;

    // Ensure nodes are sorted by ID for consistent matrix access if they weren't in order
    // std::sort(nodes.begin(), nodes.end(), [](const Node& a, const Node& b){ return a.id < b.id; });
    // Note: The above sort is only needed if the file is not guaranteed to list nodes in increasing order of ID.
    // Most VRPLIB instances do, so we'll proceed assuming 1-based indexing corresponds to vector position.

    distanceMatrix.resize(dimension + 1, std::vector<double>(dimension + 1, 0.0));

    for (size_t i = 0; i < nodes.size(); ++i) {
        for (size_t j = i; j < nodes.size(); ++j) {
            double dist = std::sqrt(std::pow(nodes[i].x - nodes[j].x, 2) +
                                    std::pow(nodes[i].y - nodes[j].y, 2));
            // Assumes node IDs are 1-based and contiguous from 1 to dimension.
            distanceMatrix[nodes[i].id][nodes[j].id] = dist;
            distanceMatrix[nodes[j].id][nodes[i].id] = dist;
        }
    }
}

// --- Getter Implementations ---

const std::string& VRPLIBReader::getName() const { return name; }
int VRPLIBReader::getDimension() const { return dimension; }
int VRPLIBReader::getCapacity() const { return capacity; }
int VRPLIBReader::getNumVehicles() const { return numVehicles; }
const std::vector<Node>& VRPLIBReader::getNodes() const { return nodes; }
const std::vector<int>& VRPLIBReader::getDemands() const { return demands; }
int VRPLIBReader::getDepotId() const { return depotId; }
const std::vector<std::vector<double>>& VRPLIBReader::getDistanceMatrix() const { return distanceMatrix; }