#include "path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "graph.h"

#define EARTH_RADIUS 6371.0

static double to_radians_path(double deg) { return deg * (3.14159265358979323846 / 180.0); }

// Convierte lat/lon a coordenadas x/y usando lat_ref/lon_ref como origen
static void latlon_to_xy(double lat_ref, double lon_ref, double lat, double lon, double *x, double *y) {
    double lat_ref_rad = to_radians_path(lat_ref);
    *x = EARTH_RADIUS * to_radians_path(lon - lon_ref) * cos(lat_ref_rad);
    *y = EARTH_RADIUS * to_radians_path(lat - lat_ref);
}

// Devuelve "left", "right" o "straight" comparando dos segmentos consecutivos
static const char *turn_direction(StreetNode *prev, StreetNode *next) {
    double ax = 0, ay = 0, bx, by, cx, cy;
    // A = from del segmento anterior, B = to del anterior (= from del siguiente), C = to del siguiente
    // Usamos B como referencia para la conversion
    double lat_b = prev->data.to_latitude;
    double lon_b = prev->data.to_longitude;

    latlon_to_xy(lat_b, lon_b, prev->data.from_latitude, prev->data.from_longitude, &ax, &ay);
    bx = 0; by = 0;  // B es el origen de referencia
    latlon_to_xy(lat_b, lon_b, next->data.to_latitude, next->data.to_longitude, &cx, &cy);

    double cross = (bx - ax) * (cy - by) - (by - ay) * (cx - bx);

    if (cross > 0) return "left";
    if (cross < 0) return "right";
    return "straight";
}

void enqueue(Queue *q, StreetList path)
{
    QueueNode *node = malloc(sizeof(QueueNode));
    node->path = path;
    node->next = NULL;
    if (q->tail == NULL)
    {
        q->head = node;
        q->tail = node;
    }
    else
    {
        q->tail->next = node;
        q->tail = node;
    }
}

StreetList dequeue(Queue *q)
{
    StreetList path = {NULL, 0};
    if (q->head == NULL)
        return path;

    QueueNode *temp = q->head;
    path = temp->path;

    q->head = q->head->next;
    if (q->head == NULL)
    {
        q->tail = NULL;
    }

    free(temp);
    return path;
}

StreetList BFS(IntersectionGraph *mapa_intersecciones, StreetNode *fromStreet, StreetNode *toStreet)
{
    Queue q = {NULL, NULL};
    StreetList no_path = {NULL, 0};

    StreetList initial_path;
    initial_path.count = 1;
    initial_path.streets = malloc(sizeof(StreetNode *));
    initial_path.streets[0] = fromStreet;

    enqueue(&q, initial_path);

    int total_streets = 50000;
    StreetNode **visited = malloc(total_streets * sizeof(StreetNode *));
    int visited_count = 0;

    while (q.head != NULL)
    {

        StreetList path = dequeue(&q);

        StreetNode *current_street = path.streets[path.count - 1];

        if (current_street == toStreet)
        {

            while (q.head != NULL)
            {
                StreetList p = dequeue(&q);
                free(p.streets);
            }
            free(visited);
            return path;
        }

        int is_visited = 0;
        for (int i = 0; i < visited_count; i++)
        {
            if (visited[i] == current_street)
            {
                is_visited = 1;
                break;
            }
        }

        if (!is_visited)
        {
            visited[visited_count++] = current_street;

            long long t_id = current_street->data.to_id;

            SegmentListNode *vecinos = graph_get(mapa_intersecciones, t_id);
            while (vecinos != NULL)
            {
                StreetNode *conn = vecinos->segment;
                if (conn != current_street)
                {

                    int conn_visited = 0;
                    for (int i = 0; i < visited_count; i++)
                    {
                        if (visited[i] == conn)
                        {
                            conn_visited = 1;
                            break;
                        }
                    }

                    if (!conn_visited)
                    {
                        StreetList new_path;
                        new_path.count = path.count + 1;
                        new_path.streets = malloc(new_path.count * sizeof(StreetNode *));

                        for (int i = 0; i < path.count; i++)
                        {
                            new_path.streets[i] = path.streets[i];
                        }
                        new_path.streets[path.count] = conn;

                        enqueue(&q, new_path);
                    }
                }

                vecinos = vecinos->next;
            }
        }
        free(path.streets);
    }

    free(visited);
    return no_path;
}

void calculate_and_print_path(IntersectionGraph *graph, StreetNode *street_list, StreetNode *fromStreet, StreetNode *toStreet)
{
    if (fromStreet == NULL || toStreet == NULL)
    {
        printf("[ERROR] Calles invalidas para calcular el camino.\n");
        return;
    }

    printf("\nCalculating path...\n");

    clock_t start_bfs = clock();
    StreetList path = BFS(graph, fromStreet, toStreet);
    clock_t end_bfs = clock();
    double time_bfs = (double)(end_bfs - start_bfs) / CLOCKS_PER_SEC * 1000.0;

    clock_t start_slow = clock();
    StreetList path_slow = BFS_slow(street_list, fromStreet, toStreet);
    clock_t end_slow = clock();
    double time_slow = (double)(end_slow - start_slow) / CLOCKS_PER_SEC * 1000.0;

    if (path_slow.count > 0)
        free(path_slow.streets);
    printf(">>> TIEMPO BFS: %f ms <<<\n", time_bfs);
    printf(">>> TIEMPO BFS_slow: %f ms <<<\n", time_slow);

    if (path.count > 0)
    {
        printf("\n--- ROUTE ---\n");
        printf("  Start at %s\n", path.streets[0]->data.name);

        int i = 1;
        while (i < path.count)
        {
            int group_start = i;
            char *name = path.streets[i]->data.name;
            double total_length = 0;
            while (i < path.count && strcmp(path.streets[i]->data.name, name) == 0)
            {
                total_length += path.streets[i]->data.length;
                i++;
            }
            const char *dir = turn_direction(path.streets[group_start - 1], path.streets[group_start]);
            if (i < path.count)
                printf("  Turn %s to %s and continue for %dm\n", dir, name, (int)total_length);
            else
                printf("  Turn %s to %s for %dm\n", dir, name, (int)total_length);
        }
        printf("  You have arrived to your destination\n");

        free(path.streets);
    }
    else
    {
        printf("\n[INFO] No route found between the specified points.\n");
    }
}

StreetList BFS_slow(StreetNode *street_list, StreetNode *fromStreet, StreetNode *toStreet)
{
    Queue q = {NULL, NULL};
    StreetList no_path = {NULL, 0};

    StreetList initial_path;
    initial_path.count = 1;
    initial_path.streets = malloc(sizeof(StreetNode *));
    initial_path.streets[0] = fromStreet;

    enqueue(&q, initial_path);

    int total_streets = 50000;
    StreetNode **visited = malloc(total_streets * sizeof(StreetNode *));
    int visited_count = 0;

    while (q.head != NULL)
    {
        StreetList path = dequeue(&q);
        StreetNode *current_street = path.streets[path.count - 1];

        if (current_street == toStreet)
        {
            while (q.head != NULL)
            {
                StreetList p = dequeue(&q);
                free(p.streets);
            }
            free(visited);
            return path;
        }

        int is_visited = 0;
        for (int i = 0; i < visited_count; i++)
        {
            if (visited[i] == current_street)
            {
                is_visited = 1;
                break;
            }
        }

        if (!is_visited)
        {
            visited[visited_count++] = current_street;

            long long f_id = current_street->data.from_id;
            long long t_id = current_street->data.to_id;

            StreetNode *conn = street_list;
            while (conn != NULL)
            {
                if (conn != current_street)
                {
                    // Comprobamos si comparten alguna intersección
                    if (conn->data.from_id == t_id || conn->data.to_id == t_id ||
                        conn->data.from_id == f_id || conn->data.to_id == f_id)
                    {

                        int conn_visited = 0;
                        for (int i = 0; i < visited_count; i++)
                        {
                            if (visited[i] == conn)
                            {
                                conn_visited = 1;
                                break;
                            }
                        }

                        if (!conn_visited)
                        {
                            StreetList new_path;
                            new_path.count = path.count + 1;
                            new_path.streets = malloc(new_path.count * sizeof(StreetNode *));

                            for (int i = 0; i < path.count; i++)
                            {
                                new_path.streets[i] = path.streets[i];
                            }
                            new_path.streets[path.count] = conn;

                            enqueue(&q, new_path);
                        }
                    }
                }
                conn = conn->next; // Siguiente calle de la lista global
            }
        }
        free(path.streets);
    }

    free(visited);
    return no_path;
}