#ifndef HOUSE_H
#define HOUSE_H

#define HOUSE_STREET_LENGTH 100
#define MAX_RESULTS 5

typedef struct House {
  char street[HOUSE_STREET_LENGTH];
  int number;
  double latitude;
  double longitude;
} House;

typedef struct HouseNodo {
  House data;
  struct HouseNodo *next;
} HouseNode;

typedef struct {
    HouseNode *node;
    int distance;
} Match;

HouseNode *init_house_list();
void append_house(HouseNode **head, HouseNode **tail, House data);
HouseNode *fetch_houses(const char *map_name);
HouseNode *search_house(HouseNode *houses, const char *house_name, int house_number);
int count_houses(HouseNode *head);
void free_houses(HouseNode *head);

#endif