#include "clarke_wright.h"
#include
#include <algorithm>
#include <limits>

ClarkeWrightSolver::ClarkeWrightSolver(const VRPLIBReader& reader)
    : reader(reader) {}

void ClarkeWrightSolver::calcularAhorros() {
    const auto& dist = reader.getDistanceMatrix();
    int depot = reader.getDepotId();
    int n = reader.getNodes().size();
    int capacity = reader.getCapacity();


    lista_ahorros.clear();

    for (int i = 1; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if(demandas[i] + demandas[j] <= capacity){
                double s = dist[depot][i] + dist[depot][j] - dist[i][j];
                lista_ahorros.push_back({i, j, s});
            }else{
                lista_ahorros.push_back({i, j, 0});
            }
        }
    }

    std::sort(lista_ahorros.begin(), lista_ahorros.end(),
              [](const Ahorro& a, const Ahorro& b) {
                  return a.valor > b.valor;
              });
}

//ESTO NO NOS SIRVE
Solucion ClarkeWrightSolver::inicializarSolucion() {
    Solucion sol;
    const auto& demandas = reader.getDemands();
    const auto& dist = reader.getDistanceMatrix();
    int depot = reader.getDepotId();
    int n = reader.getNodes().size();

    for (int i = 1; i < n; ++i) { //Creamos la ruta del deposito a cada cliente
        Ruta r;
        r.nodos = {depot, i, depot};
        r.demanda_total = demandas[i];
        r.costo_total = 2 * dist[depot][i];
        sol.agregarRuta(r);
    }
    return sol;
}

Solucion ClarkeWrightSolver::construirSolucion() {
    Solucion solucion = inicializarSolucion();
    const auto& demandas = reader.getDemands();
    const auto& dist = reader.getDistanceMatrix();
    int depot = reader.getDepotId();
    int n = reader.getNodes().size();
    vector<int> visitados(n, 0);

    for (const auto& ahorros : lista_ahorros) { // Para cada ahorro en la lista
        if (ahorros.valor > 0 && visitados[ahorros.i] == 0 && visitados[ahorros.j] == 0) {
            // Crear nueva ruta mergeada
            Ruta new_route;
            new_route.nodos = {depot, ahorros.i, ahorros.j, depot};
            new_route.demanda_total = demandas[ahorros.i] + demandas[ahorros.j];
            new_route.costo_total = dist[depot][ahorros.i] + dist[ahorros.i][ahorros.j] + dist[ahorros.j][depot];

            visitados[ahorros.i] = 1;
            visitados[ahorros.j] = 1;

            solucion.agregarRuta(new_route);
        } else if (ahorros.valor > 0 && visitados[ahorros.i] == 1 && visitados[ahorros.j] == 0) {
            // Extender ruta existente para incluir ahorros.j
            for (auto& ruta : solucion.getRutas()) {
                if (ruta.nodos[1] == ahorros.i) {
                    ruta.nodos.insert(ruta.nodos.end() - 1, ahorros.j);
                    ruta.demanda_total += demandas[ahorros.j];
                    ruta.costo_total += dist[ruta.nodos[ruta.nodos.size() - 3]][ahorros.j] + dist[ahorros.j][depot];
                    visitados[ahorros.j] = 1;
                    break;
                }
            }
        } else if (ahorros.valor > 0 && visitados[ahorros.i] == 0 && visitados[ahorros.j] == 1) {
            // Extender ruta existente para incluir ahorros.i
            for (auto& ruta : solucion.getRutas()) {
                if (ruta.nodos[1] == ahorros.j) {
                    ruta.nodos.insert(ruta.nodos.begin() + 1, ahorros.i);
                    ruta.demanda_total += demandas[ahorros.i];
                    ruta.costo_total += dist[depot][ahorros.i] + dist[ahorros.i][ruta.nodos[2]];
                    visitados[ahorros.i] = 1;
                    break;
                }
            }
        }
    }

    return solucion;
}
