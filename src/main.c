#include "sample_lib.h"
#include "house.h"
#include "map.h"
#include "places.h"
#include "segments.h"
#include "path.h"
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

void createaleak() {
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}

int main(void){
  printf("*****************\nWelcome to DSA!\n*****************\n");

  // how to import and call a function
  
  printf("Factorial of 4 is %d\n", fact(4));

  // uncomment and run "make v" to see how valgrind detects memory leaks
  // createaleak();

  int user_map = 0;
  print_map_menu();
  scanf("%d", &user_map);
  printf("Has elegido el mapa: %d", user_map);

  const char *map_name = map_name_from_option(user_map);
  if (map_name == NULL) {
    printf("\n[ERROR] Mapa invalido\n");
    return 1;
  }

  HouseNode *houses = fetch_houses(map_name);
  PlaceNode *places = fetch_places(map_name);
  StreetNode *streets = fetch_streets(map_name);

  IntersectionGraph *graph = graph_init();
  StreetNode *curr = streets;
  while (curr != NULL) {
    graph_insert(graph, curr);
    curr = curr->next;
  }

  if (places == NULL) {
    printf("[ERROR] No se pudieron cargar los lugares\n");
    free_houses(houses);
    free_streets(streets);
    return 1;
  }
  if (houses == NULL) {
    printf("[ERROR] No se pudieron cargar las casas\n");
    free_places(places);
    free_streets(streets);
    return 1;
  }
  if (streets == NULL) {
    printf("[ERROR] No se pudieron cargar las calles\n");
    free_houses(houses);
    free_places(places);
    return 1;
  }

  // imprimimos el load y el total de casas
  int total_houses = count_houses(houses);
  int total_places = count_places(places);
  int total_streets = count_streets(streets);
  printf("\nCasas cargadas correctamente para %s, total: %d\n", map_name, total_houses);
  printf("Lugares cargados correctamente para %s, total: %d\n", map_name, total_places);
  printf("Calles cargadas correctamente para %s, total: %d\n", map_name, total_streets);

  Destination origin = ask_location(houses, places, "ORIGIN", "Where are you?");
  StreetNode *start_street = NULL;

  if (origin.valid) {
      start_street = closest_segment(streets, origin.latitude, origin.longitude);
      if (start_street != NULL) {
          printf("    You are at: %s\n", start_street->data.name);

          clock_t start_secuencial=clock();

          // Lab 4: busqueda lineal (mantener para comparacion en el report)
          print_connected_segments(streets, start_street);

          clock_t end_secuencial = clock();
          double time_secuencial = (double)(end_secuencial - start_secuencial) / CLOCKS_PER_SEC * 1000.0;
          printf("\n>>> TIEMPO SECUENCIAL: %f ms <<<\n", time_secuencial);

          clock_t start_grafo = clock();

          // Lab 5: busqueda con hashmap
          SegmentListNode *connected = graph_get(graph, start_street->data.to_id);

          clock_t end_grafo = clock();
          double time_grafo = (double)(end_grafo - start_grafo) / CLOCKS_PER_SEC * 1000.0;
          printf(">>> TIEMPO GRAFO: %f ms <<<\n\n", time_grafo);
          
          printf("\n    [Graph] From this street segment, you can go to:\n");
          SegmentListNode *c = connected;
          while (c != NULL) {
              printf("    - %s\n", c->segment->data.name);
              c = c->next;
          }
      }
  } else {
      printf("[ERROR] Origen no valido. Saliendo...\n");
      free_houses(houses); free_places(places); free_streets(streets);
      return 1;
  }

  // lab 6: preguntar el destino 
  Destination destination = ask_location(houses, places, "DESTINATION", "Where do you want to go?");
  StreetNode *end_street = NULL;

  if (destination.valid) {
      end_street = closest_segment(streets, destination.latitude, destination.longitude);
      if (end_street != NULL) {
          printf("    Destination street: %s\n", end_street->data.name);
      }
  } else {
      printf("[ERROR] Destino no valido. Saliendo...\n");
      free_houses(houses); free_places(places); free_streets(streets);
      return 1;
    }

  printf("\n[INFO] Origin valid: (%.6f, %.6f)\n", origin.latitude, origin.longitude);
  printf("[INFO] Destination valid: (%.6f, %.6f)\n", destination.latitude, destination.longitude);

  calculate_and_print_path(graph, streets, start_street, end_street);
  
  free_houses(houses);
  free_places(places);
  free_streets(streets);
  graph_free(graph);
  return 0;
}