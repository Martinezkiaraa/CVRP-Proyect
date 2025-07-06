#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include "solution.h"
#include "VRPLIBReader.h"
#include <vector>

class LocalSearch {
public:
    // Operadores de búsqueda local
    static bool relocate(Solution& solucion, const VRPLIBReader& reader);
    static bool swap(Solution& solucion, const VRPLIBReader& reader);
    
    // Función para mejora iterativa
    static bool mejorarSolucion(Solution& solucion, const VRPLIBReader& reader, int max_iter = 100);

private:
    // Función auxiliar para calcular delta de swap
    static double calcularDeltaSwap(const Ruta& ruta1, const Ruta& ruta2, 
                                   size_t pos1, size_t pos2, 
                                   const std::vector<std::vector<double>>& dist);
};

#endif // LOCAL_SEARCH_H