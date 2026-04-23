#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <stdarg.h>
#include <time.h>

#define MAX_FILES 100
#define MAX_PROBLEMES 20
#define MAX_BUFFER 1024
#define INF (INT_MAX / 4) // Valeur infinie

// --- Variables globales ---
extern FILE *g_fichier_trace;
extern bool g_silent_mode;

// --- Structures de Données ---
typedef struct {
    int n;
    int m;
    int **couts;
    int *provisions;
    int *commandes;
    int numero;
} Probleme;

typedef struct {
    int **transport;
    bool **base;
    double *u;
    double *v;
    double **marginaux;
    int cout_total;
} Solution;

typedef struct { int r, c; } Point;

// --- Utils (utils.c) ---
void journaliser(const char *format, ...);
void obtenir_prochain_nom_fichier_trace(char *buffer, size_t size);
int nombre_chiffres(int n);
int estTxt(const char *nom);

// --- Mémoire (solver.c) ---
Probleme* creer_probleme(int n, int m, int numero);
void liberer_probleme(Probleme *p);
Solution* creer_solution(int n, int m);
void liberer_solution(Solution *s, int n);
Solution* cloner_solution(Probleme *p, Solution *src);

// --- Lecture / IO (io.c) ---
Probleme** charger_problemes(const char *filename, int *count);
char* choixTxt();
int calculer_largeur_colonne(Probleme *p, Solution *s, int type_affichage);
void afficher_separateur(int n_cols, int largeur);
void afficher_matrice_couts(Probleme *p);
void afficher_solution(Probleme *p, Solution *s);
void afficher_couts_marginaux(Probleme *p, Solution *s);
void afficher_couts_potentiels(Probleme *p, Solution *s);

// --- Solver (solver.c) ---
void calculer_cout_total(Probleme *p, Solution *s);
Solution* nord_ouest(Probleme *p);
Solution* balas_hammer(Probleme *p);
bool test_degenerescence(Probleme *p, Solution *s);
void corriger_base_degenerescente(Probleme *p, Solution *s);
bool test_connexite_bfs(Probleme *p, Solution *s);
void calculer_potentiels(Probleme *p, Solution *s);
void calculer_marginaux(Probleme *p, Solution *s);
bool trouver_cycle_rec(Probleme *p, Solution *s, int curr_r, int curr_c, int start_r, int start_c, Point *chemin, int *len_chemin, bool is_row_move);
void ameliorer_solution_cycle(Probleme *p, Solution *s, int enter_i, int enter_j);
void algo_marche_pied(Probleme *p, Solution *s);

#endif // TRANSPORT_H
