#include "clarke_wright.h"
#include <algorithm>
#include <limits>

ClarkeWrightSolver::ClarkeWrightSolver(const VRPLIBReader& reader)
    : reader(reader) {}

void ClarkeWrightSolver::calcularAhorros() {
    const auto& dist = reader.getDistanceMatrix();
    int depot = reader.getDepotId();
    int n = reader.getNodes().size();

    lista_ahorros.clear();
    for (int i = 1; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double s = dist[depot][i] + dist[depot][j] - dist[i][j];
            lista_ahorros.push_back({i, j, s});
        }
    }

    std::sort(lista_ahorros.begin(), lista_ahorros.end(),
              [](const Ahorro& a, const Ahorro& b) {
                  return a.valor > b.valor;
              });
}

Solucion ClarkeWrightSolver::inicializarSolucion() {
    Solucion sol;
    const auto& demandas = reader.getDemands();
    const auto& dist = reader.getDistanceMatrix();
    int depot = reader.getDepotId();
    int n = reader.getNodes().size();

    for (int i = 1; i < n; ++i) {
        Ruta r;
        r.nodos = {depot, i, depot};
        r.demanda_total = demandas[i];
        r.costo_total = dist[depot][i] + dist[i][depot];
        sol.agregarRuta(r);
    }
    return sol;
}

Solucion ClarkeWrightSolver::construirSolucion() {
    Solucion solucion = inicializarSolucion();
    calcularAhorros();

    // 🔧 TODO: Implementar la lógica de fusión de rutas acá

    return solucion;
}
