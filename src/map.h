#ifndef MAP_H
#define MAP_H

#include <stdio.h>

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

/*
 * Imprime una lista numerada de sugerencias y pide al usuario que elija una.
 * Devuelve el indice 0-based elegido, o -1 si cancela o entrada invalida.
 */
int ask_from_suggestions(char suggestions[][MAX_SUGGESTION_LEN], int max);

#endif