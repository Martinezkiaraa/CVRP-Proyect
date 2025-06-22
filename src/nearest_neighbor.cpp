#include "nearest_neighbor.h"
#include "solution.h"
#include <limits>

NearestNeighborSolver::NearestNeighborSolver(const VRPLIBReader& reader)
    : reader(reader) {
    visitado.resize(reader.getNodes().size(), false);
    visitado[reader.getDepotId()] = true;
}

Solution NearestNeighborSolver::inicializarSolucion() {
    Solution sol;
    const auto& demandas = reader.getDemands();
    const auto& dist = reader.getDistanceMatrix();
    int depot = reader.getDepotId();
    int n = reader.getNodes().size();

    for (int i = 1; i < n; ++i) {
        Ruta r;
        r.id = i-1; // empieza en 0 -> indice del vector
        r.secuencia = {depot, i, depot};
        r.suma_demanda = demandas[i];
        r.costo = dist[depot][i] + dist[i][depot];
        r.num_clientes = 1;
        r.longitud = r.costo;
        sol.agregarRuta(r);
    }
    return sol;
}

Solution NearestNeighborSolver::construirSolucion() {
    Solution sol;
    const auto& dist = reader.getDistanceMatrix();
    const auto& demandas = reader.getDemands();
    int Q = reader.getCapacity();
    int depot = reader.getDepotId();
    int total_visitados = 1;

    // TODO: Implement nearest neighbor algorithm
    // For now, return empty solution
    return sol;
}
