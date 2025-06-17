#include "solution.h"

void Solucion::agregarRuta(const Ruta& ruta) {
    rutas.push_back(ruta);
}

double Solucion::calcularCostoTotal() const {
    double total = 0.0;
    for (const auto& r : rutas) {
        total += r.costo_total;
    }
    return total;
}

int Solucion::cantidadRutas() const {
    return rutas.size();
}

const std::vector<Ruta>& Solucion::getRutas() const {
    return rutas;
}

void Solucion::imprimir() const {
    for (size_t i = 0; i < rutas.size(); ++i) {
        const auto& r = rutas[i];
        std::cout << "Ruta " << i + 1 << ": ";
        for (int nodo : r.nodos)
            std::cout << nodo << " ";
        std::cout << "| Demanda: " << r.demanda_total
                  << " | Costo: " << r.costo_total << std::endl;
    }
    std::cout << "Costo total solución: " << calcularCostoTotal() << std::endl;
}

void eliminarRuta(constRuta& ruta){
    rutas.
}