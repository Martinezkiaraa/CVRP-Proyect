#ifndef CLARKE_WRIGHT_H
#define CLARKE_WRIGHT_H

#include "VRPLIBReader.h"
#include "solution.h"

#include <vector>
#include <unordered_map>

struct Ahorro {
    int i, j;
    double valor;
    
    Ahorro(int i, int j, double valor) : i(i), j(j), valor(valor) {}
};

class ClarkeWrightSolver {
public:
    explicit ClarkeWrightSolver(const VRPLIBReader& reader);
    Solution construirSolucion();

private:
    const VRPLIBReader& reader;
    std::vector<Ahorro> lista_ahorros;
    
    // Optimized data structures
    std::unordered_map<int, int> node_to_route; // Maps node to route index
    std::vector<Ruta> rutas; // Direct access to routes
    
    void calcularAhorros();
    void crearRutaInicial(int i, int j, int depot);
    bool extenderRuta(int nodo_nuevo, int nodo_existente, int depot, int capacidad);
    void agregarRutaTrivial(int cliente, int depot);
};

#endif
