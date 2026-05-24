#include "house.h"
#include "map.h"
#include "algoritmos.h"
#include "abreviaturas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

HouseNode *init_house_list() {
  return NULL;
}

void append_house(HouseNode **head, HouseNode **tail, House data) {
  HouseNode *new_node = (HouseNode *)malloc(sizeof(HouseNode));
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

HouseNode *fetch_houses(const char *map_name)
{
  FILE *file = open_map_file(map_name, "houses.txt");
  if (file == NULL)
    return NULL;

  HouseNode *head = init_house_list();
  HouseNode *tail = NULL;
  char line[256];

  while (fgets(line, sizeof(line), file))
  {
    House h;
    int parsed = sscanf(line, "%99[^,],%d,%lf,%lf", h.street, &h.number, &h.latitude, &h.longitude);
    if (parsed != 4)
      continue;

    append_house(&head, &tail, h);
  }

  fclose(file);
  return head;
}

HouseNode* search_house(HouseNode *houses, const char *house_name, int house_number) {
  // Expandimos la abreviatura una sola vez antes del loop para no recalcularla en cada iteracion
  char abbr[100];
  abreviaturas(house_name, abbr, sizeof(abbr));

  HouseNode *current = houses;
  int street_found = 0;

  // Primer recorrido: buscamos la calle y el numero exacto simultaneamente
  while (current != NULL) {
    // Aceptamos tanto el nombre original como su forma expandida (ej: "c Exemple" -> "carrer Exemple")
    int calle_ok = strcasecmp(current->data.street, house_name) == 0 ||
                   strcasecmp(current->data.street, abbr) == 0;

    if (calle_ok) {
      street_found = 1;
      if (current->data.number == house_number) {
        printf("House found: Latitud = %.6f, Longitud = %.6f\n", current->data.latitude, current->data.longitude);
        return current;
      }
    }
    current = current->next;
  }

  if (street_found) {
    // La calle existe pero el numero no: listamos los disponibles y pedimos uno nuevo
    printf("Number not found in the specified street. ");
    printf("Available numbers in this street: ");
    current = houses;
    while (current != NULL) {
      if (strcasecmp(current->data.street, house_name) == 0 || strcasecmp(current->data.street, abbr) == 0) {
        printf("%d ", current->data.number);
      }
      current = current->next;
    }
    printf("\nEnter the correct house number: ");
    int new_house_number = 0;
    if (scanf("%d", &new_house_number) == 1) {
      return search_house(houses, house_name, new_house_number);
    } else {
      printf("[ERROR] Numero invalido\n");
    }
  } else {
    // La calle no existe: buscamos las mas parecidas usando distancia de Levenshtein
    char sugeridas[MAX_SUGERENCIAS][MAX_SUGGESTION_LEN];
    int distancias[MAX_SUGERENCIAS];

    for (int i = 0; i < MAX_SUGERENCIAS; i++) {
      sugeridas[i][0] = '\0';
      distancias[i] = 9999;
    }

    current = houses;
    while (current != NULL) {
      const char *calle = current->data.street;
      int dist = levenshtein_distance(house_name, calle);

      // Evitamos agregar la misma calle mas de una vez (hay muchas casas por calle)
      int ya_esta = 0;
      for (int i = 0; i < MAX_SUGERENCIAS; i++) {
        if (strcasecmp(sugeridas[i], calle) == 0) {
          ya_esta = 1;
          break;
        }
      }
      if (ya_esta) {
        current = current->next;
        continue;
      }

      // Insertion sort parcial: mantenemos el ranking de las N mejores ordenado por distancia
      for (int i = 0; i < MAX_SUGERENCIAS; i++) {
        if (dist < distancias[i]) {
          // Desplazamos las peores una posicion hacia abajo para hacer lugar
          for (int j = MAX_SUGERENCIAS - 1; j > i; j--) {
            distancias[j] = distancias[j - 1];
            strncpy(sugeridas[j], sugeridas[j - 1], MAX_SUGGESTION_LEN - 1);
            sugeridas[j][MAX_SUGGESTION_LEN - 1] = '\0';
          }
          distancias[i] = dist;
          strncpy(sugeridas[i], calle, MAX_SUGGESTION_LEN - 1);
          sugeridas[i][MAX_SUGGESTION_LEN - 1] = '\0';
          break;
        }
      }

      current = current->next;
    }

    printf("[ERROR] Street not found. Similar streets:\n");
    int opcion = ask_from_suggestions(sugeridas, MAX_SUGERENCIAS);
    if (opcion >= 0)
      return search_house(houses, sugeridas[opcion], house_number);

  }

  return NULL;
}

int count_houses(HouseNode *head){ 
  int total = 0;
  HouseNode *current = head;

  while (current != NULL)
  {
    total++;
    current = current->next;
  }

  return total;
}

void free_houses(HouseNode *head)
{
  HouseNode *current = head;
  while (current != NULL)
  {
    HouseNode *next = current->next;
    free(current);
    current = next;
  }
}