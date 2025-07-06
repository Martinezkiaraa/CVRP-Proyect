#include <iostream>
#include <string>
#include <vector>
#include "VRPLIBReader.h"
#include "clarke_wright.h"
#include "nearest_neighbor.h"
#include "local_search.h"
#include "SolutionReader.h"

void imprimirSolucion(const Solution& sol) {
    std::cout << "\nCosto total: " << sol.getCostoTotal() << std::endl;
    std::cout << "Cantidad de rutas: " << sol.cantidadRutas() << std::endl;
    const auto& rutas = sol.getRutas();
    for (size_t i = 0; i < rutas.size(); ++i) {
        std::cout << "Ruta " << (i+1) << ": ";
        for (int nodo : rutas[i].secuencia) {
            std::cout << nodo << " ";
        }
        std::cout << "| Demanda: " << rutas[i].suma_demanda << " | Costo: " << rutas[i].costo << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Uso: " << argv[0] << " <archivo_instancia>" << std::endl;
        return 1;
    }

    try {
        VRPLIBReader reader(argv[1]);
        std::cout << "Instancia: " << reader.getName() << std::endl;
        std::cout << "Clientes: " << reader.getNodes().size() << std::endl;
        std::cout << "Capacidad: " << reader.getCapacity() << std::endl;

        // Elegir heurística constructiva
        std::cout << "\nSeleccione heurística constructiva:" << std::endl;
        std::cout << "1) Clarke & Wright" << std::endl;
        std::cout << "2) Vecino más cercano" << std::endl;
        int heuristica = 0;
        while (heuristica != 1 && heuristica != 2) {
            std::cout << "Opción (1/2): ";
            std::cin >> heuristica;
        }

        Solution sol;
        if (heuristica == 1) {
            ClarkeWrightSolver cw_solver(reader);
            sol = cw_solver.construirSolucion();
        } else {
            NearestNeighborSolver nn_solver(reader);
            sol = nn_solver.construirSolucion();
        }

        // Elegir búsqueda local
        std::cout << "\n¿Desea aplicar búsqueda local?" << std::endl;
        std::cout << "0) No aplicar" << std::endl;
        std::cout << "1) Relocate" << std::endl;
        std::cout << "2) Swap" << std::endl;
        std::cout << "3) Ambas (Relocate + Swap)" << std::endl;
        int opcion_bl = -1;
        while (opcion_bl < 0 || opcion_bl > 3) {
            std::cout << "Opción (0/1/2/3): ";
            std::cin >> opcion_bl;
        }

        bool mejora = true;
        int iter = 0;
        const int max_iter = 200;
        if (opcion_bl == 1) {
            // Relocate
            while (mejora && iter < max_iter) {
                mejora = LocalSearch::relocate(sol, reader);
                iter++;
            }
        } else if (opcion_bl == 2) {
            // Swap
            while (mejora && iter < max_iter) {
                mejora = LocalSearch::swap(sol, reader);
                iter++;
            }
        } else if (opcion_bl == 3) {
            // mejorarSolucion() aplica relocate y swap alternadamente
            LocalSearch::mejorarSolucion(sol, reader, max_iter);
        }

        imprimirSolucion(sol);
        
        // Imprime (si es que existe) la solucion real 
        std::string instancia_path(argv[1]);
        size_t last_slash = instancia_path.find_last_of("/");
        std::string carpeta = instancia_path.substr(0, last_slash); // carpeta de la instancia
        std::string nombre_archivo = instancia_path.substr(last_slash + 1);
        // Quitar extensión .dat o .DAT
        size_t last_dot = nombre_archivo.find_last_of('.');
        std::string nombre_base = (last_dot != std::string::npos) ? nombre_archivo.substr(0, last_dot) : nombre_archivo;
        // Buscar solución con extensión .HRE (mayúscula)
        std::string solucion_path = carpeta + "/soluciones/" + nombre_base + ".HRE";
        // Intentar abrir la solución real
        std::ifstream test_file(solucion_path);
        if (test_file.good()) {
            try {
                SolutionReader sol_reader(solucion_path);
                std::cout << "\n=== Solución óptima ===" << std::endl;
                std::cout << "Nombre: " << sol_reader.getName() << std::endl;
                std::cout << "Comentario: " << sol_reader.getComment() << std::endl;
                std::cout << "Tipo: " << sol_reader.getType() << std::endl;
                std::cout << "Número de rutas: " << sol_reader.getNumRoutes() << std::endl;
                std::cout << "Costo total: " << sol_reader.getTotalCost() << std::endl;
                std::cout << "\nRutas óptimas:" << std::endl;
                sol_reader.getSolution().imprimir();
                std::cout << "\nComparación de costos:" << std::endl;
                std::cout << "  Costo heurística: " << sol.getCostoTotal() << std::endl;
                std::cout << "  Costo real:       " << sol_reader.getTotalCost() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "No se pudo leer la solución real: " << e.what() << std::endl;
            }
        } else {
            std::cout << "\nNo se encontró solución real para comparar en: " << solucion_path << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
} 