#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

class SolutionReader {
public:
    explicit SolutionReader(const std::string& filePath) {
        parseFile(filePath);
    }

    const Solution& getSolution() const {
        return solution;
    }

private:
    Solution solution;

    void parseFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("No se pudo abrir el archivo: " + filePath);
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            Ruta ruta;
            int nodo;
            while (iss >> nodo) {
                ruta.nodos.push_back(nodo);
            }
            // Ejemplo: Agregar demanda y costo (puedes ajustar según el formato del archivo)
            ruta.demanda_total = ruta.nodos.size(); // Ejemplo: demanda como número de nodos
            ruta.costo_total = ruta.nodos.size() * 10.0; // Ejemplo: costo ficticio
            solucion.rutas.push_back(ruta);
        }

        file.close();
    }
};