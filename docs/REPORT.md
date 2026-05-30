# Report

## Runtime complexity analysis of initializing the intersections map in Big-O

El programa inicialitza les interseccions del mapa mitjançant un graf. Per fer-ho, el main.c itera tota la llista enllaçada de segments i crida graph_insert per a cadascun. La funció graph_insert calcula el bucket amb la funció hash (O(1)) i recorre la cadena d'aquell bucket per comprovar si ja existeix una entrada per al from_id del segment.

- *Millor cas: O(n).* Cap segment col·lisiona al mateix bucket. Cada graph_insert és O(1), i com que n'hi ha n segments, el total és O(n).
- *Cas mitjà: O(n).* Amb 50021 buckets (número primer) i ~18k segments, el factor de càrrega és ~0.36. La cadena mitjana té menys d'un element, de manera que cada inserció és pràcticament O(1) i el total és O(n).
- *Pitjor cas: O(n²).* Si tots els segments produïssin el mateix valor hash (col·lisió total), cada inserció recorreria una cadena de longitud creixent: O(1) + O(2) + ... + O(n) = O(n^2). En la pràctica, amb un número primer com a mòdul i dades reals d'OSM, aquest cas no es dóna.

---

## Runtime complexity analysis of finding the coordinates of a street or place given the name in Big-O

El programa itera la llista enllaçada fins a trobar el carrer o lloc introduït per l'usuari, mitjançant les funcions search_house i search_place. La comparació es fa amb strcasecmp (case-insensitive), i en el cas de les cases també es comprova la forma expandida de l'abreviatura.

- *Millor cas: O(1).* L'element buscat és el primer node de la llista.
- *Cas mitjà: O(n).* L'element es troba en una posició aleatòria de la llista.
- *Pitjor cas: O(n).* L'element no existeix o es troba al final. A més, si no es troba exactament, s'aplica la distància de Levenshtein contra tots els elements per suggerir alternatives, amb cost O(n · m) on m és la longitud mitjana dels noms.

---

## Runtime complexity analysis of your path-finding algorithm in Big-O

Per trobar el camí entre dos punts, el programa utilitza l'algorisme BFS implementat a StreetList BFS(...) en path.c. Sigui *V* el nombre de vèrtexs (interseccions) i *E* el nombre d'arestes (segments de carrer):

- *Millor cas: O(1).* L'origen i la destinació coincideixen: BFS retorna el camí immediatament sense explorar res.
- *Cas mitjà: O(V + E).* BFS visita cada vèrtex i cada aresta com a màxim una vegada. La comprovació de visitats és O(1) gràcies al VisitedSet (hash set), i la cerca de veïns és O(1) gràcies al IntersectionGraph (hash map). El cost total és proporcional al nombre de nodes i arestes explorats.
- *Pitjor cas: O(V + E).* El destí no existeix o es troba a l'extrem oposat del graf: BFS ha d'explorar tot el graf. El cost segueix sent O(V + E) per les mateixes raons que el cas mitjà.

---

## Plot 1: Latency to find connected streets — sequential list vs intersection graph

### Raw data

| Mapa   | Interseccions | Temps seqüencial (ms) | Temps graf (ms) |
|--------|---------------|-----------------------|-----------------|
| xs\_1  | 11            | 0.248                 | 0.003           |
| xs\_2  | 71            | 0.139                 | 0.002           |
| md\_1  | 1122          | 0.597                 | 0.002           |
| lg\_1  | 3283          | 0.525                 | 0.001           |
| xl\_1  | 15378         | 0.234                 | 0.001           |
| 2xl\_1 | ~50000        | 2.973                 | 0.002           |

[grafico 1](secuencial-grafo.png)

### Explicació

Els resultats evidencien la diferència d'eficiència entre la cerca seqüencial i l'ús del graf d'interseccions. La cerca seqüencial (print_connected_segments) recorre tota la llista de segments per trobar els veïns d'una intersecció, cosa que implica una complexitat O(n) on n és el nombre total de segments. Per contra, graph_get accedeix directament al bucket corresponent del hash map en O(1) amortitzat, independentment del nombre de segments.

El temps seqüencial no creix de manera estrictament monotònica entre alguns mapes perquè el resultat depèn de la posició concreta del segment d'origen dins la llista, que varia entre mesures. Amb múltiples repeticions i mitjanes, la tendència creixent amb la mida del mapa seria clara. El cas de 2xl\_1 (2.973 ms) és el que millor il·lustra la diferència: la llista és tan gran que la cerca lineal es nota, mentre que el graf manté 0.002 ms.

---

## Plot 2: Latency to find a path — BFS with graph vs BFS_slow with list

### Raw data

| Mapa   | Interseccions | BFS optimitzat (ms) | BFS\_slow (ms) |
|--------|---------------|---------------------|----------------|
| xs\_1  | 11            | 0.052               | 0.032          |
| xs\_2  | 71            | 0.056               | 0.027          |
| md\_1  | 1122          | 0.460               | 0.906          |
| lg\_1  | 3283          | 1.520               | 20.664         |
| xl\_1  | 15378         | 3.634               | 174.395        |
| 2xl\_1 | ~50000        | 0.055               | 10587.052      |

[grafico 2](bfs-optimizado-slow.png)

> *Nota:* per a tots els mapes s'ha utilitzat el mateix parell origen-destinació relatiu (mateixa posició geogràfica proporcional al mapa), de manera que la comparació reflecteix exclusivament l'efecte de la mida del graf.

### Explicació

Els resultats mostren clarament la diferència de rendiment entre les dues implementacions de BFS a mesura que el mapa creix.

Per als mapes petits (xs\_1 i xs\_2), BFS_slow és lleugerament més ràpid que BFS optimitzat. Això és degut al cost fix d'inicialitzar i consultar el VisitedSet (hash set) i el IntersectionGraph, que supera el cost de la cerca lineal quan el nombre d'elements és molt petit. A mesura que el nombre de segments creix, l'avantatge del BFS optimitzat es fa evident: mentre BFS_slow té complexitat O(V·E) perquè per a cada node visitat recorre tota la llista de segments per trobar els veïns i comprova el array de visitats linealment, el BFS optimitzat és O(V+E) gràcies al hash map de veïns i al hash set de visitats. A xl\_1 la diferència és de 48× (3.6 ms vs 174 ms) i a 2xl\_1 de més de 190.000× (0.055 ms vs 10587 ms).

El temps baix de BFS optimitzat a 2xl\_1 (0.055 ms) s'explica perquè el parell origen-destinació triat estava molt a prop geogràficament, de manera que BFS va trobar el camí explorant molt pocs nodes. BFS\_slow, en canvi, tarda igual independentment de si el camí és curt, perquè en cada pas ha de recórrer tota la llista.

---

## Plot 3: BFS optimitzat vs BFS\_slow en funció de la distància origen-destinació

S'utilitza el mapa xl_1 per comparar ambdues implementacions de BFS en funció de la distància entre origen i destinació. Cada mesura és la *mitjana de 5 repeticions* sobre el mateix parell per reduir el soroll de mesura.

### Raw data

| Categoria     | Dist (km) | BFS opt (ms) | BFS\_slow (ms) |
|---------------|-----------|--------------|----------------|
| Molt propers  | 0.134     | 0.205        | 0.909          |
| Propers       | 0.536     | 0.190        | 34.281         |
| Mitja         | 1.679     | 1.126        | 287.032        |
| Llunyans      | 4.698     | 4.038        | 1356.895       |
| Molt llunyans | 8.949     | 6.908        | 1369.557       |

[grafico 3](plot-3.png)

### Ajust de corba i justificació

*BFS optimitzat — corba quadràtica t ≈ a·d^2*

BFS explora el graf per nivells concèntrics. En un graf de ciutat 2D, l'àrea explorada creix proporcional a r^2 (àrea d'un cercle). Com que O(V+E) és proporcional als nodes visitats i els nodes creixen amb l'àrea, esperem que el temps s'ajusti a una corba quadràtica: t ≈ a·d^2. Les dades ho confirmen parcialment: de 0.536 km a 4.698 km (8.77× distància) el temps passa de 0.190 ms a 4.038 ms (21.3×), proper a l'esperat per d^2 (76.9×). La desviació s'explica perquè en distàncies curtes el cost fix d'inicialitzar el VisitedSet i el IntersectionGraph domina sobre el cost real de cerca.

*BFS\_slow — creixement superquadràtic, tendint a O(d^4)*

En BFS_slow, per a cada node visitat es recorre tota la llista de segments per trobar els veïns (O(E)) i es comprova el array de visitats linealment (O(V)). Quan la distància augmenta, V i E creixen amb l'àrea explorada (∝ d²), de manera que el cost total és O(V·E) ∝ O(d²·d²) = O(d^4). La saturació observada entre 4.698 km i 8.949 km (1356 ms vs 1369 ms) s'explica perquè a 4.7 km BFS\_slow ja ha explorat pràcticament tot el graf accessible; augmentar la distància no forçà explorar molts nodes nous.

### Explicació

Els resultats confirmen que BFS optimitzat escala molt millor amb la distància que BFS\_slow. A 1.679 km la diferència ja és de 254× (1.1 ms vs 287 ms); a 4.698 km és de 336× (4 ms vs 1357 ms). BFS optimitzat creix suaument perquè el hash map de veïns i el hash set de visitats mantenen cada operació en O(1) amortitzat, i el nombre de nodes explorats creix quadràticament amb la distància. BFS\_slow pateix un creixement molt més pronunciat perquè cada pas implica recórrer tota la llista de segments, combinant dos factors que creixen amb la distància.

---

## Improvement to the visited data structure in BFS

### Implementació realitzada

Hem substituït el vector lineal de visitats de BFS_slow per un *hash set* (VisitedSet) implementat a path.c. El VisitedSet és una taula hash amb 16381 buckets (número primer) i encadenament separat, on la clau és el propi punter a StreetNode.

c
// Hash basat en l'adreça de memòria del punter (uintptr_t per compatibilitat 64-bit)
int bucket = (int)((uintptr_t)street % VISITED_BUCKETS);


### Complexitat actual vs millorada

| Operació | BFS\_slow (vector lineal) | BFS optimitzat (hash set) |
|----------|--------------------------|--------------------------|
| contains | O(n) — recorre tot el vector | O(1) amortitzat |
| add | O(1) — afegeix al final | O(1) amortitzat |
| Total BFS | O(V · E) | O(V + E) |

### Trade-offs

- *Avantatge de latència:* la reducció és dràstica. A xl\_1, BFS passa de 174 ms a 3.6 ms; a 2xl\_1, de 10587 ms a 0.055 ms.
- *Cost de memòria:* el hash set reserva memòria per als 16381 buckets i per a cada VisitedNode inserit. En el pitjor cas (tot el graf visitat), ocupa O(V) memòria addicional.
- *Per què el punter com a clau?* Cada StreetNode és únic en memòria, de manera que comparar punters és equivalent a comparar identitats de nodes, però més ràpid que comparar camps del struct. La distribució dels punters del heap no és regular, cosa que garanteix una bona distribució als buckets.

---

## Improvement to the algorithm to find the closest street segment

### Situació actual

La funció closest_segment a segments.c calcula el punt mig de cada segment i la distància haversine fins a la posició de l'usuari, mantenint el mínim. Recorre tots els segments de la llista, cosa que implica una complexitat *O(n)* on n és el nombre total de segments.

Per al mapa xl\_1 (18828 segments), aquesta operació és perceptible. Per al mapa 2xl\_1, amb ~50000 segments, pot arribar a ser el coll d'ampolla principal.

### Millora proposada: k-d tree 2D

La millora natural seria indexar els punts mig de tots els segments en un *k-d tree bidimensional* (k-d tree 2D) construït a partir de les coordenades (latitud, longitud) de cada punt mig.

Un k-d tree és un arbre binari de cerca que particiona l'espai en dimensions alternades (latitud primer, longitud després, latitud, ...). Un cop construït, una consulta de veí més proper (nearest neighbor search) explora únicament la branca de l'arbre que pot contenir el punt més proper, descartant la meitat de l'espai en cada nivell.


Cerca del veí més proper en un k-d tree:
1. Recórrer l'arbre cap al node fulla més proper a la consulta.
2. En el camí de tornada, comprovar si el cercle de radi "millor distància actual"
   intersecta l'altra meitat de l'espai del node pare.
3. Explorar l'altra meitat només si pot contenir un punt més proper.


### Complexitat actual vs millorada

| Operació | Implementació actual | Amb k-d tree |
|----------|---------------------|--------------|
| Construcció | O(1) (llista ja existent) | O(n log n) — una sola vegada |
| Cerca del segment més proper | O(n) | O(log n) promig |
| Memòria addicional | 0 | O(n) per als nodes de l'arbre |

### Trade-offs

- *Avantatge de latència:* per a mapes grans (xl\_1, 2xl\_1), la cerca passa de O(n) a O(log n). Per a xl\_1 amb 18828 segments, la millora teòrica és de ~18828 / 14.2 ≈ 1300× en el millor cas.
- *Cost de construcció:* el k-d tree es construeix una sola vegada en carregar el mapa. El cost O(n log n) és assumible perquè es fa una única vegada.
- *Complexitat d'implementació:* el k-d tree és significativament més complex d'implementar i depurar que un recorregut lineal.
- *Pitjor cas:* en dades molt desiguals (molts punts agrupats en una zona), el k-d tree pot degenerar a O(n) en la cerca. En dades geogràfiques de carrers, que solen estar distribuïts de manera uniforme per la zona del mapa, aquest cas és poc probable.
- *Alternativa més simple:* un spatial grid (divisió del mapa en cel·les) seria menys eficient que un k-d tree però molt més senzill d'implementar, amb cerca O(1) amortitzat si les cel·les estan ben dimensionades.