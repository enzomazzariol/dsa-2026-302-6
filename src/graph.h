#ifndef GRAPH_H
#define GRAPH_H

#include "segments.h"

#define GRAPH_BUCKETS 50021 // numero primo para reducir colisiones

// Un segmento dentro de la lista de un nodo del grafo
typedef struct SegmentListNode {
    Street segment;
    struct SegmentListNode *next;
} SegmentListNode;

// Una entrada en el bucket: una intersection_id y sus segmentos salientes
typedef struct GraphEntry {
    long long intersection_id;
    SegmentListNode *segments;
    struct GraphEntry *next; // chaining para manejar colisiones
} GraphEntry;

// El grafo en si: array de punteros a GraphEntry
typedef struct {
    GraphEntry *buckets[GRAPH_BUCKETS];
} IntersectionGraph;

IntersectionGraph *graph_init();
void graph_insert(IntersectionGraph *g, Street s);
SegmentListNode *graph_get(IntersectionGraph *g, long long intersection_id);
void graph_free(IntersectionGraph *g);

#endif