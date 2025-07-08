#include "SolutionReader.h"
#include <stdexcept>

SolutionReader::SolutionReader(const std::string& filePath) {
    parseFile(filePath);
}

const Solution& SolutionReader::getSolution() const {
    return solution;
}

std::string SolutionReader::getName() const {
    return name;
}

std::string SolutionReader::getComment() const {
    return comment;
}

std::string SolutionReader::getType() const {
    return type;
}

int SolutionReader::getNumRoutes() const {
    return solution.getNumRutas();
}

double SolutionReader::getTotalCost() const {
    return solution.getCostoTotal();
}

int SolutionReader::getDepot() const {
    return solution.getDepot();
}

void SolutionReader::parseFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo: " + filePath);
    }

    std::string line;
    bool inSolutionSection = false;
    bool inDepotSection = false;

    while (std::getline(file, line)) {
        // Eliminar espacios en blanco al inicio y final
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty()) continue;

        if (line == "SOLUTION_SECTION") {
            inSolutionSection = true;
            continue;
        }

        if (line == "DEPOT_SECTION") {
            inSolutionSection = false;
            inDepotSection = true;
            continue;
        }

        if (line == "END") {
            break;
        }

        if (inSolutionSection) {
            // Verificar si es la línea de encabezado de rutas
            if (line.find("#R") != std::string::npos) {
                continue; // Saltar línea de encabezado
            }
            
            // Parsear línea de ruta
            Ruta ruta = parseRouteLine(line);
            solution.agregarRuta(ruta);
        } else if (inDepotSection) {
            parseDepotSection(file);
            break;
        } else {
            // Parsear información del header
            parseHeader(line);
        }
    }

    file.close();
}

void SolutionReader::parseHeader(const std::string& line) {
    std::istringstream iss(line);
    std::string key, value;
    
    if (iss >> key) {
        if (key == "NAME") {
            std::getline(iss, value);
            // Eliminar espacios al inicio
            value.erase(0, value.find_first_not_of(" \t"));
            name = value;
        } else if (key == "COMMENT") {
            std::getline(iss, value);
            value.erase(0, value.find_first_not_of(" \t"));
            comment = value;
        } else if (key == "TYPE") {
            std::getline(iss, value);
            value.erase(0, value.find_first_not_of(" \t"));
            type = value;
        } else if (key == "ROUTES") {
            iss >> num_routes;
            solution.setNumRutas(num_routes);
        } else if (key == "COST") {
            iss >> total_cost;
            solution.setCostoTotal(total_cost);
        }
    }
}

void SolutionReader::parseSolutionSection(std::ifstream& file) {
    std::string line;
    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty()) continue;
        if (line == "DEPOT_SECTION") break;

        // Verificar si es la línea de encabezado de rutas
        if (line.find("#R") != std::string::npos) {
            continue; // Saltar línea de encabezado
        }

        // Parsear línea de ruta
        Ruta ruta = parseRouteLine(line);
        solution.agregarRuta(ruta);
    }
}

void SolutionReader::parseDepotSection(std::ifstream& file) {
    std::string line;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> depot;
        solution.setDepot(depot);
    }
}

Ruta SolutionReader::parseRouteLine(const std::string& line) {
    std::istringstream iss(line);
    Ruta ruta;
    
    // Leer los campos en orden: #R SUMD COST LENGTH #C SEQUENCE
    iss >> ruta.id;           // #R
    iss >> ruta.suma_demanda; // SUMD
    iss >> ruta.costo;        // COST
    iss >> ruta.longitud;     // LENGTH
    iss >> ruta.num_clientes; // #C
    
    // Leer la secuencia de nodos y aplicar mapeo de índices
    // En .HRE: nodo 1 = primer cliente, nodo 101 = depósito
    // En .dat:  nodo 1 = depósito, nodo 2 = primer cliente
    int nodo;
    while (iss >> nodo) {
        // Mapeo: HRE -> DAT
        // Si es el depósito (nodo 101 en HRE), mapear a nodo 1 en DAT
        if (nodo == 101) {
            ruta.secuencia.push_back(1); // Depósito
        } else {
            // Clientes: HRE nodo N -> DAT nodo N+1
            ruta.secuencia.push_back(nodo + 1);
        }
    }
    
    return ruta;
}
