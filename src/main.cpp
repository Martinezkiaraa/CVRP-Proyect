#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include "VRPLIBReader.h"
#include "clarke_wright.h"
#include "nearest_neighbor.h"
#include "SolutionReader.h"

// Función para comparar dos soluciones
bool areSolutionsEqual(const Solution& sol1, const Solution& sol2) {
    // Compara el número de rutas
    if (sol1.cantidadRutas() != sol2.cantidadRutas()) {
        std::cout << "Diferencia en número de rutas: " << sol1.cantidadRutas() << " vs " << sol2.cantidadRutas() << std::endl;
        return false;
    }
    
    // Compara el costo total
    double costo1 = sol1.calcularCostoTotal();
    double costo2 = sol2.calcularCostoTotal();
    if (std::abs(costo1 - costo2) > 0.001) {
        std::cout << "Diferencia en costo total: " << costo1 << " vs " << costo2 << std::endl;
        return false;
    }
    
    // Compara las rutas individuales
    const auto& rutas1 = sol1.getRutas();
    const auto& rutas2 = sol2.getRutas();
    
    for (size_t i = 0; i < rutas1.size(); ++i) {
        if (rutas1[i].secuencia != rutas2[i].secuencia) {
            std::cout << "Diferencia en ruta " << i + 1 << std::endl;
            return false;
        }
    }
    
    return true;
}

// Función para convertir el nombre de la instancia al formato de solución
std::string convertInstanceNameToSolutionName(const std::string& instanceName) {
    std::string solutionName = instanceName;
    // Eliminar todos los espacios en blanco
    solutionName.erase(remove_if(solutionName.begin(), solutionName.end(), ::isspace), solutionName.end());

    // Buscar el punto antes de la extensión
    size_t dotPos = solutionName.find_last_of('.');
    std::string namePart = (dotPos != std::string::npos) ? solutionName.substr(0, dotPos) : solutionName;
    // Buscar la última letra alfabética antes del punto y convertirla a mayúscula
    for (size_t i = namePart.size(); i-- > 0;) {
        if (std::isalpha(namePart[i])) {
            namePart[i] = std::toupper(namePart[i]);
            break;
        }
    }
    return namePart;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_vrp_file>" << std::endl;
        std::cerr << "Example: " << argv[0] << " ../instancias/2l-cvrp-0/E045-04f.dat" << std::endl;
        return 1;
    }

    try {
        // -------------------------------------
        // 1. CARGAR INSTANCIA
        // -------------------------------------
        std::cout << "=== CARGANDO INSTANCIA ===" << std::endl;
        VRPLIBReader reader(argv[1]);

        std::cout << "Instance Name: " << reader.getName() << std::endl;
        std::cout << "Dimension: " << reader.getDimension() << std::endl;
        std::cout << "Number of Vehicles: " << reader.getNumVehicles() << std::endl;
        std::cout << "Capacity: " << reader.getCapacity() << std::endl;
        std::cout << "Depot ID: " << reader.getDepotId() << std::endl;

        std::vector<Node> clients = reader.getNodes();
        std::cout << "Number of clients: " << clients.size() << std::endl;

        // -------------------------------------
        // 2. HEURÍSTICA CONSTRUCTIVA: Clarke & Wright
        // -------------------------------------
        std::cout << "\n=== CLARKE & WRIGHT ===" << std::endl;
        ClarkeWrightSolver solver(reader);
        Solution sol_cw = solver.construirSolucion();

        std::cout << "Solución Clarke & Wright:" << std::endl;
        sol_cw.imprimir();
        std::cout << "Costo total: " << sol_cw.calcularCostoTotal() << std::endl;
        std::cout << "Número de rutas: " << sol_cw.cantidadRutas() << std::endl;

        // -------------------------------------
        // 2.5. HEURÍSTICA CONSTRUCTIVA: Nearest Neighbor
        // -------------------------------------
        std::cout << "\n=== NEAREST NEIGHBOR ===" << std::endl;
        NearestNeighborSolver solver_nn(reader);
        Solution sol_nn = solver_nn.construirSolucion();

        std::cout << "Solución Nearest Neighbor:" << std::endl;
        sol_nn.imprimir();
        std::cout << "Costo total: " << sol_nn.calcularCostoTotal() << std::endl;
        std::cout << "Número de rutas: " << sol_nn.cantidadRutas() << std::endl;

        // -------------------------------------
        // 2.6. COMPARACIÓN ENTRE ALGORITMOS
        // -------------------------------------
        std::cout << "\n=== COMPARACIÓN ENTRE ALGORITMOS ===" << std::endl;
        double costo_cw = sol_cw.calcularCostoTotal();
        double costo_nn = sol_nn.calcularCostoTotal();
        int rutas_cw = sol_cw.cantidadRutas();
        int rutas_nn = sol_nn.cantidadRutas();

        std::cout << "Clarke & Wright:  Costo = " << costo_cw << ", Rutas = " << rutas_cw << std::endl;
        std::cout << "Nearest Neighbor: Costo = " << costo_nn << ", Rutas = " << rutas_nn << std::endl;
        
        if (costo_cw < costo_nn) {
            std::cout << "🏆 Clarke & Wright es mejor en costo por " << (costo_nn - costo_cw) << std::endl;
        } else if (costo_nn < costo_cw) {
            std::cout << "🏆 Nearest Neighbor es mejor en costo por " << (costo_cw - costo_nn) << std::endl;
        } else {
            std::cout << "🤝 Ambos algoritmos tienen el mismo costo" << std::endl;
        }

        if (rutas_cw < rutas_nn) {
            std::cout << "🚛 Clarke & Wright usa menos vehículos (" << rutas_cw << " vs " << rutas_nn << ")" << std::endl;
        } else if (rutas_nn < rutas_cw) {
            std::cout << "🚛 Nearest Neighbor usa menos vehículos (" << rutas_nn << " vs " << rutas_cw << ")" << std::endl;
        } else {
            std::cout << "🚛 Ambos algoritmos usan el mismo número de vehículos" << std::endl;
        }

        // -------------------------------------
        // 3. COMPARAR CON SOLUCIÓN DE REFERENCIA
        // -------------------------------------
        std::cout << "\n=== COMPARACIÓN CON SOLUCIÓN DE REFERENCIA ===" << std::endl;
        
        // Construir el path de la solución de referencia
        std::string instance_name = reader.getName();
        std::string converted_name = convertInstanceNameToSolutionName(instance_name);
        std::cout << "DEBUG: Nombre original: '" << instance_name << "'" << std::endl;
        std::cout << "DEBUG: Nombre convertido: '" << converted_name << "'" << std::endl;
        std::string solution_path = "instancias/2l-cvrp-0/soluciones/" + converted_name + ".HRE";
        
        std::cout << "Buscando solución de referencia en: " << solution_path << std::endl;
        
        try {
            SolutionReader solution_reader(solution_path);
            const Solution& sol_ref = solution_reader.getSolution();
            
            std::cout << "Solución de referencia:" << std::endl;
            sol_ref.imprimir();
            std::cout << "Costo total referencia: " << sol_ref.calcularCostoTotal() << std::endl;
            std::cout << "Número de rutas referencia: " << sol_ref.cantidadRutas() << std::endl;
            
            // Comparar soluciones
            std::cout << "\n=== RESULTADO DE LA COMPARACIÓN ===" << std::endl;
            if (areSolutionsEqual(sol_cw, sol_ref)) {
                std::cout << "✅ Las soluciones son iguales!" << std::endl;
            } else {
                std::cout << "❌ Las soluciones son diferentes." << std::endl;
                
                // Mostrar diferencias detalladas
                std::cout << "\nDiferencias:" << std::endl;
                std::cout << "Clarke & Wright - Costo: " << sol_cw.calcularCostoTotal() 
                          << ", Rutas: " << sol_cw.cantidadRutas() << std::endl;
                std::cout << "Referencia - Costo: " << sol_ref.calcularCostoTotal() 
                          << ", Rutas: " << sol_ref.cantidadRutas() << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cout << "⚠️  No se pudo cargar la solución de referencia: " << e.what() << std::endl;
            std::cout << "Continuando solo con la solución de Clarke & Wright..." << std::endl;
        }

        // -------------------------------------
        // 4. ESTADÍSTICAS FINALES
        // -------------------------------------
        std::cout << "\n=== ESTADÍSTICAS FINALES ===" << std::endl;
        std::cout << "Instancia: " << reader.getName() << std::endl;
        std::cout << "Algoritmo: Clarke & Wright" << std::endl;
        std::cout << "Costo total: " << sol_cw.calcularCostoTotal() << std::endl;
        std::cout << "Número de rutas: " << sol_cw.cantidadRutas() << std::endl;
        std::cout << "Capacidad del vehículo: " << reader.getCapacity() << std::endl;
        std::cout << "Demanda total: " << sol_cw.getDemandaTotal() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} 