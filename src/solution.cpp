#include "solution.h"

void Solution::agregarRuta(const Ruta& ruta) {
    rutas.push_back(ruta);
    costo_total += ruta.costo;
    demanda_total += ruta.demanda_total;
    num_rutas++;
}

double Solution::calcularCostoTotal() const {
    return costo_total;
}

int Solution::cantidadRutas() const {
    return rutas.size();
}

const std::vector<Ruta>& Solucion::getRutas() const {
    return rutas;
}

void Solution::imprimir() const {
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
    auto it = find(rutas.begin(), rutas.end(), ruta);
    if (it != rutas.end()) {
        costo_total -= ruta.costo;
        demanda_total -= ruta.demanda_total;
        num_rutas--;
        rutas.erase(it);
    }
}
