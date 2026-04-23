# Projet Transports - Résolution du Problème de Transport

Ce projet propose une implémentation complète et optimisée en C d'algorithmes permettant de résoudre le **problème de transport**, un problème classique d'optimisation mathématique et de recherche opérationnelle.

## Le Problème Mathématique Concret (Le README.md)

Le problème de transport consiste à déterminer le plan de transport optimal d'une marchandise depuis plusieurs sources (usines, entrepôts) vers plusieurs destinations (magasins, clients), de manière à **minimiser le coût total de transport**.

Mathématiquement, il s'agit d'un problème de programmation linéaire. Nous avons :
- **n sources** (provisions $P_i$)
- **m destinations** (commandes $C_j$)
- Une **matrice de coûts** $C_{i,j}$ représentant le coût unitaire de transport de la source $i$ vers la destination $j$.

Le but est de trouver la matrice des quantités transportées $X_{i,j}$ minimisant $\sum_{i,j} X_{i,j} \cdot C_{i,j}$ sous contraintes de respect des provisions et des commandes (problème équilibré où $\sum P_i = \sum C_j$).

L'algorithme implémenté utilise :
1. **Génération d'une solution de base initiale** : Algorithme du Nord-Ouest ou Algorithme de Balas-Hammer (Pénalités).
2. **Optimisation itérative** : Algorithme du Marche-Pied (Stepping-Stone).

## Benchmarks et Complexité

Afin de valider l'efficacité de nos algorithmes, un programme de *benchmark* a été développé (`src/benchmark.c`). Il génère des problèmes de transport de taille croissante ($N \times N$) et mesure le temps d'exécution.

L'analyse de ces mesures a permis de mettre en évidence les complexités temporelles suivantes :
- **Algorithme du Nord-Ouest** : Très rapide, de l'ordre de $O(n + m)$. Idéal pour une première solution basique, mais généralement très éloignée de l'optimum.
- **Algorithme de Balas-Hammer** : Plus coûteux (approchant $O(n^2 \log n)$ à $O(n^3)$ selon les implémentations pour le calcul des pénalités à chaque étape), mais offre une solution de base souvent très proche de l'optimale.
- **Algorithme du Marche-Pied** : Bien que la pire complexité d'un pivot de simplexe puisse être exponentielle, en pratique sur nos benchmarks, le nombre d'itérations nécessaires après un Balas-Hammer est drastiquement inférieur à celui après un Nord-Ouest.

Des graphiques Python (`plot_trace.py`) peuvent être générés à l'issue du benchmark pour illustrer cette évolution $O(n \log n)$ vs $O(n^2)$.

## Tests Unitaires

La robustesse de l'implémentation est assurée par une suite de **tests unitaires** automatisés (`tests/test_solver.c`).
Ces tests valident le comportement des fonctions fondamentales sur des graphes déterministes et de taille restreinte :
- Vérification du respect des contraintes par le *Nord-Ouest*.
- Calcul correct des pénalités et de la solution par *Balas-Hammer*.
- Recherche de cycles (Graphes) et détections de dégénérescences lors du *Marche-Pied*.

Pour exécuter les tests :
```bash
make tests
./test_solver
```

## Outils de Profiling

Pour garantir un code de qualité professionnelle (faible empreinte mémoire, exécution rapide), des outils de profiling ont été intégrés au cycle de développement :
- **Valgrind** : Utilisé de manière intensive (avec `--leak-check=full`) pour vérifier la gestion stricte de la mémoire dynamique (malloc/free). L'absence de fuites mémoire ou d'accès illégaux a été confirmée.
- **Gprof** : Utilisé sur l'exécutable `benchmark` (compilé avec le flag `-pg`) pour identifier les goulots d'étranglement (hotspots). Le profiling a permis d'optimiser la méthode `trouver_cycle_rec` (recherche récursive) pour fluidifier le pivot de la méthode du Marche-Pied.

## Architecture du Projet

Le code a été modulé selon les standards C :
- `include/transport.h` : Structures et prototypes
- `src/io.c` : Entrées/sorties (lecture fichiers .txt et affichages)
- `src/solver.c` : Coeur algorithmique (Nord-Ouest, Balas-Hammer, Stepping Stone)
- `src/main.c` : Interface utilisateur interactive
- `src/benchmark.c` : Script d'évaluation des performances
- `tests/test_solver.c` : Tests unitaires
- `original_state_backup/` : Sauvegarde du code source monolithique initial.

### Compilation

```bash
make clean
make all
```

Trois exécutables sont générés :
- `./transports` : Le programme interactif.
- `./benchmark` : L'outil d'évaluation (muet) des performances.
- `./test_solver` : L'outil de validation (tests assert).
