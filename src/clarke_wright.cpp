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
    vector <int> visitados(n, 0);
    for(auto& ahorros: lista_ahorros){ //Para cada ahorro en la lista, creamos una nueva ruta mergeada

        //VERIFICAR SI ES POSIBLE MERGEAR ESA RUTA
        if( ahorro[2] != 0 && visitados[ahorro[0]] != 1 && visitados[ahorro[1]] != 1){

            Ruta new_route;
            new_route.nodos = {depot, ahorros[0], ahorros[1], depot};
            
            new_route.demanta_total = demandas[ahorro[0]] + demandas[ahorro[1]];

            new_route.costo_total = dist[depot][ahorro[0]] + dist[ahorro[0]][ahorro[1]] + dist[ahorro[1]][depot];

            sol.agregarRuta(new_route);

            
        }

        

        lista_ahorros //sacar este save de lista_ahorros

    }


    calcularAhorros();

    // 🔧 TODO: Implementar la lógica de fusión de rutas acá

    return solucion;
}
