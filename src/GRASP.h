#ifndef GRASP_H
#define GRASP_H

#include "VRPLIBReader.h"
#include "solution.h"
#include "local_search.h"
#include <vector>
#include <utility>

class GRASP {
public:
    // Función principal de GRASP
    static Solution ejecutarGRASP(const VRPLIBReader& reader, int rcl_size);
    
    // Función de construcción aleatorizada
    static Solution construirSolucionAleatorizada(const VRPLIBReader& reader, int rcl_size);
    
    // Función para seleccionar de RCL
    static int seleccionarDeRCL(const std::vector<std::pair<double, int>>& rcl);
};

#endif // GRASP_H