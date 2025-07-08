#include "solution.h"
#include <algorithm>

void Solution::agregarRuta(const Ruta& ruta) {
    rutas.push_back(ruta);
    costo_total += ruta.costo;
    demanda_total += ruta.suma_demanda;
    num_rutas++;
}

int Solution::cantidadRutas() const {
    return rutas.size();
}

std::vector<Ruta>& Solution::getRutas() {
    return rutas;
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
    std::cout << "Costo total solución: " << getCostoTotal() << std::endl;
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

void Solution::actualizarCostoTotal(const std::vector<std::vector<double>>& distancia) {
    recalcularCostosRutas(distancia);
    costo_total = 0.0;
    for (const auto& ruta : rutas) {
        costo_total += ruta.costo;
    }
}


void Solution::recalcularCostosRutas(const std::vector<std::vector<double>>& distancia) {
    for (auto& ruta : rutas) {
        double costo = 0.0;
        for (size_t i = 1; i < ruta.secuencia.size(); ++i) {
            int desde = ruta.secuencia[i - 1];
            int hasta = ruta.secuencia[i];
            costo += distancia[desde][hasta];
        }
        ruta.costo = costo;
    }
}
