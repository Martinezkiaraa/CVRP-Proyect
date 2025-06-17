#ifndef NEAREST_NEIGHBOR_H
#define NEAREST_NEIGHBOR_H

#include "vrplib_reader.h"
#include "solucion.h"

class NearestNeighborSolver {
public:
    explicit NearestNeighborSolver(const VRPLIBReader& reader);
    Solucion construirSolucion();

private:
    const VRPLIBReader& reader;
    std::vector<bool> visitado;
};

#endif
