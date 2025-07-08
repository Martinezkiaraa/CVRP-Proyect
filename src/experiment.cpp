#include "VRPLIBReader.h"
#include "solution.h"
#include "local_search.h"
#include <fstream>
#include <chrono>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include "VRPLIBReader.h"
#include "clarke_wright.h"
#include "nearest_neighbor.h"
#include "local_search.h"
#include "SolutionReader.h"

void escribirTablaConsola(const std::vector<std::vector<std::string>>& filas, const std::string& titulo) {
    std::cout << "\n" << titulo << "\n";
    std::cout << std::left << std::setw(25) << "Método"
              << std::setw(10) << "Costo"
              << std::setw(8) << "Rutas"
              << std::setw(15) << "Mejora vs Óptimo" << "\n";
    std::cout << std::string(58, '-') << "\n";
    for (const auto& fila : filas) {
        for (const auto& val : fila) {
            std::cout << std::left << std::setw(25) << val;
        }
        std::cout << "\n";
    }
}

void guardarCSV(const std::string& filename, const std::vector<std::vector<std::string>>& filas) {
    std::ofstream file(filename);
    file << "instancia,metodo,costo,rutas,mejora_vs_optimo\n";
    for (const auto& fila : filas) {
        for (size_t i = 0; i < fila.size(); ++i) {
            file << fila[i];
            if (i < fila.size() - 1) file << ",";
        }
        file << "\n";
    }
    file.close();
}

int main() {
    struct Caso {
        std::string nombre;
        std::string path;
        double optimo;
    } casos[] = {
        {"E045-04f", "instancias/2l-cvrp-0/E045-04f.dat", 723.54},
        {"E076-08u", "instancias/2l-cvrp-0/E076-08u.dat", 742.582}
    };

    std::vector<std::vector<std::string>> acumulado_csv;

    for (const auto& caso : casos) {
        VRPLIBReader reader(caso.path);
        std::vector<std::vector<std::string>> resultados;

        for (const std::string& heuristica : {"CW", "NN"}) {
            Solution base;
            if (heuristica == "CW"){
                ClarkeWrightSolver cw_solver(reader);
                base = cw_solver.construirSolucion();
            }
            if (heuristica == "NN") {
                NearestNeighborSolver nn_solver(reader);
                base = nn_solver.construirSolucion();
            }

            for (const std::string& mejora : {"none", "relocate", "swap", "hibrido"}) {
                Solution sol = base;
                if (mejora == "relocate") while (LocalSearch::relocate(sol, reader));
                if (mejora == "swap") while (LocalSearch::swap(sol, reader));
                if (mejora == "hibrido") LocalSearch::mejorarSolucion(sol, reader);

                sol.actualizarCostoTotal(reader.getDistanceMatrix());
                double costo = sol.getCostoTotal();
                int rutas = sol.cantidadRutas();
                double mejora_vs_optimo = 100.0 * (costo - caso.optimo) / caso.optimo;

                std::string nombre_metodo = heuristica == "CW" ? "Clarke & Wright" : "Nearest Neighbor";
                if (mejora == "relocate") nombre_metodo += " + Relocate";
                if (mejora == "swap") nombre_metodo += " + Swap";
                if (mejora == "hibrido") nombre_metodo += " + Híbrido";

                std::string mejora_str = (mejora_vs_optimo >= 0 ? "+" : "") + std::to_string(mejora_vs_optimo).substr(0, 4) + "%";

                resultados.push_back({
                    nombre_metodo,
                    std::to_string(costo),
                    std::to_string(rutas),
                    mejora_str
                });

                acumulado_csv.push_back({
                    caso.nombre,
                    nombre_metodo,
                    std::to_string(costo),
                    std::to_string(rutas),
                    mejora_str
                });
            }
        }

        resultados.push_back({"Óptimo", std::to_string(caso.optimo), "-", "0 %"});
        escribirTablaConsola(resultados, "Cuadro: Resultados para instancia " + caso.nombre);
    }

    guardarCSV("resultados_experimento.csv", acumulado_csv);
    std::cout << "\n✔ Resultados exportados a resultados_experimento.csv\n";

    return 0;
}
