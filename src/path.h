#ifndef PATH_H
#define PATH_H

#include "segments.h"

// 1. Estructura para la lista de calles (Path)
typedef struct {
    StreetNode **streets; // Array dinámico de punteros a calles
    int count;            // Cuántas calles hay en el array
} StreetList;

// 2. Estructura para los nodos de la Cola (Queue)
typedef struct QueueNode {
    StreetList path;
    struct QueueNode *next;
} QueueNode;

// 3. Estructura de la Cola
typedef struct {
    QueueNode *head;
    QueueNode *tail;
} Queue;

// Funciones
void calculate_and_print_path(StreetNode *graph, StreetNode *fromStreet, StreetNode *toStreet);

#endif