#ifndef INITIAL_METHODS_H
#define INITIAL_METHODS_H

#include "VRPLIBReader.h"
#include "solution.h"

// Función que combina Clarke & Wright con búsqueda local
Solution initial_methods(const VRPLIBReader& reader);

#endif // INITIAL_METHODS_H 