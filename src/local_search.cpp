#include "local_search.h"
#include "SolutionReader.h"
#include "solution.h"
#include <limits>
#include <algorithm>
#include <iostream>

bool LocalSearch::relocate(Solution& solucion, const VRPLIBReader& reader) {
    auto& rutas = solucion.getRutas();
    const auto& dist = reader.getDistanceMatrix();
    const auto& demandas = reader.getDemands();
    int capacidad = reader.getCapacity();

    for (size_t r1 = 0; r1 < rutas.size(); ++r1) {
        auto& ruta1 = rutas[r1];
        if (ruta1.secuencia.size() <= 3) continue;

        for (size_t i = 1; i < ruta1.secuencia.size() - 1; ++i) {
            int cliente = ruta1.secuencia[i];
            int demanda = demandas[cliente];

            for (size_t r2 = 0; r2 < rutas.size(); ++r2) {
                if (r1 == r2 && ruta1.secuencia.size() <= 4) continue;
                if (r1 != r2 && rutas[r2].suma_demanda + demanda > capacidad) continue;

                for (size_t j = 1; j < rutas[r2].secuencia.size(); ++j) {
                    if (r1 == r2 && (j == i || j == i + 1)) continue;

                    int prev_src = ruta1.secuencia[i - 1];
                    int next_src = ruta1.secuencia[i + 1];
                    double costo_remover = dist[prev_src][cliente] + dist[cliente][next_src] - dist[prev_src][next_src];

                    int prev_dst = rutas[r2].secuencia[j - 1];
                    int next_dst = rutas[r2].secuencia[j];
                    double costo_insertar = dist[prev_dst][cliente] + dist[cliente][next_dst] - dist[prev_dst][next_dst];

                    double delta = costo_insertar - costo_remover;


                    if (delta < 0) {

                        int cliente = rutas[r1].secuencia[i];
                        rutas[r1].secuencia.erase(rutas[r1].secuencia.begin() + i);
                        rutas[r1].suma_demanda -= demandas[cliente];

                        size_t insert_pos = j;
                        if (r1 == r2 && j > i) insert_pos--;

                        rutas[r2].secuencia.insert(rutas[r2].secuencia.begin() + insert_pos, cliente);
                        rutas[r2].suma_demanda += demandas[cliente];

                        // Actualizar costos usando el delta calculado
                        // El delta ya es: costo_insertar - costo_remover
                        // Para la ruta origen: restar costo_remover
                        rutas[r1].costo -= costo_remover;
                        // Para la ruta destino: sumar costo_insertar  
                        rutas[r2].costo += costo_insertar;
                        
                        // Actualizar costo total de la solución
                        // Después de terminar movimientos
                        solucion.actualizarCostoTotal(dist);

                        return true; // Sale en el primer movimiento que mejora
                    }
                }
            }
        }
    }
    return false;
}

bool LocalSearch::swap(Solution& solucion, const VRPLIBReader& reader) {
    auto& rutas = solucion.getRutas();
    const auto& dist = reader.getDistanceMatrix();
    const auto& demandas = reader.getDemands();
    int capacidad = reader.getCapacity();

    for (size_t r1 = 0; r1 < rutas.size(); ++r1) {
        auto& ruta1 = rutas[r1];
        if (ruta1.secuencia.size() <= 3) continue;

        for (size_t i = 1; i < ruta1.secuencia.size() - 1; ++i) {
            for (size_t r2 = r1; r2 < rutas.size(); ++r2) {
                auto& ruta2 = rutas[r2];
                if (ruta2.secuencia.size() <= 3) continue;

                size_t j_start = (r1 == r2) ? i + 1 : 1;

                for (size_t j = j_start; j < ruta2.secuencia.size() - 1; ++j) {
                    int c1 = ruta1.secuencia[i];
                    int c2 = ruta2.secuencia[j];
                    int d1_new = rutas[r1].suma_demanda - demandas[c1] + demandas[c2];
                    int d2_new = rutas[r2].suma_demanda - demandas[c2] + demandas[c1];

                    if (d1_new > capacidad || d2_new > capacidad) continue;

                    double delta = calcularDeltaSwap(rutas[r1], rutas[r2], i, j, dist);

                    if (delta < 0) {
                        // Calcular deltas individuales para cada ruta
                        double delta_r1, delta_r2;
                        
                        if (r1 == r2) {
                            // Misma ruta: todo el delta va a una sola ruta
                            delta_r1 = delta;
                            delta_r2 = 0;
                        } else {
                            // Rutas diferentes: calcular deltas individuales
                            int c1 = ruta1.secuencia[i];
                            int c2 = ruta2.secuencia[j];
                            
                            // Delta para ruta 1: costo de remover c1 + costo de insertar c2
                            int prev1 = ruta1.secuencia[i - 1];
                            int next1 = ruta1.secuencia[i + 1];
                            double costo_remover_c1 = dist[prev1][c1] + dist[c1][next1] - dist[prev1][next1];
                            double costo_insertar_c2 = dist[prev1][c2] + dist[c2][next1] - dist[prev1][next1];
                            delta_r1 = costo_insertar_c2 - costo_remover_c1;
                            
                            // Delta para ruta 2: costo de remover c2 + costo de insertar c1
                            int prev2 = ruta2.secuencia[j - 1];
                            int next2 = ruta2.secuencia[j + 1];
                            double costo_remover_c2 = dist[prev2][c2] + dist[c2][next2] - dist[prev2][next2];
                            double costo_insertar_c1 = dist[prev2][c1] + dist[c1][next2] - dist[prev2][next2];
                            delta_r2 = costo_insertar_c1 - costo_remover_c2;
                        }
                        
                        // Aplica el swap
                        std::swap(rutas[r1].secuencia[i], rutas[r2].secuencia[j]);
                        rutas[r1].suma_demanda = d1_new;
                        rutas[r2].suma_demanda = d2_new;

                        // Actualizar costos usando los deltas individuales
                        rutas[r1].costo += delta_r1;
                        rutas[r2].costo += delta_r2;

                        solucion.actualizarCostoTotal(dist);
                        return true; // Sale en el primer swap que mejora
                    }
                }
            }
        }
    }
    return false;
}

bool LocalSearch::mejorarSolucion(Solution& solucion, const VRPLIBReader& reader, int max_iter) {
    bool mejora_global = false;
    int iter = 0;

    while (iter < max_iter) {
        bool cambio = false;
        cambio = cambio || relocate(solucion, reader);
        cambio = cambio || swap(solucion, reader);
        if (!cambio) break;
        mejora_global = true;
        iter++;
    }

    return mejora_global;
}

double LocalSearch::calcularDeltaSwap(const Ruta& ruta1, const Ruta& ruta2, 
                                      size_t pos1, size_t pos2, 
                                      const std::vector<std::vector<double>>& dist) {
    int c1 = ruta1.secuencia[pos1];
    int c2 = ruta2.secuencia[pos2];

    if (&ruta1 == &ruta2) {
        if (pos2 == pos1 + 1 || pos1 == pos2 + 1) {
            size_t i = std::min(pos1, pos2);
            int a = ruta1.secuencia[i - 1];
            int b = ruta1.secuencia[i];
            int c = ruta1.secuencia[i + 1];
            int d = ruta1.secuencia[i + 2];
            return dist[a][c] + dist[c][b] + dist[b][d] - dist[a][b] - dist[b][c] - dist[c][d];
        } else {
            int a1 = ruta1.secuencia[pos1 - 1];
            int b1 = ruta1.secuencia[pos1 + 1];
            int a2 = ruta2.secuencia[pos2 - 1];
            int b2 = ruta2.secuencia[pos2 + 1];
            return dist[a1][c2] + dist[c2][b1] + dist[a2][c1] + dist[c1][b2] -
                   dist[a1][c1] - dist[c1][b1] - dist[a2][c2] - dist[c2][b2];
        }
    } else {
        int a1 = ruta1.secuencia[pos1 - 1];
        int b1 = ruta1.secuencia[pos1 + 1];
        int a2 = ruta2.secuencia[pos2 - 1];
        int b2 = ruta2.secuencia[pos2 + 1];
        return dist[a1][c2] + dist[c2][b1] + dist[a2][c1] + dist[c1][b2] -
               dist[a1][c1] - dist[c1][b1] - dist[a2][c2] - dist[c2][b2];
    }
}
