#include "transport.h"

Probleme** charger_problemes(const char *filename, int *count) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        journaliser("Erreur: Impossible d'ouvrir %s\n", filename);
        return NULL;
    }

    Probleme **problemes = malloc(MAX_PROBLEMES * sizeof(Probleme*));
    *count = 0;
    int n, m;

    while (fscanf(f, "%d %d", &n, &m) == 2 && *count < MAX_PROBLEMES) {
        Probleme *p = creer_probleme(n, m, *count + 1);
        for(int i=0; i<n; i++) {
            for(int j=0; j<m; j++) {
                fscanf(f, "%d", &p->couts[i][j]);
            }
            fscanf(f, "%d", &p->provisions[i]);
        }
        for(int j=0; j<m; j++) {
            fscanf(f, "%d", &p->commandes[j]);
        }

        problemes[(*count)++] = p;
    }
    fclose(f);
    return problemes;
}

char* choixTxt() {
    DIR *dir;
    struct dirent *entree;
    char *fichiers[MAX_FILES];
    int compteur = 0;

    dir = opendir(".");
    if (dir == NULL){
        journaliser("Erreur d'ouverture du répertoire courant.\n");
        return NULL;
    }

    while ((entree = readdir(dir)) != NULL){
        if (estTxt(entree->d_name)) {
            if (compteur < MAX_FILES) {
                char *copie = malloc(strlen(entree->d_name) + 1);
                strcpy(copie, entree->d_name);
                fichiers[compteur++] = copie;
            }
        }
    }
    closedir(dir);

    if (compteur == 0) {
        journaliser("Aucun fichier .txt trouvé dans le répertoire courant.\n");
        return NULL;
    }
    
    journaliser("Fichiers .txt trouvés :\n");
    for (int i = 0; i < compteur; i++) {
        journaliser("%d: %s\n", i + 1, fichiers[i]);
    }

    int choix;
    journaliser("Entrez le numéro du fichier à choisir : ");
    if (scanf("%d", &choix) != 1) choix = 0;
    journaliser("%d\n", choix);
    
    while (choix < 1 || choix > compteur) {
        journaliser("Choix invalide. Veuillez réessayer : ");
        if (scanf("%d", &choix) != 1) {
             int c;
             while ((c = getchar()) != '\n' && c != EOF);
        }
        journaliser("%d\n", choix);
    }

    char *fichierChoisi = fichiers[choix - 1];

    for (int i = 0; i < compteur; i++) {
        if (i != choix - 1) free(fichiers[i]);
    }

    return fichierChoisi;
}

int calculer_largeur_colonne(Probleme *p, Solution *s, int type) {
    int max_val = 0;
    for(int i=0; i<p->n; i++) {
        if (abs(p->provisions[i]) > max_val) max_val = abs(p->provisions[i]);
        for(int j=0; j<p->m; j++) {
            int val = 0;
            if (type == 0) val = p->couts[i][j];
            else if (type == 1 && s) val = s->transport[i][j];
            else if (type == 2 && s) val = (int)s->marginaux[i][j];
            else if (type == 3 && s) val = (int)(s->u[i] + s->v[j]);
            if (abs(val) > max_val) max_val = abs(val);
        }
    }
    for(int j=0; j<p->m; j++) {
        if (abs(p->commandes[j]) > max_val) max_val = abs(p->commandes[j]);
    }
    
    int largeur = nombre_chiffres(max_val);
    if (largeur < 3) largeur = 3;
    if (type == 2 || type == 3) largeur += 2;
    return largeur + 2;
}

void afficher_separateur(int n_cols, int largeur) {
    journaliser("      +");
    for(int j=0; j<n_cols; j++) {
        for(int k=0; k<largeur; k++) journaliser("-");
        journaliser("+");
    }
    journaliser("------------+\n");
}

static void afficher_matrice_generique(Probleme *p, Solution *s, int type) {
    if (g_silent_mode) return;
    int l = calculer_largeur_colonne(p, s, type);

    journaliser("      |");
    for(int j=0; j<p->m; j++) {
        journaliser("%*sC%d |", l - (j<9?3:4), "", j+1); 
    }
    if (type == 0 || type == 1) journaliser(" Provisions |\n");
    else journaliser("\n");
    
    afficher_separateur(p->m, l);

    for(int i=0; i<p->n; i++) {
        journaliser(" P%2d  |", i+1);
        for(int j=0; j<p->m; j++) {
            if (type == 0) journaliser("%*d |", l-1, p->couts[i][j]);
            else if (type == 1) {
                if (s->base[i][j]) journaliser("%*d |", l-1, s->transport[i][j]);
                else journaliser("%*d |", l-1, s->transport[i][j]);
            }
            else if (type == 2) {
                 if (s->base[i][j]) journaliser("%*s |", l-1, ".");
                 else journaliser("%*.1f |", l-1, s->marginaux[i][j]);
            }
            else if (type == 3) journaliser("%*.1f |", l-1, s->u[i] + s->v[j]);
        }
        
        if (type == 0 || type == 1) journaliser(" %10d |", p->provisions[i]);
        else if (type == 2 || type == 3) journaliser(" u=%-7.1f|", s->u[i]);
        journaliser("\n");
        afficher_separateur(p->m, l);
    }
    
    if (type == 0 || type == 1) {
        journaliser(" Comm.|");
        for(int j=0; j<p->m; j++) {
            journaliser("%*d |", l-1, p->commandes[j]);
        }
        journaliser("\n");
    } else {
        journaliser("      |");
        for(int j=0; j<p->m; j++) {
            journaliser(" v=%-*.1f|", l-4, s->v[j]);
        }
        journaliser("\n");
    }
    journaliser("\n");
}

void afficher_matrice_couts(Probleme *p) {
    journaliser("=== Matrice des Couts ===\n");
    afficher_matrice_generique(p, NULL, 0);
}

void afficher_solution(Probleme *p, Solution *s) {
    journaliser("=== Proposition de Transport (Cout Total: %d) ===\n", s->cout_total);
    afficher_matrice_generique(p, s, 1);
}

void afficher_couts_marginaux(Probleme *p, Solution *s) {
    journaliser("=== Table des Couts Marginaux (Reduits) ===\n");
    afficher_matrice_generique(p, s, 2);
}

void afficher_couts_potentiels(Probleme *p, Solution *s) {
    journaliser("=== Table des Couts Potentiels ===\n");
    afficher_matrice_generique(p, s, 3);
}
