#include "nearest_neighbor.h"
#include "solution.h"
#include <limits>
#include <algorithm>
#include <vector>

using namespace std;

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
    int n = reader.getDimension();

    vector<bool> visitados(n + 1, false);
    visitados[depot] = true;
    int total_visitados = 1; // Ya visitamos el depósito

    // Continuar hasta visitar todos los nodos
    while (total_visitados < n) {
        // Crear una nueva ruta
        Ruta ruta_actual;
        ruta_actual.secuencia.push_back(depot);
        int capacidad_remanente = Q;
        int nodo_actual = depot;
        int ruta_id = sol.cantidadRutas() + 1;
        ruta_actual.id = ruta_id;
        ruta_actual.costo = 0.0; // Inicializar costo en 0

        // Construir la ruta actual
        while (true) {
            // Encontrar los 5 nodos más cercanos no visitados
            vector<pair<double, int>> candidatos; //Guar
            for (int i = 1; i <= n; i++) {
                if (!visitados[i] && i != depot) {
                    candidatos.push_back({dist[nodo_actual][i], i});
                }
            }

            // Si no hay candidatos, terminar la ruta
            if (candidatos.empty()) {
                break;
            }

            // Ordenar por distancia y tomar los 5 primeros
            sort(candidatos.begin(), candidatos.end());
            candidatos.resize(std::min(5, (int)candidatos.size()));

            // Intentar agregar el mejor candidato que quepa en el vehículo
            bool agregado = false;
            for (const auto& candidato : candidatos) {
                int nodo = candidato.second;
                if (demandas[nodo] <= capacidad_remanente) {
                    // Agregar el nodo a la ruta
                    ruta_actual.secuencia.push_back(nodo);
                    
                    // Actualizar costo incrementalmente
                    ruta_actual.costo += dist[nodo_actual][nodo];
                    
                    // Actualizar capacidad y estado
                    capacidad_remanente -= demandas[nodo];
                    visitados[nodo] = true;
                    nodo_actual = nodo;
                    total_visitados++;
                    agregado = true;
                    break;
                }
            }

            // Si no se pudo agregar ningún nodo, terminar la ruta
            if (!agregado) {
                break;
            }
        }

        // Cerrar la ruta volviendo al depósito
        ruta_actual.secuencia.push_back(depot);
        ruta_actual.costo += dist[nodo_actual][depot]; // Agregar costo de vuelta al depósito

        // Calcular propiedades finales de la ruta
        ruta_actual.suma_demanda = Q - capacidad_remanente;
        ruta_actual.num_clientes = ruta_actual.secuencia.size() - 2; // Sin contar depósito
        ruta_actual.longitud = ruta_actual.costo; // En este caso son iguales

        // Agregar la ruta a la solución
        sol.agregarRuta(ruta_actual);
    }

    return sol;
}
