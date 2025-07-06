#include "local_search.h"
#include "VRPLIBReader.h"
#include "solution.h"
#include <iostream>

int main() {
    // Crear una instancia mínima del problema manualmente
    std::vector<std::vector<double>> dist = {
        {0, 2, 9, 10},
        {2, 0, 6, 4},
        {9, 6, 0, 3},
        {10, 4, 3, 0}
    };
    std::vector<int> demandas = {0, 2, 2, 2};
    int capacidad = 4;

    // Crear dos rutas subóptimas
    Ruta r1, r2;
    r1.secuencia = {0, 1, 2, 0};
    r1.suma_demanda = 4;
    r2.secuencia = {0, 3, 0};
    r2.suma_demanda = 2;

    // Calcular costos iniciales
    r1.costo = dist[0][1] + dist[1][2] + dist[2][0];
    r2.costo = dist[0][3] + dist[3][0];

    Solution sol;
    sol.agregarRuta(r1);
    sol.agregarRuta(r2);

    // Simular VRPLIBReader
    VRPLIBReader reader; 
    reader.setDistanceMatrix(dist);
    reader.setDemands(demandas);
    reader.setCapacity(capacidad);

    std::cout << "Costo inicial: " << sol.getCostoTotal() << std::endl;
    sol.imprimir();

    // Ejecutar búsqueda local
    bool mejorado = LocalSearch::mejorarSolucion(sol, reader);

    std::cout << (mejorado ? "Se encontró mejora." : "No se encontró mejora.") << std::endl;
    std::cout << "Costo final: " << sol.getCostoTotal() << std::endl;
    sol.imprimir();

    return 0;
}
