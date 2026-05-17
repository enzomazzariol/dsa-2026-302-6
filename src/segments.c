#include "segments.h"
#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14
#endif

#define EARTH_RADIUS 6371.0

StreetNode *init_street_list() {
  return NULL;
}

void append_street(StreetNode **head, StreetNode **tail, Street data) {
  StreetNode *new_node = (StreetNode *)malloc(sizeof(StreetNode));
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

StreetNode *fetch_streets(const char *map_name)
{
  FILE *file = open_map_file(map_name, "streets.txt");
  if (file == NULL)
    return NULL;

  StreetNode *head = init_street_list();
  StreetNode *tail = NULL;
  char line[512];

  while (fgets(line, sizeof(line), file))
  {
    Street s;
    int parsed = sscanf(line, "%lld,%lf,%lf,%lld,%lf,%lf,%lf,%99[^\r\n]",
                        &s.from_id, &s.from_latitude, &s.from_longitude,
                        &s.to_id, &s.to_latitude, &s.to_longitude,
                        &s.length, s.name);
    if (parsed != 8)
      continue;

    append_street(&head, &tail, s);
  }

  fclose(file);
  return head;
}

int count_streets(StreetNode *head)
{
  int total = 0;
  StreetNode *current = head;

  while (current != NULL)
  {
    total++;
    current = current->next;
  }

  return total;
}

// pasa de grados a radianes
double to_radians(double degrees) {
  return degrees * (M_PI / 180.0);
}

// pasa de radianes a grados
double to_degrees(double radians) {
  return radians * (180.0 / M_PI);
}

// haversine: distancia (km) entre dos coordenadas
double haversine(double lat1, double lon1, double lat2, double lon2) {
  double rlat1 = to_radians(lat1);
  double rlat2 = to_radians(lat2);
  double dlat = to_radians(lat2 - lat1);
  double dlon = to_radians(lon2 - lon1);

  double a = sin(dlat / 2) * sin(dlat / 2) +
             cos(rlat1) * cos(rlat2) * sin(dlon / 2) * sin(dlon / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));

  return EARTH_RADIUS * c;
}

// punto medio entre dos coordenadas
void midpoint(double lat1, double lon1, double lat2, double lon2,
              double *mid_lat, double *mid_lon) {
  double rlat1 = to_radians(lat1);
  double rlon1 = to_radians(lon1);
  double rlat2 = to_radians(lat2);
  double rlon2 = to_radians(lon2);

  double x1 = cos(rlat1) * cos(rlon1);
  double y1 = cos(rlat1) * sin(rlon1);
  double z1 = sin(rlat1);

  double x2 = cos(rlat2) * cos(rlon2);
  double y2 = cos(rlat2) * sin(rlon2);
  double z2 = sin(rlat2);

  double x = (x1 + x2) / 2.0;
  double y = (y1 + y2) / 2.0;
  double z = (z1 + z2) / 2.0;

  double lon = atan2(y, x);
  double hyp = sqrt(x * x + y * y);
  double lat = atan2(z, hyp);

  *mid_lat = to_degrees(lat);
  *mid_lon = to_degrees(lon);
}

StreetNode *closest_segment(StreetNode *streets, double latitude, double longitude)
{
  StreetNode *current = streets;
  StreetNode *closest = NULL;
  double min_dist = 99999999.0;

  // recorremos todos los segmentos y nos quedamos con el mas cercano
  // usando el punto medio del segmento + haversine
  while (current != NULL) {
    double mid_lat, mid_lon;
    midpoint(current->data.from_latitude, current->data.from_longitude,
             current->data.to_latitude, current->data.to_longitude,
             &mid_lat, &mid_lon);

    double dist = haversine(latitude, longitude, mid_lat, mid_lon);

    if (dist < min_dist) {
      min_dist = dist;
      closest = current;
    }

    current = current->next;
  }

  return closest;
}

void print_connected_segments(StreetNode *streets, StreetNode *closest)
{
  if (closest == NULL) {
    return;
  }

  long long b = closest->data.to_id;

  printf("\n    From this street segment, you can go to:\n");

  char vistos[256][STREET_NAME_LENGTH];
  int n_vistos = 0;

  StreetNode *current = streets;
  while (current != NULL) {
    if (current == closest) {
      current = current->next;
      continue;
    }

    long long f = current->data.from_id;

    if (f == b) {

      int repetido = 0;
      for (int i = 0; i < n_vistos; i++) {
        if (strcmp(vistos[i], current->data.name) == 0) {
          repetido = 1;
          break;
        }
      }

      if (!repetido && n_vistos < 256) {
        strncpy(vistos[n_vistos], current->data.name, STREET_NAME_LENGTH - 1);
        vistos[n_vistos][STREET_NAME_LENGTH - 1] = '\0';
        n_vistos++;
      }
    }

    current = current->next;
  }

  if (n_vistos == 0) {
    printf("    (none)\n");
    return;
  }

  // para cada calle de nivel 1, expandimos a nivel 2:
  // listamos calles distintas que tocan cualquier endpoint
  // de cualquier segmento que se llame igual que la de nivel 1
  for (int i = 0; i < n_vistos; i++) {
    printf("    - %s\n", vistos[i]);

    long long endpoints[8192];
    int n_end = 0;
    StreetNode *s = streets;
    while (s != NULL) {
      if (strcmp(s->data.name, vistos[i]) == 0) {
        if (n_end < 8192) endpoints[n_end++] = s->data.from_id;
        if (n_end < 8192) endpoints[n_end++] = s->data.to_id;
      }
      s = s->next;
    }

    char vistos2[256][STREET_NAME_LENGTH];
    int n_vistos2 = 0;
    s = streets;
    while (s != NULL) {
      if (strcmp(s->data.name, vistos[i]) != 0) {
        long long sf = s->data.from_id;
        long long st = s->data.to_id;
        int conecta = 0;
        for (int k = 0; k < n_end; k++) {
          if (endpoints[k] == sf || endpoints[k] == st) {
            conecta = 1;
            break;
          }
        }
        if (conecta) {
          int rep2 = 0;
          for (int k = 0; k < n_vistos2; k++) {
            if (strcmp(vistos2[k], s->data.name) == 0) {
              rep2 = 1;
              break;
            }
          }
          if (!rep2 && n_vistos2 < 256) {
            strncpy(vistos2[n_vistos2], s->data.name, STREET_NAME_LENGTH - 1);
            vistos2[n_vistos2][STREET_NAME_LENGTH - 1] = '\0';
            n_vistos2++;
          }
        }
      }
      s = s->next;
    }

    if (n_vistos2 > 0) {
      printf("        Which is connected to:\n");
      for (int k = 0; k < n_vistos2; k++) {
        printf("         - %s\n", vistos2[k]);
      }
    }
  }
}

void free_streets(StreetNode *head)
{
  StreetNode *current = head;
  while (current != NULL)
  {
    StreetNode *next = current->next;
    free(current);
    current = next;
  }
}
