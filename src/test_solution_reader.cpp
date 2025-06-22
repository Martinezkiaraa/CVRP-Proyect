#include <iostream>
#include "SolutionReader.h"

int main() {
    try {
        // Leer una solución de ejemplo
        SolutionReader reader("../instancias/2l-cvrp-0/soluciones/E045-04F.HRE");
        
        std::cout << "=== Información de la Solución ===" << std::endl;
        std::cout << "Nombre: " << reader.getName() << std::endl;
        std::cout << "Comentario: " << reader.getComment() << std::endl;
        std::cout << "Tipo: " << reader.getType() << std::endl;
        std::cout << "Número de rutas: " << reader.getNumRoutes() << std::endl;
        std::cout << "Costo total: " << reader.getTotalCost() << std::endl;
        std::cout << "Depósito: " << reader.getDepot() << std::endl;
        
        std::cout << "\n=== Rutas ===" << std::endl;
        const Solution& solution = reader.getSolution();
        solution.imprimir();
        
        std::cout << "\n=== Verificación ===" << std::endl;
        std::cout << "Costo calculado: " << solution.calcularCostoTotal() << std::endl;
        std::cout << "Número de rutas: " << solution.cantidadRutas() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 