#ifndef SOLUTION_READER_H
#define SOLUTION_READER_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "solution.h"

class SolutionReader {
public:
    explicit SolutionReader(const std::string& filePath);
    
    const Solution& getSolution() const;
    std::string getName() const;
    std::string getComment() const;
    std::string getType() const;
    int getNumRoutes() const;
    double getTotalCost() const;
    int getDepot() const;

private:
    Solution solution;
    std::string name;
    std::string comment;
    std::string type;
    int num_routes;
    double total_cost;
    int depot;

    void parseFile(const std::string& filePath);
    void parseHeader(const std::string& line);
    void parseSolutionSection(std::ifstream& file);
    void parseDepotSection(std::ifstream& file);
    Ruta parseRouteLine(const std::string& line);
};

#endif