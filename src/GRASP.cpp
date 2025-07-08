#include "GRASP.h"
#include <algorithm>
#include <random>
#include <iostream>
#include "local_search.h"
#include "solution.h"

Solution GRASP::ejecutarGRASP(const VRPLIBReader& reader, int rcl_size) {
    std::cout << "Ejecutando GRASP con RCL de tamaño " << rcl_size << std::endl;
    
    // Fase 1: Construcción aleatorizada
    Solution solucion = construirSolucionAleatorizada(reader, rcl_size);
    
    std::cout << "Aplicando búsqueda local mejorada..." << std::endl;
    
    // Búsqueda local más intensiva
    bool mejora = true;
    int iteraciones = 0;
    const int max_iteraciones = reader.getDimension();
    for (int i=0 ; i< reader.getDimension(); i++){
        while (mejora && iteraciones < max_iteraciones) {
            bool mejora_relocate = LocalSearch::relocate(solucion, reader);
            bool mejora_swap = LocalSearch::swap(solucion, reader);
            mejora = mejora_relocate || mejora_swap;
            iteraciones++;
        }
    }
    std::cout << "Búsqueda local completada en " << iteraciones << " iteraciones" << std::endl;
    
    return solucion;
}

Solution GRASP::construirSolucionAleatorizada(const VRPLIBReader& reader, int rcl_size) {
    const auto& nodes = reader.getNodes();
    const auto& demands = reader.getDemands();
    int depot = reader.getDepotId();
    int capacidad = reader.getCapacity();
    const auto& distanceMatrix = reader.getDistanceMatrix();
    
    Solution solucion;
    std::vector<bool> visitados(nodes.size(), false);
    visitados[depot] = true; // El depósito ya está visitado
    
    // Generador de números aleatorios
    std::random_device rd;
    std::mt19937 gen(rd());
    
    while (true) {
        // Crear nueva ruta desde el depósito
        Ruta ruta_actual;
        ruta_actual.secuencia.push_back(depot);
        ruta_actual.suma_demanda = 0;
        ruta_actual.costo = 0;
        
        bool ruta_completada = false;
        
        while (!ruta_completada) {
            int nodo_actual = ruta_actual.secuencia.back();
            
            // Encontrar todos los candidatos no visitados que caben en la ruta
            std::vector<std::pair<double, int>> candidatos;
            
            for (size_t i = 0; i < nodes.size(); ++i) {
                if (!visitados[i] && i != depot) {
                    double distancia = distanceMatrix[nodo_actual][i];
                    int demanda = demands[i];
                    
                    if (ruta_actual.suma_demanda + demanda <= capacidad) {
                        candidatos.push_back({distancia, i});
                    }
                }
            }
            
            if (candidatos.empty()) {
                ruta_completada = true;
                break;
            }
            
            // Ordenar candidatos por distancia (más cercanos primero)
            std::sort(candidatos.begin(), candidatos.end());
            
            // Crear RCL con los mejores rcl_size candidatos
            int tamano_rcl = std::min(rcl_size, (int)candidatos.size());
            std::vector<std::pair<double, int>> rcl(candidatos.begin(), candidatos.begin() + tamano_rcl);
            
            // Seleccionar aleatoriamente de RCL
            int candidato_seleccionado = seleccionarDeRCL(rcl);
            
            // Agregar nodo a la ruta
            int nodo_anterior = ruta_actual.secuencia.back();
            ruta_actual.secuencia.push_back(candidato_seleccionado);
            ruta_actual.suma_demanda += demands[candidato_seleccionado];
            ruta_actual.costo += distanceMatrix[nodo_anterior][candidato_seleccionado];
            visitados[candidato_seleccionado] = true;
            if(ruta_actual.suma_demanda >= capacidad){
                ruta_completada = true;
            }
        }
        
        // Cerrar la ruta (volver al depósito)
        if (ruta_actual.secuencia.size() > 1) {
            ruta_actual.secuencia.push_back(depot);
            int ultimo_nodo = ruta_actual.secuencia[ruta_actual.secuencia.size() - 2];
            ruta_actual.costo += distanceMatrix[ultimo_nodo][depot];
        }
        
        solucion.agregarRuta(ruta_actual);
        
        // Verificar si todos los nodos han sido visitados
        bool todos_visitados = true;
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (!visitados[i] && i != depot) {
                todos_visitados = false;
                break;
            }
        }
        
        if (todos_visitados) {
            break;
        }
    }
    
    return solucion;
}

int GRASP::seleccionarDeRCL(const std::vector<std::pair<double, int>>& rcl) {
    if (rcl.empty()) {
        return -1;
    }
    
    // Generador de números aleatorios
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, rcl.size() - 1);
    
    int indice_aleatorio = dis(gen);
    return rcl[indice_aleatorio].second;
} 