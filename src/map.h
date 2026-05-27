#ifndef MAP_H
#define MAP_H

#include <stdio.h>
#include "segments.h"  
#include "house.h"     
#include "places.h"    

#define MAX_SUGERENCIAS 5
#define MAX_SUGGESTION_LEN 100

typedef struct {
    double latitude;
    double longitude;
    int valid;
} Destination;

const char *map_name_from_option(int option);
void print_map_menu(void);
void print_map_options(void);

/* Abre maps/<map_name>/<filename> e imprime el error si no existe. */
FILE *open_map_file(const char *map_name, const char *filename);


int ask_from_suggestions(char suggestions[][MAX_SUGGESTION_LEN], int max);

Destination ask_location(HouseNode *houses, PlaceNode *places, const char *title, const char *question);
#endif
