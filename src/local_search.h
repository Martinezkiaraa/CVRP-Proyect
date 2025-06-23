#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include "solution.h"
#include "VRPLIBReader.h"


class LocalSearch {
public:
    /**
     * Aplica el operador de reubicación a una solución.
     *
     * Este operador intenta mover un cliente de su posición actual a otra
     * posición en la misma ruta o en una ruta diferente para reducir el costo.
     *
     * @param solucion La solución a mejorar (se modifica por referencia).
     * @param reader Referencia al VRPLIBReader para acceder a la matriz de distancias.
     * @return true si se encontró una mejora, false en caso contrario.
     */
    static bool relocate(Solution& solucion, const VRPLIBReader& reader);

    /**
     *  Aplica el operador 2-opt dentro de cada ruta de una solución.
     *
     * Este operador intenta mejorar una ruta eliminando dos aristas no adyacentes
     * y reconectando los dos caminos resultantes de una manera diferente.
     * Se aplica a cada ruta de la solución de forma independiente.
     *
     * @param solucion La solución a mejorar (se modifica por referencia).
     * @param reader Referencia al VRPLIBReader para acceder a la matriz de distancias.
     * @return true si se encontró una mejora en al menos una ruta, false en caso contrario.
     */
    static bool two_opt_intra_ruta(Solution& solucion, const VRPLIBReader& reader);
};

#endif // LOCAL_SEARCH_H 