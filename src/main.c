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

// void compare_house_name(*char );
Destination ask_location(HouseNode *houses, PlaceNode *places, const char *title, const char *question) {
  Destination dest = {0.0, 0.0, 0};
  int option;


  printf("\n---- %s ----\n", title);
  printf("%s Address (1), Place (2) or Coordinate (3): ", question);

  if (scanf("%d", &option) != 1) return dest;

  switch (option) {
    case 1: {
      char street_name[256];
      int street_number;
      printf("Enter street name: \n");
      scanf(" %255[^\n]", street_name);
      printf("Enter street number: \n");
      scanf("%d", &street_number);

      HouseNode *found = search_house(houses, street_name, street_number);
      if (found != NULL) {
        dest.latitude = found->data.latitude;
        dest.longitude = found->data.longitude;
        dest.valid = 1;
      }
      break;
    }
    case 2: { 
      char place_name[256];
      printf("Enter place name (e.g. \"Universitat Pompeu Fabra-Campus del Poblenou\"): \n");
      scanf(" %255[^\n]", place_name);

      PlaceNode *found = search_place(places, place_name);
      if (found != NULL) {
        dest.latitude = found->data.latitude;
        dest.longitude = found->data.longitude;
        dest.valid = 1;
      }
      break;
    }
    case 3: { // COORDINATE
      printf("Enter latitude and longitude (format: lat,lon): \n");
      if (scanf("%lf,%lf", &dest.latitude, &dest.longitude) == 2) {
        dest.valid = 1;
      } else {
        printf("[ERROR] Invalid coordinate format.\n");
      }
      break;
    }
    default:
      printf("[ERROR] Invalid option.\n");
      break;
  }

  return dest;
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