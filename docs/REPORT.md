# Report

## Runtime complexity analysis of initializing the intersections map in Big-O.

El programa inicialitza les interseccions del mapa mitjançant, un graf, per fer-ho el programa utilitza un bucle while (a main.c) que itera tota la llista enllaçada i afegeix cada segment al graf. Aleshores la complexitat d'aquesta activitat, és:

-Millor cas: O(n). El programa itera cada segment (O(1)) i els fica al graf, fins haver-los iterat tots.
-Cas mitjà: Igual O(n)
-Pitjor cas: O(n^2). Si hi ha col·lisions.

## Runtime complexity analysis of finding the coordinates of a street or place given the name in Big-O.

El programa itera la llista enllaçada fins a trobar el carrer o lloc introduït per l'usuari, això ho fa mitjançant les funcions search_house i search_place. D'aquesta manera la complexitat és:

-Millor cas: O(1). L'element que buscavem era el primer de la llista.
-Cas mitjà: O(n). L'element es trobava a una posició aleatoria.
-Pitjor cas: O(n). l'element es trobava a l'última posició.

## Runtime complexity analysis of your path-finding algorithm in Big-O.

Per trobar el camí més curt per arribar a la destinació que indica l’usuari, el programa utilitza l’algoritme BFS a StreetList BFS. Sigui V el nombre de vèrtexs (interseccions) i E el nombre d’arestes (carrers), la complexitat és la següent

-Millor cas: O(1). L’usuari vol anar des de la posició en què es troba fins a la mateixa posició.
-Cas mitjà: O(V+E). Per cada carrer que visita el programa mira també les seves interseccions, d'aquesta manera hem de sumar els carrers visitats amb les interseccions d'aquests.
-Pitjor cas: O(V+E). En aquest cas el programa ha de visitar tots els carrers i per tant les seves interseccions.

##   A plot comparing the latency to find connected streets by sequentially looking through the list (lab 3) compared to using the intersections map (lab 4), depending on the map size. - Experimentally determine the results by measuring multiple times your program's behaviour with different relevant scenarios in the same machine. Include your raw data in the report, besides the plot. - Explain the results.
     
per al mapa 1 

>>> TIEMPO SECUENCIAL: 0.248000 ms <<<
>>> TIEMPO GRAFO: 0.003000 ms <<<

per al mapa 2

>>> TIEMPO SECUENCIAL: 0.139000 ms <<<
>>> TIEMPO GRAFO: 0.002000 ms <<<

per al mapa 3

>>> TIEMPO SECUENCIAL: 0.597000 ms <<<
>>> TIEMPO GRAFO: 0.002000 ms <<<

per al mapa 4

>>> TIEMPO SECUENCIAL: 0.525000 ms <<<
>>> TIEMPO GRAFO: 0.001000 ms <<<

per al mapa 5

>>> TIEMPO SECUENCIAL: 0.234000 ms <<<
>>> TIEMPO GRAFO: 0.001000 ms <<<

per al mapa 6

>>> TIEMPO SECUENCIAL: 2.973000 ms <<<
>>> TIEMPO GRAFO: 0.002000 ms <<<

Aquest fet evidencia la millora pel que fa a l’eficiència entre la cerca seqüencial i la utilització d’un mapa d’interseccions. Aquest fet està estrictament relacionat amb la complexitat del primer cas, O(n), en què s’ha de recórrer la llista enllaçada fins a trobar l’element, i la del segon cas, O(1), en què es permet l’accés constant.

##   - A plot comparing the latency to find a path between two points finding connected streets sequentially looking through the list compared to using the intersections map, depending on the map size.- Experimentally determine the results by measuring multiple times your program's behaviour with different relevant scenarios in the same machine. Include your raw data in the report, besides the plot.- Explain the results.

per al mapa 1

>>> TIEMPO BFS: 0.052000 ms <<<
>>> TIEMPO BFS_slow: 0.032000 ms <<<

per al mapa 2

>>> TIEMPO BFS: 0.056000 ms <<<
>>> TIEMPO BFS_slow: 0.027000 ms <<<

per al mapa 3

>>> TIEMPO BFS: 0.460000 ms <<<
>>> TIEMPO BFS_slow: 0.906000 ms <<<

per al mapa 4

>>> TIEMPO BFS: 1.520000 ms <<<
>>> TIEMPO BFS_slow: 20.664000 ms <<<

per al mapa 5

>>> TIEMPO BFS: 3.634000 ms <<<
>>> TIEMPO BFS_slow: 174.395000 ms <<<

per al mapa 6

>>> TIEMPO BFS: 0.055000 ms <<<
>>> TIEMPO BFS_slow: 10587.052000 ms <<<

Aquests resultats evidencien la millora del temps de l’algoritme utilitzant un graf en comptes d’una llista enllaçada, això sí, per a un nombre d’elements gran. En els primers casos, on hi havia un nombre insignificant de carrers, el programa triga menys a recórrer tota la llista que no pas fent ús del graf, però a mesura que els valors creixen es veu l’avantatge de l’ús d’un graf

## - A plot comparing the latency to find a path between two points that are close in the map compared to two points that are very far in the map, for different distances. - Experimentally determine the results by measuring multiple times your program's behaviour with different relevant scenarios in the same machine. Include your raw data in the report, besides the plot. - Explain the results.

Per determinar la diferència en la latència per trobar un camí entre dos punts que estan prop i camins que estan més junts, utilitzarem el mapa 6, ja que aquest es el més gran i evidencia més aquest canvi. Per mostrar el fet mostrarem tres casos: carrers que estan conectats, carrers que es troben a una diferència mitjana i carrers que es troben lluny.

-Prop.
 Des de carrer de calàbria fins a carrer de paris :

>>> TIEMPO BFS: 0.082000 ms <<<

 Des de carrer de Carrer Major fins a Carrer de Sant Jaume:


>>> TIEMPO BFS: 0.066000 ms <<<

-Distancia mitja.

Des de Plaça Pau Vila fins a carrer de Còrsega:

>>> TIEMPO BFS: 16.182000 ms <<<

Des de Avinguda de Pau Negre fins a Plaça Major

>>> TIEMPO BFS: 29.118000 ms <<<

-Distancia llunyana.

Des de Autopista la Mediterrània fins a Riera d'en Nofre

>>> TIEMPO BFS: 228.717000 ms <<<

Des de Avinguda de la Unitat fins a carrer de Pepe Rubianes.

>>> TIEMPO BFS: 398.349000 ms <<<

Com podem observar amb aquests resultats la latència es dispara a mesura que creix la distancia, això és deu a la naturalesa del algoritme BFS, que va fent cercles, cada cop més grans fins a trobar la destinació.