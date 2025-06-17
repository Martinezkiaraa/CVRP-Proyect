#ifndef SOLUTION_H
#define SOLUTION_H

#include <vector>
#include <string>
#include <iostream>

struct Ruta {
    std::vector<int> nodos;         // Secuencia: [depósito, clientes..., depósito]
    int demanda_total = 0;          // SUMD
    double costo_total = 0.0;       // COST
};

class Solution {
public:
    // Agrega una ruta
    void agregarRuta(const Ruta& ruta);

    // Calcula el costo total de toda la solución
    double calcularCostoTotal() const;

    // Cantidad de vehículos usados
    int cantidadRutas() const;

    // Imprime solución tipo TP/informe
    void imprimir() const;

    // Acceso directo a las rutas
    const std::vector<Ruta>& getRutas() const;

private:
    std::vector<Ruta> rutas;
};

#endif
