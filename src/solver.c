#include "transport.h"

Probleme* creer_probleme(int n, int m, int numero) {
    Probleme *p = malloc(sizeof(Probleme));
    p->n = n;
    p->m = m;
    p->numero = numero;
    p->couts = malloc(n * sizeof(int*));
    for(int i=0; i<n; i++) p->couts[i] = malloc(m * sizeof(int));
    p->provisions = malloc(n * sizeof(int));
    p->commandes = malloc(m * sizeof(int));
    return p;
}

void liberer_probleme(Probleme *p) {
    if(!p) return;
    for(int i=0; i<p->n; i++) free(p->couts[i]);
    free(p->couts);
    free(p->provisions);
    free(p->commandes);
    free(p);
}

Solution* creer_solution(int n, int m) {
    Solution *s = malloc(sizeof(Solution));
    s->transport = malloc(n * sizeof(int*));
    s->base = malloc(n * sizeof(bool*));
    s->marginaux = malloc(n * sizeof(double*));
    for(int i=0; i<n; i++) {
        s->transport[i] = calloc(m, sizeof(int));
        s->base[i] = calloc(m, sizeof(bool));
        s->marginaux[i] = calloc(m, sizeof(double));
    }
    s->u = calloc(n, sizeof(double));
    s->v = calloc(m, sizeof(double));
    s->cout_total = 0;
    return s;
}

void liberer_solution(Solution *s, int n) {
    if(!s) return;
    for(int i=0; i<n; i++) {
        free(s->transport[i]);
        free(s->base[i]);
        free(s->marginaux[i]);
    }
    free(s->transport);
    free(s->base);
    free(s->marginaux);
    free(s->u);
    free(s->v);
    free(s);
}

Solution* cloner_solution(Probleme *p, Solution *src) {
    Solution *s = creer_solution(p->n, p->m);
    s->cout_total = src->cout_total;
    for(int i=0; i<p->n; i++) {
        s->u[i] = src->u[i];
        for(int j=0; j<p->m; j++) {
            s->transport[i][j] = src->transport[i][j];
            s->base[i][j] = src->base[i][j];
            s->marginaux[i][j] = src->marginaux[i][j];
        }
    }
    for(int j=0; j<p->m; j++) s->v[j] = src->v[j];
    return s;
}

void calculer_cout_total(Probleme *p, Solution *s) {
    s->cout_total = 0;
    for(int i=0; i<p->n; i++) {
        for(int j=0; j<p->m; j++) {
            s->cout_total += s->transport[i][j] * p->couts[i][j];
        }
    }
}

Solution* nord_ouest(Probleme *p) {
    journaliser("--- Execution Nord-Ouest ---\n");
    Solution *s = creer_solution(p->n, p->m);
    
    int *reste_p = malloc(p->n * sizeof(int));
    int *reste_c = malloc(p->m * sizeof(int));
    memcpy(reste_p, p->provisions, p->n * sizeof(int));
    memcpy(reste_c, p->commandes, p->m * sizeof(int));
    
    int i = 0, j = 0;
    while(i < p->n && j < p->m) {
        int qte = (reste_p[i] < reste_c[j]) ? reste_p[i] : reste_c[j];
        s->transport[i][j] = qte;
        s->base[i][j] = true;
        
        reste_p[i] -= qte;
        reste_c[j] -= qte;

        if(reste_p[i] == 0 && reste_c[j] == 0) {
            if (i < p->n - 1) i++;
            else if (j < p->m - 1) j++;
            else break;
        }
        else if(reste_p[i] == 0) i++;
        else j++;
    }
    
    free(reste_p);
    free(reste_c);
    calculer_cout_total(p, s);
    return s;
}

Solution* balas_hammer(Probleme *p) {
    journaliser("--- Execution Balas-Hammer ---\n");
    Solution *s = creer_solution(p->n, p->m);
    
    int *reste_p = malloc(p->n * sizeof(int));
    int *reste_c = malloc(p->m * sizeof(int));
    bool *ligne_bannie = calloc(p->n, sizeof(bool));
    bool *col_bannie = calloc(p->m, sizeof(bool));
    
    memcpy(reste_p, p->provisions, p->n * sizeof(int));
    memcpy(reste_c, p->commandes, p->m * sizeof(int));
    
    int nb_remplis = 0;
    int cible = p->n + p->m - 1; 

    while(nb_remplis < cible) {
        int max_penalite = -1;
        int type_max = -1;
        int index_max = -1;
        
        for(int i=0; i<p->n; i++) {
            if(reste_p[i] <= 0 || ligne_bannie[i]) continue;
            int min1 = INF, min2 = INF;
            for(int j=0; j<p->m; j++) {
                if(reste_c[j] > 0 && !col_bannie[j]) {
                    if(p->couts[i][j] < min1) {
                        min2 = min1; min1 = p->couts[i][j];
                    } else if(p->couts[i][j] < min2) {
                        min2 = p->couts[i][j];
                    }
                }
            }
            int pen = (min2 == INF) ? min1 : (min2 - min1); 
            if(pen > max_penalite || (pen == max_penalite && reste_p[i] > (type_max==0 ? reste_p[index_max] : 0))) {
                max_penalite = pen; type_max = 0; index_max = i;
            }
        }
        
        for(int j=0; j<p->m; j++) {
            if(reste_c[j] <= 0 || col_bannie[j]) continue;
            int min1 = INF, min2 = INF;
            for(int i=0; i<p->n; i++) {
                if(reste_p[i] > 0 && !ligne_bannie[i]) {
                    if(p->couts[i][j] < min1) {
                        min2 = min1; min1 = p->couts[i][j];
                    } else if(p->couts[i][j] < min2) {
                        min2 = p->couts[i][j];
                    }
                }
            }
            int pen = (min2 == INF) ? min1 : (min2 - min1);
             if(pen > max_penalite) { 
                max_penalite = pen; type_max = 1; index_max = j;
            }
        }
        
        if (max_penalite == -1) break;
        
        int best_r = -1, best_c = -1;
        int min_cout = INF;
        
        if(type_max == 0) {
            int i = index_max;
            for(int j=0; j<p->m; j++) {
                if(reste_c[j] > 0 && !col_bannie[j]) {
                    if(p->couts[i][j] < min_cout) {
                        min_cout = p->couts[i][j]; best_r = i; best_c = j;
                    }
                }
            }
        } else {
            int j = index_max;
            for(int i=0; i<p->n; i++) {
                if(reste_p[i] > 0 && !ligne_bannie[i]) {
                    if(p->couts[i][j] < min_cout) {
                        min_cout = p->couts[i][j]; best_r = i; best_c = j;
                    }
                }
            }
        }
        
        int qte = (reste_p[best_r] < reste_c[best_c]) ? reste_p[best_r] : reste_c[best_c];
        s->transport[best_r][best_c] = qte;
        s->base[best_r][best_c] = true;
        
        journaliser("Balas-Hammer: Max Penalité %d sur %s %d -> Choix case (%d,%d) Cout %d Qté %d\n", 
               max_penalite, type_max==0?"Ligne":"Com", index_max+1, best_r+1, best_c+1, min_cout, qte);
        
        reste_p[best_r] -= qte;
        reste_c[best_c] -= qte;
        nb_remplis++;
        
        if(reste_p[best_r] == 0) ligne_bannie[best_r] = true;
        if(reste_c[best_c] == 0) col_bannie[best_c] = true;
    }
    
    for(int i=0; i<p->n; i++) {
        for(int j=0; j<p->m; j++) {
            if(reste_p[i]>0 && reste_c[j]>0) {
                 int qte = (reste_p[i] < reste_c[j]) ? reste_p[i] : reste_c[j];
                 if(qte > 0) {
                     s->transport[i][j] += qte;
                     s->base[i][j] = true;
                     reste_p[i] -= qte;
                     reste_c[j] -= qte;
                 }
            }
        }
    }

    free(reste_p);
    free(reste_c);
    free(ligne_bannie);
    free(col_bannie);
    calculer_cout_total(p, s);
    return s;
}

bool test_degenerescence(Probleme *p, Solution *s) {
    int nb_base = 0;
    int nb_zero = 0;

    for (int i = 0; i < p->n; i++) {
        for (int j = 0; j < p->m; j++) {
            if (s->base[i][j]) {
                nb_base++;
                if (s->transport[i][j] == 0) {
                    nb_zero++;
                }
            }
        }
    }

    int cible = p->n + p->m - 1;
    bool degenerer = (nb_base != cible) || (nb_zero > 0);

    journaliser("Test de dégénérescence :\n");
    journaliser("  - Variables de base : %d (cible n+m-1 = %d)\n", nb_base, cible);
    journaliser("  - Variables de base avec flux nul : %d\n", nb_zero);

    if (degenerer) {
        journaliser("  -> La proposition de transport est DEGENEREE.\n");
    } else {
        journaliser("  -> La proposition de transport n'est pas dégénérée.\n");
    }

    return degenerer;
}

void corriger_base_degenerescente(Probleme *p, Solution *s) {
    while(true) {
        bool **visite = malloc(p->n * sizeof(bool*));
        for(int i=0; i<p->n; i++) visite[i] = calloc(p->m, sizeof(bool));
        
        bool *visite_lignes = calloc(p->n, sizeof(bool));
        bool *visite_cols = calloc(p->m, sizeof(bool));
        int *queue = malloc((p->n + p->m) * sizeof(int));
        int head = 0, tail = 0;
        
        visite_lignes[0] = true;
        queue[tail++] = 0;
        
        int count_connected = 0;
        
        while(head < tail) {
            int u = queue[head++];
            count_connected++;
            
            if(u < p->n) {
                int r = u;
                for(int c=0; c<p->m; c++) {
                    if(s->base[r][c] && !visite_cols[c]) {
                        visite_cols[c] = true; queue[tail++] = p->n + c;
                    }
                }
            } else {
                int c = u - p->n;
                for(int r=0; r<p->n; r++) {
                    if(s->base[r][c] && !visite_lignes[r]) {
                        visite_lignes[r] = true; queue[tail++] = r;
                    }
                }
            }
        }
        
        if(count_connected == p->n + p->m) {
             free(queue); free(visite_lignes); free(visite_cols);
             for(int i=0; i<p->n; i++) free(visite[i]); free(visite);
             break;
        }
        
        int best_i = -1, best_j = -1;
        int min_val = INF;
        
        for(int i=0; i<p->n; i++) {
            for(int j=0; j<p->m; j++) {
                if(s->base[i][j]) continue;
                bool link_possible = (visite_lignes[i] && !visite_cols[j]) || (!visite_lignes[i] && visite_cols[j]);
                if(link_possible) {
                    if(p->couts[i][j] < min_val) {
                        min_val = p->couts[i][j]; best_i = i; best_j = j;
                    }
                }
            }
        }
        
        if(best_i != -1) {
            s->base[best_i][best_j] = true;
            s->transport[best_i][best_j] = 0;
            journaliser("Correction Connexité: Ajout base fictive (%d,%d)\n", best_i+1, best_j+1);
        } else {
            break; 
        }

        free(queue); free(visite_lignes); free(visite_cols);
        for(int i=0; i<p->n; i++) free(visite[i]); free(visite);
    }
}

bool test_connexite_bfs(Probleme *p, Solution *s) {
    int V = p->n + p->m;
    int *visite = calloc(V, sizeof(int));
    int *queue = malloc(V * sizeof(int));
    if (!visite || !queue) {
        journaliser("Erreur d'allocation dans test_connexite_bfs.\n");
        if(visite) free(visite);
        if(queue) free(queue);
        return true; 
    }

    int nb_composantes = 0;
    journaliser("Test de connexite (graphe biparti lignes/colonnes) :\n");

    for (int start = 0; start < V; start++) {
        if (visite[start]) continue;

        nb_composantes++;
        journaliser("  Sous-graphe connexe %d : ", nb_composantes);

        int head = 0, tail = 0;
        visite[start] = 1;
        queue[tail++] = start;

        while (head < tail) {
            int u = queue[head++];
            if (u < p->n) journaliser("P%d ", u + 1);
            else journaliser("C%d ", (u - p->n) + 1);

            if (u < p->n) {
                int i = u;
                for (int j = 0; j < p->m; j++) {
                    if (s->base[i][j]) {
                        int v = p->n + j;
                        if (!visite[v]) { visite[v] = 1; queue[tail++] = v; }
                    }
                }
            } else {
                int j = u - p->n;
                for (int i = 0; i < p->n; i++) {
                    if (s->base[i][j]) {
                        int v = i;
                        if (!visite[v]) { visite[v] = 1; queue[tail++] = v; }
                    }
                }
            }
        }
        journaliser("\n");
    }

    bool connexe = (nb_composantes == 1);
    if (connexe) journaliser("  -> La proposition de transport est CONNEXE.\n");
    else journaliser("  -> La proposition de transport n'est PAS connexe (%d composantes).\n", nb_composantes);

    free(visite);
    free(queue);
    return connexe;
}

void calculer_potentiels(Probleme *p, Solution *s) {
    for(int i=0; i<p->n; i++) s->u[i] = -INF;
    for(int j=0; j<p->m; j++) s->v[j] = -INF;
    s->u[0] = 0;
    
    bool changement = true;
    while(changement) {
        changement = false;
        for(int i=0; i<p->n; i++) {
            for(int j=0; j<p->m; j++) {
                if(s->base[i][j]) {
                    if(s->u[i] != -INF && s->v[j] == -INF) {
                        s->v[j] = p->couts[i][j] - s->u[i];
                        changement = true;
                    }
                    else if(s->v[j] != -INF && s->u[i] == -INF) {
                        s->u[i] = p->couts[i][j] - s->v[j];
                        changement = true;
                    }
                }
            }
        }
    }
}

void calculer_marginaux(Probleme *p, Solution *s) {
    for(int i=0; i<p->n; i++) {
        for(int j=0; j<p->m; j++) {
            if(!s->base[i][j]) {
                s->marginaux[i][j] = p->couts[i][j] - (s->u[i] + s->v[j]);
            } else {
                s->marginaux[i][j] = 0;
            }
        }
    }
}

bool trouver_cycle_rec(Probleme *p, Solution *s, int curr_r, int curr_c, int start_r, int start_c, 
                       Point *chemin, int *len_chemin, bool is_row_move) {
    chemin[*len_chemin].r = curr_r;
    chemin[*len_chemin].c = curr_c;
    (*len_chemin)++;
    
    if(curr_r == start_r && curr_c == start_c && *len_chemin > 1) {
        return true;
    }
    
    if(is_row_move) {
        for(int j=0; j<p->m; j++) {
            if(j != curr_c) {
                if((j == start_c && curr_r == start_r) || s->base[curr_r][j]) {
                    if(*len_chemin >= 2 && chemin[*len_chemin-2].r == curr_r && chemin[*len_chemin-2].c == j) continue;
                    if(trouver_cycle_rec(p, s, curr_r, j, start_r, start_c, chemin, len_chemin, !is_row_move)) return true;
                }
            }
        }
    } else {
        for(int i=0; i<p->n; i++) {
            if(i != curr_r) {
                 if((curr_c == start_c && i == start_r) || s->base[i][curr_c]) {
                     if(*len_chemin >= 2 && chemin[*len_chemin-2].r == i && chemin[*len_chemin-2].c == curr_c) continue;
                     if(trouver_cycle_rec(p, s, i, curr_c, start_r, start_c, chemin, len_chemin, !is_row_move)) return true;
                 }
            }
        }
    }
    
    (*len_chemin)--;
    return false;
}

void ameliorer_solution_cycle(Probleme *p, Solution *s, int enter_i, int enter_j) {
    Point *chemin = malloc(2 * (p->n + p->m) * sizeof(Point));
    if(!chemin) return;
    int len = 0;
    
    if(!trouver_cycle_rec(p, s, enter_i, enter_j, enter_i, enter_j, chemin, &len, true)) {
        len = 0;
        if(!trouver_cycle_rec(p, s, enter_i, enter_j, enter_i, enter_j, chemin, &len, false)) {
            journaliser("Erreur critique: Pas de cycle trouvé pour entrée (%d,%d)\n", enter_i+1, enter_j+1);
            free(chemin);
            return;
        }
    }
    
    journaliser("Cycle de modifications détecté: ");
    int min_val = INF;
    Point pdos = {-1, -1};
    
    for(int k=0; k<len; k++) {
        char signe = (k%2 == 0) ? '+' : '-';
        journaliser("(%d,%d)[%c] ", chemin[k].r+1, chemin[k].c+1, signe);
        
        if(k%2 != 0) {
            int val = s->transport[chemin[k].r][chemin[k].c];
            if(val < min_val) {
                min_val = val;
                pdos = chemin[k];
            }
        }
    }
    journaliser("\n");
    
    journaliser("Quantité à déplacer Theta = %d. Case sortante (%d,%d)\n", min_val, pdos.r+1, pdos.c+1);
    
    if(min_val == 0) {
        journaliser("Dégénérescence détectée : pivotement avec Theta=0 (changement de base sans changement de flux).\n");
    }

    for(int k=0; k<len-1; k++) {
        if(k%2 == 0) s->transport[chemin[k].r][chemin[k].c] += min_val;
        else s->transport[chemin[k].r][chemin[k].c] -= min_val;
    }
    
    s->base[enter_i][enter_j] = true;
    s->base[pdos.r][pdos.c] = false;
    free(chemin);
}

void algo_marche_pied(Probleme *p, Solution *s) {
    int iter = 0;
    while (true) {
        journaliser("\n--- Iteration %d ---\n", ++iter);

        bool degenerer = test_degenerescence(p, s);
        (void)degenerer;

        bool connexe = test_connexite_bfs(p, s);

        if (!connexe) {
            journaliser("  Correction du graphe non connexe...\n");
            corriger_base_degenerescente(p, s);
            test_connexite_bfs(p, s);
        }

        calculer_potentiels(p, s);
        calculer_marginaux(p, s);

        afficher_couts_potentiels(p, s);
        afficher_couts_marginaux(p, s);
        calculer_cout_total(p, s);
        afficher_solution(p, s);

        double min_marginal = 0.0;
        int best_i = -1, best_j = -1;

        for (int i = 0; i < p->n; i++) {
            for (int j = 0; j < p->m; j++) {
                if (!s->base[i][j] && s->marginaux[i][j] < min_marginal) {
                    min_marginal = s->marginaux[i][j];
                    best_i = i;
                    best_j = j;
                }
            }
        }

        if (min_marginal >= -0.0001) {
            journaliser("\n>>> SOLUTION OPTIMALE ATTEINTE. Cout total: %d <<<\n", s->cout_total);
            break;
        }

        journaliser("Amelioration possible: Entree case (%d,%d) Marginal %.1f\n",
               best_i + 1, best_j + 1, min_marginal);

        ameliorer_solution_cycle(p, s, best_i, best_j);
    }
}
