#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include "VRPLIBReader.h"
#include "clarke_wright.h"
#include "nearest_neighbor.h"
#include "SolutionReader.h"

// Convert instance name to solution file name format
std::string getSolutionFileName(const std::string& instanceName) {
    std::string name = instanceName;
    name.erase(remove_if(name.begin(), name.end(), ::isspace), name.end());
    
    size_t dotPos = name.find_last_of('.');
    std::string namePart = (dotPos != std::string::npos) ? name.substr(0, dotPos) : name;
    
    // Convert last letter to uppercase
    for (size_t i = namePart.size(); i-- > 0;) {
        if (std::isalpha(namePart[i])) {
            namePart[i] = std::toupper(namePart[i]);
            break;
        }
    }
    return namePart;
}

// Compare two solutions and return true if they are equal
bool compareSolutions(const Solution& sol1, const Solution& sol2) {
    if (sol1.cantidadRutas() != sol2.cantidadRutas()) return false;
    if (std::abs(sol1.calcularCostoTotal() - sol2.calcularCostoTotal()) > 0.001) return false;
    
    const auto& rutas1 = sol1.getRutas();
    const auto& rutas2 = sol2.getRutas();
    
    for (size_t i = 0; i < rutas1.size(); ++i) {
        if (rutas1[i].secuencia != rutas2[i].secuencia) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_vrp_file>" << std::endl;
        return 1;
    }

    try {
        // Load instance
        VRPLIBReader reader(argv[1]);
        std::cout << "Instance: " << reader.getName() << std::endl;
        std::cout << "Clients: " << reader.getNodes().size() << std::endl;
        std::cout << "Capacity: " << reader.getCapacity() << std::endl;

        // Solve with Clarke & Wright
        ClarkeWrightSolver cw_solver(reader);
        Solution sol_cw = cw_solver.construirSolucion();

        // Solve with Nearest Neighbor
        NearestNeighborSolver nn_solver(reader);
        Solution sol_nn = nn_solver.construirSolucion();

        // Compare algorithms
        double costo_cw = sol_cw.calcularCostoTotal();
        double costo_nn = sol_nn.calcularCostoTotal();
        int rutas_cw = sol_cw.cantidadRutas();
        int rutas_nn = sol_nn.cantidadRutas();

        std::cout << "\nResults:" << std::endl;
        std::cout << "Clarke & Wright:  Cost=" << costo_cw << " Routes=" << rutas_cw << std::endl;
        std::cout << "Nearest Neighbor: Cost=" << costo_nn << " Routes=" << rutas_nn << std::endl;

        // Determine best algorithm
        std::string best_cost = (costo_cw < costo_nn) ? "Clarke & Wright" : 
                               (costo_nn < costo_cw) ? "Nearest Neighbor" : "Tie";
        std::string best_routes = (rutas_cw < rutas_nn) ? "Clarke & Wright" : 
                                 (rutas_nn < rutas_cw) ? "Nearest Neighbor" : "Tie";

        if (best_cost != "Tie") {
            std::cout << "Best cost: " << best_cost << std::endl;
        }
        if (best_routes != "Tie") {
            std::cout << "Fewest routes: " << best_routes << std::endl;
        }

        // Compare with reference solution if available
        std::string solution_path = "instancias/2l-cvrp-0/soluciones/" + 
                                   getSolutionFileName(reader.getName()) + ".HRE";
        
        try {
            SolutionReader solution_reader(solution_path);
            const Solution& sol_ref = solution_reader.getSolution();
            
            std::cout << "\nReference solution: Cost=" << sol_ref.calcularCostoTotal() 
                      << " Routes=" << sol_ref.cantidadRutas() << std::endl;
            
            if (compareSolutions(sol_cw, sol_ref)) {
                std::cout << "Clarke & Wright matches reference solution" << std::endl;
            } else {
                std::cout << "Clarke & Wright differs from reference solution" << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cout << "Reference solution not found" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} 