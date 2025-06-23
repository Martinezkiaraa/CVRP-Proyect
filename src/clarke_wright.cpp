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
    lista_ahorros.reserve((n-1) * (n-2) / 2); // Pre-allocate memory

    // Only store valid savings (positive values)
    for (int i = 1; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (demandas[i] + demandas[j] <= capacity) {
                double s = dist[depot][i] + dist[depot][j] - dist[i][j];
                if (s > 0) { // Only store positive savings
                    lista_ahorros.emplace_back(i, j, s);
                }
            }
        }
    }

    // Sort by savings value (descending)
    std::sort(lista_ahorros.begin(), lista_ahorros.end(), 
              [](const Ahorro& a, const Ahorro& b) { return a.valor > b.valor; });
}

void ClarkeWrightSolver::crearRutaInicial(int i, int j, int depot) {
    const auto& dist = reader.getDistanceMatrix();
    const auto& demandas = reader.getDemands();
    
    Ruta nueva;
    nueva.id = rutas.size();
    nueva.secuencia = {depot, i, j, depot};
    nueva.suma_demanda = demandas[i] + demandas[j];
    nueva.costo = dist[depot][i] + dist[i][j] + dist[j][depot];
    nueva.num_clientes = 2;
    nueva.longitud = nueva.costo;
    
    rutas.push_back(nueva);
    node_to_route[i] = nueva.id;
    node_to_route[j] = nueva.id;
}

bool ClarkeWrightSolver::extenderRuta(int nodo_nuevo, int nodo_existente, int depot, int capacidad) {
    const auto& dist = reader.getDistanceMatrix();
    const auto& demandas = reader.getDemands();
    
    auto it = node_to_route.find(nodo_existente);
    if (it == node_to_route.end()) return false;
    
    int route_idx = it->second;
    Ruta& ruta = rutas[route_idx];
    
    // Check capacity constraint
    int nueva_demanda = ruta.suma_demanda + demandas[nodo_nuevo];
    if (nueva_demanda > capacidad) return false;
    
    // Determine insertion position
    if (ruta.secuencia[1] == nodo_existente) {
        // Insert at beginning (after depot)
        ruta.secuencia.insert(ruta.secuencia.begin() + 1, nodo_nuevo);
        ruta.costo += dist[depot][nodo_nuevo] + dist[nodo_nuevo][nodo_existente] - dist[depot][nodo_existente];
    } else {
        // Insert at end (before depot)
        ruta.secuencia.insert(ruta.secuencia.end() - 1, nodo_nuevo);
        ruta.costo += dist[nodo_existente][nodo_nuevo] + dist[nodo_nuevo][depot] - dist[nodo_existente][depot];
    }
    
    ruta.suma_demanda = nueva_demanda;
    ruta.num_clientes++;
    ruta.longitud = ruta.costo;
    node_to_route[nodo_nuevo] = route_idx;
    
    return true;
}

void ClarkeWrightSolver::agregarRutaTrivial(int cliente, int depot) {
    const auto& dist = reader.getDistanceMatrix();
    const auto& demandas = reader.getDemands();
    
    Ruta r;
    r.id = rutas.size();
    r.secuencia = {depot, cliente, depot};
    r.suma_demanda = demandas[cliente];
    r.costo = dist[depot][cliente] + dist[cliente][depot];
    r.num_clientes = 1;
    r.longitud = r.costo;
    
    rutas.push_back(r);
    node_to_route[cliente] = r.id;
}

Solution ClarkeWrightSolver::construirSolucion() {
    const auto& demandas = reader.getDemands();
    int depot = reader.getDepotId();
    int n = reader.getNodes().size();
    int capacidad = reader.getCapacity();

    // Initialize data structures
    rutas.clear();
    node_to_route.clear();
    std::vector<bool> visitados(n, false);
    visitados[depot] = true;

    // Calculate savings
    calcularAhorros();

    // Process savings in descending order
    for (const auto& ahorro : lista_ahorros) {
        int i = ahorro.i;
        int j = ahorro.j;

        // Both nodes unassigned - create new route
        if (!visitados[i] && !visitados[j]) {
            int demanda = demandas[i] + demandas[j];
            if (demanda <= capacidad) {
                crearRutaInicial(i, j, depot);
                visitados[i] = visitados[j] = true;
            }
        }
        // One node assigned, one unassigned - extend existing route
        else if (visitados[i] && !visitados[j]) {
            if (extenderRuta(j, i, depot, capacidad)) {
                visitados[j] = true;
            }
        }
        else if (!visitados[i] && visitados[j]) {
            if (extenderRuta(i, j, depot, capacidad)) {
                visitados[i] = true;
            }
        }
        // If both are already visited, skip this savings
    }

    // Add trivial routes for unvisited clients
    for (int cliente = 1; cliente < n; ++cliente) {
        if (!visitados[cliente]) {
            agregarRutaTrivial(cliente, depot);
        }
    }

    // Build final solution
    Solution solucion;
    for (const auto& ruta : rutas) {
        solucion.agregarRuta(ruta);
    }

    return solucion;
}
