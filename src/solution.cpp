#include "solution.h"
#include <algorithm>

void Solution::agregarRuta(const Ruta& ruta) {
    rutas.push_back(ruta);
    costo_total += ruta.costo;
    demanda_total += ruta.suma_demanda;
    num_rutas++;
}

double Solution::calcularCostoTotal() const {
    return costo_total;
}

int Solution::cantidadRutas() const {
    return rutas.size();
}

const std::vector<Ruta>& Solution::getRutas() const {
    return rutas;
}

void Solution::imprimir() const {
    for (size_t i = 0; i < rutas.size(); ++i) {
        const auto& r = rutas[i];
        std::cout << "Ruta " << i + 1 << ": ";
        for (int nodo : r.secuencia)
            std::cout << nodo << " ";
        std::cout << "| Demanda: " << r.suma_demanda
                  << " | Costo: " << r.costo << std::endl;
    }
    std::cout << "Costo total solución: " << calcularCostoTotal() << std::endl;
}

void Solution::eliminarRuta(const Ruta& ruta) {
    // Buscar la ruta por ID y eliminarla
    for (auto it = rutas.begin(); it != rutas.end(); ++it) {
        if (it->id == ruta.id) {
            costo_total -= ruta.costo;
            demanda_total -= ruta.suma_demanda;
            num_rutas--;
            rutas.erase(it);
            break;
        }
    }
}
