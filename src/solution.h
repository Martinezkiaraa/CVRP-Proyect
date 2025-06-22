#ifndef SOLUTION_H
#define SOLUTION_H

#include <vector>
#include <string>
#include <iostream>

using namespace std;

struct Ruta {
    int id; // Número de la ruta
    int suma_demanda; // SUMD
    double costo; // COST
    double longitud; // LENGTH
    int num_clientes; // #C
    std::vector<int> secuencia; // SEQUENCE
};

class Solution {
public:
    // Constructor
    Solution() : costo_total(0.0), demanda_total(0), num_rutas(0) {}

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

    // Eliminar una ruta
    void eliminarRuta(const Ruta& ruta);

    // Setters para valores para soluciones ya dadas
    void setCostoTotal(double costo) { costo_total = costo; }
    void setNumRutas(int num) { num_rutas = num; }
    void setDepot(int d) { depot = d; }

    // Getters para valores del header
    double getCostoTotal() const { return costo_total; }
    int getNumRutas() const { return num_rutas; }
    int getDepot() const { return depot; }

private:
    int num_rutas; // RUTAS
    double costo_total; // COSTO
    double demanda_total; 
    int depot; // DESPOSITO
    vector<Ruta> rutas; // SOLUCION
};

#endif
