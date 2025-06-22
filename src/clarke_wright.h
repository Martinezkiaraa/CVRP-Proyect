#ifndef CLARKE_WRIGHT_H
#define CLARKE_WRIGHT_H

#include "VRPLIBReader.h"
#include "solution.h"

#include <vector>

struct Ahorro {
    int i, j;
    double valor;
};

class ClarkeWrightSolver {
public:
    explicit ClarkeWrightSolver(const VRPLIBReader& reader); //cambiar, no lo entendemos
    Solution construirSolucion();

private:
    const VRPLIBReader& reader;
    std::vector<Ahorro> lista_ahorros;
    void calcularAhorros();
};

#endif
