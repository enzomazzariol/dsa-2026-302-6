#include "graph.h"
#include <stdlib.h>
#include <stdio.h>

static int hash(long long id) {
    if (id < 0) {
        id = -id;
    }
    return (int)(id % GRAPH_BUCKETS);
}

IntersectionGraph *graph_init(){
    IntersectionGraph *g = malloc(sizeof(IntersectionGraph));
    if (g == NULL) {
        printf("[ERROR] No se pudo asignar memoria para el grafo\n");
        return NULL;
    }
    for(int i = 0; i < GRAPH_BUCKETS; i++) {
        g->buckets[i] = NULL;
    }
    return g;
}

void graph_insert(IntersectionGraph *g, StreetNode *s) {
      int bucket = hash(s-> data.from_id);

      // Buscamos si ya existe una entrada para este intersection_id
      GraphEntry *entry = g->buckets[bucket];
      while (entry != NULL) {
          if (entry->intersection_id == s->data.from_id)
              break;
          entry = entry->next;
      }

      // Si no existe, creamos una entrada nueva y la ponemos al frente del bucket
      if (entry == NULL) {
          entry = malloc(sizeof(GraphEntry));
          if (entry == NULL) {
              printf("[ERROR] fallo al crear entrada del grafo\n");
              return;
          }
          entry->intersection_id = s->data.from_id;
          entry->segments = NULL;
          entry->next = g->buckets[bucket];  // chaining: apunta al anterior frente
          g->buckets[bucket] = entry;
      }

      // Añadimos el segmento a la lista de esta entrada
      SegmentListNode *seg_node = malloc(sizeof(SegmentListNode));
      if (seg_node == NULL) {
          printf("[ERROR] fallo al crear nodo de segmento\n");
          return;
      }
      seg_node->segment = s;
      seg_node->next = entry->segments;
      entry->segments = seg_node;
}

SegmentListNode *graph_get(IntersectionGraph *g, long long intersection_id) {
      int bucket = hash(intersection_id);

      GraphEntry *entry = g->buckets[bucket];
      while (entry != NULL) {
          if (entry->intersection_id == intersection_id)
              return entry->segments;
          entry = entry->next;
      }

      return NULL;  // no hay segmentos saliendo de esta interseccion
}

void graph_free(IntersectionGraph *g) {
    // tres niveles de free: segmentos, entradas del bucket, y el grafo
      for (int i = 0; i < GRAPH_BUCKETS; i++) {
          GraphEntry *entry = g->buckets[i];
          while (entry != NULL) {
              SegmentListNode *seg = entry->segments;
              while (seg != NULL) {
                  SegmentListNode *next_seg = seg->next;
                  free(seg);
                  seg = next_seg;
              }
              GraphEntry *next_entry = entry->next;
              free(entry);
              entry = next_entry;
          }
      }
      free(g);
}
