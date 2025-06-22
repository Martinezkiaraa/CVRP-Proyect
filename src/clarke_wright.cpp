#include "clarke_wright.h"
#include "solution.h"
#include <algorithm>
#include <limits>

ClarkeWrightSolver::ClarkeWrightSolver(const VRPLIBReader& reader) : reader(reader) {}

void ClarkeWrightSolver::calcularAhorros() {
    const auto& dist = reader.getDistanceMatrix();
    int depot = reader.getDepotId();
    int n = reader.getNodes().size();
    int capacity = reader.getCapacity();
    vector<int>


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

    std::sort(lista_ahorros.begin(), lista_ahorros.end(), [](const Ahorro& a, const Ahorro& b) {
                  return a.valor > b.valor;
              });
}


Solution ClarkeWrightSolver::construirSolucion() {
    Solution solucion = inicializarSolucion();
    const auto& demandas = reader.getDemands();
    const auto& dist = reader.getDistanceMatrix();
    int depot = reader.getDepotId();
    int n = reader.getNodes().size();
    int capacidad = reader.getCapacity();

    std::vector<int> visitados(n, 0);

    calcularAhorros();

    for (const auto& ahorros : lista_ahorros) {
        if (ahorros.valor <= 0) continue;

        int i = ahorros.i;
        int j = ahorros.j;

        // Ambos nodos no asignados → crear nueva ruta
        if (visitados[i] == 0 && visitados[j] == 0) {
            int demanda = demandas[i] + demandas[j];
            if (demanda <= capacidad) {
                Ruta nueva;
                nueva.nodos = {depot, i, j, depot};
                nueva.demanda_total = demanda;
                nueva.costo_total = dist[depot][i] + dist[i][j] + dist[j][depot];
                solucion.agregarRuta(nueva);
                visitados[i] = 1;
                visitados[j] = 1;
            }
        }

        // i asignado, j no asignado → agregar j al final de la ruta de i
        else if (visitados[i] == 1 && visitados[j] == 0) {
            for (auto& ruta : solucion.getRutas()) {
                int ultimo = ruta.nodos[ruta.nodos.size() - 2];
                if (ultimo == i) {
                    int nueva_demanda = ruta.demanda_total + demandas[j];
                    if (nueva_demanda <= capacidad) {
                        ruta.nodos.insert(ruta.nodos.end() - 1, j);
                        ruta.demanda_total = nueva_demanda;
                        ruta.costo_total += dist[i][j] + dist[j][depot] - dist[i][depot];
                        visitados[j] = 1;
                        break;
                    }
                }
            }
        } else if (ahorros.valor > 0 && visitados[ahorros.i] == 0 && visitados[ahorros.j] == 1) {
            // Extender ruta existente para incluir ahorros.i
            for (auto& ruta : solucion.getRutas()) {
                int primero = ruta.nodos[1];
                if (primero == j) {
                    int nueva_demanda = ruta.demanda_total + demandas[i];
                    if (nueva_demanda <= capacidad) {
                        ruta.nodos.insert(ruta.nodos.begin() + 1, i);
                        ruta.demanda_total = nueva_demanda;
                        ruta.costo_total += dist[depot][i] + dist[i][j] - dist[depot][j];
                        visitados[i] = 1;
                        break;
                    }
                }
            }
        }

        // Si no entra en ningún caso válido, no hacemos nada
    }

    // Asignar rutas triviales a clientes sueltos
    for (int cliente = 1; cliente < n; ++cliente) {
        if (visitados[cliente] == 0) {
            Ruta r;
            r.nodos = {depot, cliente, depot};
            r.demanda_total = demandas[cliente];
            r.costo_total = dist[depot][cliente] + dist[cliente][depot];
            solucion.agregarRuta(r);
        }
    }

    return solucion;
}
