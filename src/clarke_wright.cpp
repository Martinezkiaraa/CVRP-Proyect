#include "clarke_wright.h"
#include "solution.h"
#include <algorithm>
#include <limits>

ClarkeWrightSolver::ClarkeWrightSolver(const VRPLIBReader& reader) : reader(reader) {}

void ClarkeWrightSolver::calcularAhorros() {
    const auto& dist = reader.getDistanceMatrix();
    const auto& demandas = reader.getDemands();
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

    std::sort(lista_ahorros.begin(), lista_ahorros.end(), [](const Ahorro& a, const Ahorro& b) {
                  return a.valor > b.valor;
              });
}

Solution ClarkeWrightSolver::construirSolucion() {
    Solution solucion;
    const auto& demandas = reader.getDemands();
    const auto& dist = reader.getDistanceMatrix();
    int depot = reader.getDepotId();
    int n = reader.getNodes().size();
    int capacidad = reader.getCapacity();

    std::vector<int> visitados(n, 0);

    calcularAhorros();

    for (const auto& ahorro : lista_ahorros) {
        if (ahorro.valor <= 0) continue;

        int i = ahorro.i;
        int j = ahorro.j;

        // Ambos nodos no asignados → crear nueva ruta
        if (visitados[i] == 0 && visitados[j] == 0) {
            int demanda = demandas[i] + demandas[j];
            if (demanda <= capacidad) {
                Ruta nueva;
                nueva.id = solucion.getNumRutas();
                nueva.secuencia = {depot, i, j, depot};
                nueva.suma_demanda = demanda;
                nueva.costo = dist[depot][i] + dist[i][j] + dist[j][depot];
                nueva.num_clientes = 2;
                nueva.longitud = nueva.costo;
                solucion.agregarRuta(nueva);
                visitados[i] = 1;
                visitados[j] = 1;
            }
        }

        // i asignado, j no asignado → agregar j al final de la ruta de i
        else if (visitados[i] == 1 && visitados[j] == 0) {
            auto& rutas = const_cast<std::vector<Ruta>&>(solucion.getRutas());
            for (auto& ruta : rutas) {
                int ultimo = ruta.secuencia[ruta.secuencia.size() - 2];
                if (ultimo == i) {
                    int nueva_demanda = ruta.suma_demanda + demandas[j];
                    if (nueva_demanda <= capacidad) {
                        ruta.secuencia.insert(ruta.secuencia.end() - 1, j);
                        ruta.suma_demanda = nueva_demanda;
                        ruta.costo += dist[i][j] + dist[j][depot] - dist[i][depot];
                        ruta.num_clientes++;
                        ruta.longitud = ruta.costo;
                        visitados[j] = 1;
                        break;
                    }
                }
            }
        } else if (ahorro.valor > 0 && visitados[i] == 0 && visitados[j] == 1) {
            // Extender ruta existente para incluir ahorro.i
            auto& rutas = const_cast<std::vector<Ruta>&>(solucion.getRutas());
            for (auto& ruta : rutas) {
                int primero = ruta.secuencia[1];
                if (primero == j) {
                    int nueva_demanda = ruta.suma_demanda + demandas[i];
                    if (nueva_demanda <= capacidad) {
                        ruta.secuencia.insert(ruta.secuencia.begin() + 1, i);
                        ruta.suma_demanda = nueva_demanda;
                        ruta.costo += dist[depot][i] + dist[i][j] - dist[depot][j];
                        ruta.num_clientes++;
                        ruta.longitud = ruta.costo;
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
            r.id = solucion.getNumRutas();
            r.secuencia = {depot, cliente, depot};
            r.suma_demanda = demandas[cliente];
            r.costo = dist[depot][cliente] + dist[cliente][depot];
            r.num_clientes = 1;
            r.longitud = r.costo;
            solucion.agregarRuta(r);
        }
    }

    return solucion;
}
