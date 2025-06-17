#include "nearest_neighbor.h"
#include <limits>

NearestNeighborSolver::NearestNeighborSolver(const VRPLIBReader& reader)
    : reader(reader) {
    visitado.resize(reader.getNodes().size(), false);
    visitado[reader.getDepotId()] = true;
}

Solucion NearestNeighborSolver::construirSolucion() {
    Solucion sol;
    const auto& dist = reader.getDistanceMatrix();
    const auto& demandas = reader.getDemands();
    int Q = reader.getCapacity();
    int depot = reader.getDepotId();
    int total_visitados = 1;

    while (total_visitados < (int)visitado.size()) {
        Ruta ruta;
        ruta.nodos.push_back(depot);
        int carga = 0;
        int actual = depot;
        double costo = 0.0;

        while (true) {
            double mejor_dist = std::numeric_limits<double>::max();
            int siguiente = -1;

            for (int i = 1; i < (int)visitado.size(); ++i) {
                if (!visitado[i] && carga + demandas[i] <= Q &&
                    dist[actual][i] < mejor_dist) {
                    mejor_dist = dist[actual][i];
                    siguiente = i;
                }
            }

            if (siguiente == -1) break;

            costo += dist[actual][siguiente];
            ruta.nodos.push_back(siguiente);
            carga += demandas[siguiente];
            visitado[siguiente] = true;
            actual = siguiente;
            ++total_visitados;
        }

        ruta.nodos.push_back(depot);
        costo += dist[actual][depot];
        ruta.demanda_total = carga;
        ruta.costo_total = costo;
        sol.agregarRuta(ruta);
    }

    return sol;
}
