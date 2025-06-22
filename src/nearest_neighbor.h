#ifndef NEAREST_NEIGHBOR_H
#define NEAREST_NEIGHBOR_H

#include "VRPLIBReader.h"
#include "solution.h"

class NearestNeighborSolver {
public:
    explicit NearestNeighborSolver(const VRPLIBReader& reader);
    Solution construirSolucion();

private:
    const VRPLIBReader& reader;
    std::vector<bool> visitado;
    Solution inicializarSolucion();
};

#endif
