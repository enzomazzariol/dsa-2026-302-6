#include "places.h"
#include "map.h"
#include "algoritmos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

PlaceNode *init_place_list() {
  return NULL;
}

void append_place(PlaceNode **head, PlaceNode **tail, Place data) {
  PlaceNode *new_node = (PlaceNode *)malloc(sizeof(PlaceNode));
  if (new_node == NULL) {
    printf("[ERROR] fallo en la asignacion de memoria\n");
    return;
  }

  new_node->data = data;
  new_node->next = NULL;

  if (*head == NULL) {
    *head = new_node;
    *tail = new_node;
  } else {
    (*tail)->next = new_node;
    *tail = new_node;
  }
}

PlaceNode *fetch_places(const char *map_name)
{
  FILE *file = open_map_file(map_name, "places.txt");
  if (file == NULL)
    return NULL;

  PlaceNode *head = init_place_list();
  PlaceNode *tail = NULL;
  char line[256];

  while (fgets(line, sizeof(line), file))
  {
    Place p;
    int parsed = sscanf(line, "%36[^,],%49[^,],%49[^,],%lf,%lf",
                        p.id, p.place, p.amenity, &p.latitude, &p.longitude);
    if (parsed != 5)
      continue;

    append_place(&head, &tail, p);
  }

  fclose(file);
  return head;
}

PlaceNode *search_place(PlaceNode *places, const char *place_name)
{
  PlaceNode *current = places;
  while (current != NULL) {
    if (strcasecmp(current->data.place, place_name) == 0) {
      printf("    Found at (%.6f, %.6f)\n", current->data.latitude, current->data.longitude);
      return current;
    }
    current = current->next;
  }

  char sugeridos[MAX_SUGERENCIAS][MAX_SUGGESTION_LEN];
  int distancias[MAX_SUGERENCIAS];

  for (int i = 0; i < MAX_SUGERENCIAS; i++) {
    sugeridos[i][0] = '\0';
    distancias[i] = 9999;
  }

  current = places;
  while (current != NULL) {
    const char *nombre = current->data.place;
    int dist = levenshtein_distance(place_name, nombre);

    int ya_esta = 0;
    for (int i = 0; i < MAX_SUGERENCIAS; i++) {
      if (strcasecmp(sugeridos[i], nombre) == 0) {
        ya_esta = 1;
        break;
      }
    }
    if (ya_esta) {
      current = current->next;
      continue;
    }

    for (int i = 0; i < MAX_SUGERENCIAS; i++) {
      if (dist < distancias[i]) {
        for (int j = MAX_SUGERENCIAS - 1; j > i; j--) {
          distancias[j] = distancias[j - 1];
          strncpy(sugeridos[j], sugeridos[j - 1], MAX_SUGGESTION_LEN - 1);
          sugeridos[j][MAX_SUGGESTION_LEN - 1] = '\0';
        }
        distancias[i] = dist;
        strncpy(sugeridos[i], nombre, MAX_SUGGESTION_LEN - 1);
        sugeridos[i][MAX_SUGGESTION_LEN - 1] = '\0';
        break;
      }
    }

    current = current->next;
  }

  printf("[ERROR] Place not found. Similar places:\n");
  int opcion = ask_from_suggestions(sugeridos, MAX_SUGERENCIAS);
  if (opcion >= 0)
    return search_place(places, sugeridos[opcion]);
  return NULL;
}

int count_places(PlaceNode *head)
{
  int total = 0;
  PlaceNode *current = head;

  while (current != NULL)
  {
    total++;
    current = current->next;
  }

  return total;
}

void free_places(PlaceNode *head)
{
  PlaceNode *current = head;
  while (current != NULL)
  {
    PlaceNode *next = current->next;
    free(current);
    current = next;
  }
}
