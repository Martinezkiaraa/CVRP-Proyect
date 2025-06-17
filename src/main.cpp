#include <iostream>
#include "VRPLIBReader.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_vrp_file>" << std::endl;
        return 1;
    }

    VRPLIBReader reader(argv[1]);

    std::cout << "Instance Name: " << reader.getName() << std::endl;
    std::cout << "Dimension: " << reader.getDimension() << std::endl;
    std::cout << "Number of Vehicles: " << reader.getNumVehicles() << std::endl;
    std::cout << "Capacity: " << reader.getCapacity() << std::endl;
    // Notar que el deposito varia y no siempre es 0. getDepotId() nos dice cual es el id del deposito.
    std::cout << "Depot ID: " << reader.getDepotId() << std::endl;

    std::vector<Node> clients = reader.getNodes();

    std::cout << clients.size() << std::endl;
    
    // Testeamos la matriz de distancia. Tomando los id de los nodos, indexa desde 1.
    // Tener en cuenta esto al momento de decidir como representar una solucion.
    std::cout << "Distance matrix" << std::endl;
    std::vector<std::vector<double>> dist_matrix = reader.getDistanceMatrix();
    for (int i = 1; i <= reader.getDimension(); i++) {
        for (int j = 1; j <= reader.getDimension(); j++) {
            std::cout << dist_matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }

    // -------------------------------------
    // 2. HEURÍSTICA CONSTRUCTIVA 1: Clarke & Wright
    // -------------------------------------
    

    // -------------------------------------
    // 3. HEURÍSTICA CONSTRUCTIVA 2: Vecino más cercano
    // -------------------------------------

    return 0;
}