#ifndef VRPLIB_READER_H
#define VRPLIB_READER_H

#include <string>
#include <vector>

// A structure to represent a node (customer or depot)
struct Node {
    int id;
    double x;
    double y;
};

class VRPLIBReader {
public:
    // Constructor that takes the path to the VRPLIB file
    explicit VRPLIBReader(const std::string& filePath);

    // --- Getter methods to access the parsed data ---

    const std::string& getName() const;
    int getDimension() const;
    int getCapacity() const;
    int getNumVehicles() const;
    const std::vector<Node>& getNodes() const;
    const std::vector<int>& getDemands() const;
    int getDepotId() const;
    const std::vector<std::vector<double>>& getDistanceMatrix() const;

private:
    // --- Member variables to store instance data ---

    std::string name;
    int dimension {0};
    int capacity {0};
    int numVehicles {0}; // Note: Some instances might not specify this
    int depotId {0};
    std::vector<Node> nodes;
    std::vector<int> demands;
    std::vector<std::vector<double>> distanceMatrix;

    // --- Private helper methods for parsing and computation ---

    // Main parsing method, called by the constructor
    void parse(const std::string& filePath);

    // Computes the Euclidean distance matrix after parsing node coordinates
    void computeDistanceMatrix();
};

#endif // VRPLIB_READER_H