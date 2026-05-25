#include "path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- FUNCIONES DE LA COLA (QUEUE) ---

void enqueue(Queue *q, StreetList path) {
    QueueNode *node = malloc(sizeof(QueueNode));
    node->path = path;
    node->next = NULL;
    if (q->tail == NULL) {
        q->head = node;
        q->tail = node;
    } else {
        q->tail->next = node;
        q->tail = node;
    }
}

StreetList dequeue(Queue *q) {
    StreetList path = {NULL, 0};
    if (q->head == NULL) return path;

    QueueNode *temp = q->head;
    path = temp->path;
    
    q->head = q->head->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }
    
    free(temp);
    return path;
}

// --- ALGORITMO BFS ---

StreetList BFS(StreetNode *graph, StreetNode *fromStreet, StreetNode *toStreet) {
    // 1. create an empty queue of street lists, Q
    Queue q = {NULL, NULL};
    StreetList no_path = {NULL, 0};

    // 2. create a street list [fromStreet], initial_path
    StreetList initial_path;
    initial_path.count = 1;
    initial_path.streets = malloc(sizeof(StreetNode*));
    initial_path.streets[0] = fromStreet;

    // 3. enqueue initial_path into Q
    enqueue(&q, initial_path);

    // 4. create a street list [], visited
    // En C, lo representamos con un array para guardar qué calles hemos visto
    int total_streets = count_streets(graph);
    StreetNode **visited = malloc(total_streets * sizeof(StreetNode*));
    int visited_count = 0;

    // 5. while Q is not empty:
    while (q.head != NULL) {
        
        // 6. path = dequeue(Q)
        StreetList path = dequeue(&q);
        
        // 7. current_street = path[-1] (la última calle añadida al camino)
        StreetNode *current_street = path.streets[path.count - 1];

        // 8. if current_street == toStreet: return path
        if (current_street == toStreet) {
            // Limpiamos la memoria restante en la cola antes de salir
            while (q.head != NULL) {
                StreetList p = dequeue(&q);
                free(p.streets);
            }
            free(visited);
            return path;
        }

        // 9. if current_street not in visited:
        int is_visited = 0;
        for (int i = 0; i < visited_count; i++) {
            if (visited[i] == current_street) {
                is_visited = 1;
                break;
            }
        }

        if (!is_visited) {
            // 10. add current_street to visited
            visited[visited_count++] = current_street;

            // 11. for connected_street in intersections_graph
            long long f_id = current_street->data.from_id;
            long long t_id = current_street->data.to_id;

            StreetNode *conn = graph;
            while (conn != NULL) {
                if (conn != current_street) {
                    // Verificamos si comparten intersección (conecta por from_id o to_id)
                    if (conn->data.from_id == t_id || conn->data.to_id == t_id ||
                        conn->data.from_id == f_id || conn->data.to_id == f_id) {
                        
                        // 12. if connected_street not in visited:
                        int conn_visited = 0;
                        for (int i = 0; i < visited_count; i++) {
                            if (visited[i] == conn) {
                                conn_visited = 1;
                                break;
                            }
                        }

                        if (!conn_visited) {
                            // 13. new_path = path + [connected_street]
                            StreetList new_path;
                            new_path.count = path.count + 1;
                            new_path.streets = malloc(new_path.count * sizeof(StreetNode*));
                            
                            // Copiamos el camino anterior
                            for (int i = 0; i < path.count; i++) {
                                new_path.streets[i] = path.streets[i];
                            }
                            // Añadimos la nueva calle conectada
                            new_path.streets[path.count] = conn;

                            // 14. enqueue new_path into Q
                            enqueue(&q, new_path);
                        }
                    }
                }
                conn = conn->next;
            }
        }
        // Liberamos la memoria del camino evaluado porque ya lo copiamos a sus hijos
        free(path.streets); 
    }

    // 15. return NULL
    free(visited);
    return no_path; 
}

// --- FUNCIÓN QUE LLAMA EL MAIN ---
void calculate_and_print_path(StreetNode *graph, StreetNode *fromStreet, StreetNode *toStreet) {
    if (fromStreet == NULL || toStreet == NULL) {
        printf("[ERROR] Calles invalidas para calcular el camino.\n");
        return;
    }

    printf("\nCalculating path...\n");
    
    StreetList path = BFS(graph, fromStreet, toStreet);

    if (path.count > 0) {
        printf("\n--- STEP BY STEP DIRECTIONS ---\n");
        char last_street[100] = "";
        
        for (int i = 0; i < path.count; i++) {
            // Imprimimos la calle solo si es distinta a la anterior
            if (strcmp(path.streets[i]->data.name, last_street) != 0) {
                printf(" -> Go along %s\n", path.streets[i]->data.name);
                strncpy(last_street, path.streets[i]->data.name, 99);
            }
        }
        printf(" -> Arrive at destination.\n");
        free(path.streets); // Liberar memoria del camino devuelto
    } else {
        printf("\n[INFO] No route found between the specified points.\n");
    }
}