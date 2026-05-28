# Report

## Runtime complexity analysis of initializing the intersections map in Big-O.

El programa inicialitza les interseccions del mapa mitjançant, un graf, per fer-ho el programa utilitza un bucle while (a main.c) que itera tota la llista enllaçada i afegeix cada segment al graf. Aleshores la complexitat d'aquesta activitat, és:

- Millor cas: O(n). El programa itera cada segment (O(1)) i els fica al graf, fins haver-los iterat tots.
- Cas mitjà: Igual O(n)
- Pitjor cas: O(n^2). Si hi ha col·lisions.

## Runtime complexity analysis of finding the coordinates of a street or place given the name in Big-O.

El programa itera la llista enllaçada fins a trobar el carrer o lloc introduït per l'usuari, això ho fa mitjançant les funcions search_house i search_place. D'aquesta manera la complexitat és:

- Millor cas: O(1). L'element que buscavem era el primer de la llista.
- Cas mitjà: O(n). L'element es trobava a una posició aleatoria.
- Pitjor cas: O(n). l'element es trobava a l'última posició.

## Runtime complexity analysis of your path-finding algorithm in Big-O.

Per trobar el camí més curt per arribar a la destinació que indica l'usuari, el programa utilitza l'algoritme BFS, a StreetList BFS. Sigui V el nombre de vèrtex (interseccions) i E el nombre de nodes (carrers) la complexitat es la següent:

- Millor cas: O(1). El usuario vol anar des de la posició en la que està, a aquesta mateixa.
- Cas mitjà: O(V+E). Per cada carrer que visita el programa mira també les seves interseccions, d'aquesta manera hem de sumar els carrers visitats amb les interseccions d'aquests.
- Pitjor cas: O(V+E). En aquest cas el programa ha de visitar tots els carrers i per tant les seves interseccions.

## A plot comparing the latency to find connected streets by sequentially looking through the list (lab 3) compared to using the intersections map (lab 4), depending on the map size. - Experimentally determine the results by measuring multiple times your program's behaviour with different relevant scenarios in the same machine. Include your raw data in the report, besides the plot. - Explain the results.

per al mapa 1

> > > TIEMPO SECUENCIAL: 0.248000 ms <<<
> > > TIEMPO GRAFO: 0.003000 ms <<<

per al mapa 2

> > > TIEMPO SECUENCIAL: 0.139000 ms <<<
> > > TIEMPO GRAFO: 0.002000 ms <<<

per al mapa 3

> > > TIEMPO SECUENCIAL: 0.597000 ms <<<
> > > TIEMPO GRAFO: 0.002000 ms <<<

per al mapa 4

> > > TIEMPO SECUENCIAL: 0.525000 ms <<<
> > > TIEMPO GRAFO: 0.001000 ms <<<

per al mapa 5

> > > TIEMPO SECUENCIAL: 0.234000 ms <<<
> > > TIEMPO GRAFO: 0.001000 ms <<<

per al mapa 6

> > > TIEMPO SECUENCIAL: 2.973000 ms <<<
> > > TIEMPO GRAFO: 0.002000 ms <<<

Aquest fet evidència la millora pel que fa a l'eficiència entre la cerca seqüencial i la utilització d'un mapa de interseccions. Aquest fet esta estrictamente relacionat amb la complexitat del primer cas O(n), que ha de recorrer la llista enllaçada fins a trobar l'element, amb la del segon cas O(1), en que es permet l'accés constant.

## - A plot comparing the latency to find a path between two points finding connected streets sequentially looking through the list compared to using the intersections map, depending on the map size.- Experimentally determine the results by measuring multiple times your program's behaviour with different relevant scenarios in the same machine. Include your raw data in the report, besides the plot.- Explain the results.

per al mapa 1

> > > TIEMPO BFS: 0.052000 ms <<<
> > > TIEMPO BFS_slow: 0.032000 ms <<<

per al mapa 2

> > > TIEMPO BFS: 0.056000 ms <<<
> > > TIEMPO BFS_slow: 0.027000 ms <<<

per al mapa 3

> > > TIEMPO BFS: 0.460000 ms <<<
> > > TIEMPO BFS_slow: 0.906000 ms <<<

per al mapa 4

> > > TIEMPO BFS: 1.520000 ms <<<
> > > TIEMPO BFS_slow: 20.664000 ms <<<

per al mapa 5

> > > TIEMPO BFS: 3.634000 ms <<<
> > > TIEMPO BFS_slow: 174.395000 ms <<<

per al mapa 6

> > > TIEMPO BFS: 0.055000 ms <<<
> > > TIEMPO BFS_slow: 10587.052000 ms <<<

Aquests resultats evidèncien la millora del temps de l'algoritme utilitzant un graf en comptes d'una llista enllaçada, això si per a un nombre d'elements gran. Als primers casos on hi havia un nombre insignificant de carrers el programa tarda menys en recorrer tota la llista que no fent ús del graf, però a mesura que els valors creixen es veu l'aventatge de l'ús d'un graf.

## - A plot comparing the latency to find a path between two points that are close in the map compared to two points that are very far in the map, for different distances. - Experimentally determine the results by measuring multiple times your program's behaviour with different relevant scenarios in the same machine. Include your raw data in the report, besides the plot. - Explain the results.
