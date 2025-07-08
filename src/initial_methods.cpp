#include <limits>
#include <algorithm>
#include <iostream>
#include "clarke_wright.h"
#include "local_search.h"
#include "initial_methods.h"

using namespace std;

Solution initial_methods(const VRPLIBReader& reader) {

    // Solucion con Clarke y Wright
    ClarkeWrightSolver cw_solver(reader);
    Solution sol = cw_solver.construirSolucion();

    //Le aplico busqueda local 
    bool mejora = true;
    int iter = 0;
    const int max_iter = 200;
    while (mejora && iter < max_iter) {
        mejora = LocalSearch::relocate(sol, reader);
        iter++;
    }
    return sol;
}








