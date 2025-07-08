#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "VRPLIBReader.h"
#include "clarke_wright.h"
#include "nearest_neighbor.h"
#include "local_search.h"
#include "SolutionReader.h"
#include "initial_methods.h"
#include "GRASP.h"

// Función para obtener el nombre del archivo según el algoritmo
std::string obtenerArchivoResultados(int heuristica) {
    switch (heuristica) {
        case 1:
            return "soluciones/resultados_clarke_wright.txt";
        case 2:
            return "soluciones/resultados_nearest_neighbor.txt";
        case 3:
            return "soluciones/resultados_initial_methods.txt";
        case 4:
            return "soluciones/resultados_grasp.txt";
        default:
            return "soluciones/resultados_generico.txt";
    }
}

// Función para agregar resultado al archivo
void agregarResultadoAlArchivo(const std::string& archivo, const std::string& instancia, double costoHeuristica, 
                              double error) {
    std::string lineaNueva = instancia + "|" + 
                            std::to_string(costoHeuristica) + "|" + 
                            std::to_string(error);
    
    // Crear directorio si no existe
    size_t last_slash = archivo.find_last_of("/");
    if (last_slash != std::string::npos) {
        std::string directorio = archivo.substr(0, last_slash);
        // Crear directorio si no existe (compatible con C++11)
        std::string comando = "mkdir -p " + directorio;
        system(comando.c_str());
    }
    
    // Agregar línea al final del archivo (crea el archivo si no existe)
    std::ofstream archivoSalida(archivo, std::ios::app);
    if (archivoSalida.is_open()) {
        archivoSalida << lineaNueva << std::endl;
        archivoSalida.close();
        std::cout << "Resultado guardado en: " << archivo << std::endl;
    } else {
        std::cerr << "Error: No se pudo abrir/crear el archivo: " << archivo << std::endl;
    }
}

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

#include <iostream>
#include "solution.h"
#include "VRPLIBReader.h"

bool validarSolucion(const Solution& sol, const VRPLIBReader& reader) {
    int n = reader.getDimension();
    int depot = reader.getDepotId(); // ej: 1
    int capacidad = reader.getCapacity();
    const std::vector<int>& demandas = reader.getDemands();

    // Vector para marcar visitados, índices de 1 a n, ignoramos índice 0
    std::vector<bool> visitados(n + 1, false);

    // Marcar depósito como visitado (no es cliente)
    visitados[depot] = true;

    for (const auto& ruta : sol.getRutas()) {
        // Validar que la ruta no esté vacía y empieza/termina en depósito
        if (ruta.secuencia.empty() || ruta.secuencia.front() != depot || ruta.secuencia.back() != depot) {
            std::cout << "[ERROR] Ruta " << ruta.id << " no comienza o termina en depósito.\n";
            return false;
        }

        int demanda_acumulada = 0;

        for (size_t i = 1; i < ruta.secuencia.size() - 1; ++i) {
            int nodo = ruta.secuencia[i];

            // Validar nodo válido
            if (nodo < 1 || nodo > n) {
                std::cout << "[ERROR] Nodo inválido " << nodo << " en ruta " << ruta.id << ".\n";
                return false;
            }

            // Validar no repetido
            if (visitados[nodo]) {
                std::cout << "[ERROR] Nodo " << nodo << " visitado más de una vez.\n";
                return false;
            }
            visitados[nodo] = true;

            // Acumular demanda
            demanda_acumulada += demandas[nodo];
        }

        // Validar capacidad no excedida
        if (demanda_acumulada > capacidad) {
            std::cout << "[ERROR] Ruta " << ruta.id << " supera capacidad. Demanda: " 
                      << demanda_acumulada << " > Capacidad: " << capacidad << "\n";
            return false;
        }
    }

    // Verificar que todos los clientes hayan sido visitados (todos excepto depósito)
    for (int nodo = 1; nodo <= n; ++nodo) {
        if (!visitados[nodo]) {
            std::cout << "[ERROR] Cliente " << nodo << " no fue visitado.\n";
            return false;
        }
    }

    std::cout << "Solución válida: todas las restricciones se cumplen.\n";
    return true;
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
        std::cout << "\nSeleccione heuristica constructiva:" << std::endl;
        std::cout << "1) Clarke & Wright" << std::endl;
        std::cout << "2) Vecino mas cercano" << std::endl;
        std::cout << "3) Metodo Inicial" << std::endl;
        std::cout << "4) GRASP" << std::endl;
        int heuristica = 0;
        while (heuristica != 1 && heuristica != 2 && heuristica != 3 && heuristica != 4) {
            std::cout << "Opcion (1/2/3/4): ";
            std::cin >> heuristica;
        }

        Solution sol;
        if (heuristica == 1) {
            ClarkeWrightSolver cw_solver(reader);
            sol = cw_solver.construirSolucion();
        } else if (heuristica == 2) {
            NearestNeighborSolver nn_solver(reader);
            sol = nn_solver.construirSolucion();
        } else if (heuristica == 3) {
            sol = initial_methods(reader);
        } else if (heuristica == 4) {
            // GRASP
            std::cout << "Ingrese el tamaño de RCL (1-10): ";
            int rcl_size;
            std::cin >> rcl_size;
            if (rcl_size < 1) rcl_size = 1;
            if (rcl_size > 10) rcl_size = 10;
            
            sol = GRASP::ejecutarGRASP(reader, rcl_size);
            
            
        }

        if(heuristica!=4){// Elegir búsqueda local
            std::cout << "\n¿Desea aplicar busqueda local?" << std::endl;
            std::cout << "0) No aplicar" << std::endl;
            std::cout << "1) Relocate" << std::endl;
            std::cout << "2) Swap" << std::endl;
            std::cout << "3) Ambas (Relocate + Swap)" << std::endl;
            int opcion_bl = -1;
            while (opcion_bl < 0 || opcion_bl > 3) {
                std::cout << "Opcion (0/1/2/3): ";
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
        }    
        bool es_valida = validarSolucion(sol, reader);
            if (!es_valida) {
                std::cout << "La solución no es válida.\n";
            }

        std::cout << "=== Solucion encontrada ===" << std::endl; 
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
                std::cout << "\n=== Solucion optima ===" << std::endl;
                std::cout << "Nombre: " << sol_reader.getName() << std::endl;
                std::cout << "Comentario: " << sol_reader.getComment() << std::endl;
                std::cout << "Tipo: " << sol_reader.getType() << std::endl;
                std::cout << "Numero de rutas: " << sol_reader.getNumRoutes() << std::endl;
                std::cout << "Costo total: " << sol_reader.getTotalCost() << std::endl;
                std::cout << "\nRutas optimas:" << std::endl;
                sol_reader.getSolution().imprimir();
                std::cout << "\nComparacion de costos:" << std::endl;
                std::cout << "  Costo de solucion heuristica: " << sol.getCostoTotal() << std::endl;
                std::cout << "  Costo real:       " << sol_reader.getTotalCost() << std::endl;

                if(sol.cantidadRutas() > sol_reader.getNumRoutes()){
                    std::cout << "\n=== Comparacion de rutas ===" << std::endl;
                    std::cout << "La solucion encontrada tiene mas vehiculos que el numero de vehiculos permitidos" << std::endl;
                }else{
                    // Determinar archivo según el algoritmo
                    std::string archivo_resultados = obtenerArchivoResultados(heuristica);
                                    
                    // Guardar resultado en el archivo correspondiente
                    agregarResultadoAlArchivo(archivo_resultados, nombre_base, sol.getCostoTotal(), 
                                            sol.getCostoTotal() - sol_reader.getTotalCost());

                    std::cout << "Resultado guardado en: " << archivo_resultados << std::endl;
                }
                
            } catch (const std::exception& e) {
                std::cerr << "No se pudo leer la solucion real: " << e.what() << std::endl;
            }
        } else {
            std::cout << "\nNo se encontro solucion real para comparar en: " << solucion_path << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
} 