#include "transport.h"

typedef struct {
    double theta_no;
    double theta_bh;
    double t_no;
    double t_bh;
    double total_no;
    double total_bh;
} Resultat;

void print_bar(int current, int total, int *last_percent, const char* label, const char* suffix) {
    int p = (int)((long long)current * 100 / total); 
    if (p > *last_percent || p == 100) { 
        *last_percent = p;
        int barWidth = 40;
        int pos = (barWidth * p) / 100;

        printf("\r\033[K%s [", label);
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) printf("=");
            else if (i == pos) printf(">");
            else printf(" ");
        }
        printf("] %d%%", p);
        if (suffix) {
            printf("%s", suffix);
        }
        fflush(stdout);
    }
}

Probleme* generer_probleme_aleatoire(int n) {
    Probleme *p = creer_probleme(n, n, 0);
    
    for(int i=0; i<n; i++) p->provisions[i] = 0;
    for(int j=0; j<n; j++) p->commandes[j] = 0;
    
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            int qte = (rand() % 100) + 1;
            p->provisions[i] += qte;
            p->commandes[j] += qte;
            p->couts[i][j] = (rand() % 100) + 1; 
        }
    }
    
    return p;
}

int main() {
    srand(time(NULL));

    char nom_fichier[64];
    obtenir_prochain_nom_fichier_trace(nom_fichier, sizeof(nom_fichier));
    g_fichier_trace = fopen(nom_fichier, "w");
    
    int sizes[] = {10, 40, 100, 400, 1000, 4000, 10000}; 
    int num_sizes = 7; 

    printf("Choix de la taille du probleme (n):\n");
    for(int i=0; i<num_sizes; i++) {
        printf("%d. n = %d\n", i+1, sizes[i]);
    }
    printf("Votre choix (1-%d) : ", num_sizes);
    
    int choix;
    if (scanf("%d", &choix) != 1 || choix < 1 || choix > num_sizes) {
        printf("Choix invalide.\n");
        return 1;
    }
    
    int n = sizes[choix - 1];
    int iterations = 100;

    bool show_global = (n >= 100);
    bool show_unit = (n > 400); 
    
    Resultat *results = malloc(iterations * sizeof(Resultat));

    printf("\nLancement de %d tests pour n=%d...\n", iterations, n);
    printf("\033[?25l"); 

    int last_global_percent = -1;

    g_silent_mode = true; // Activer le mode silencieux pour le benchmark

    for(int iter=0; iter<iterations; iter++) {
        if(show_global) {
             char suffix[32] = "";
             if (show_unit) {
                 snprintf(suffix, sizeof(suffix), " Test %d/%d", iter + 1, iterations);
             }
             print_bar(iter, iterations, &last_global_percent, "Globale", show_unit ? suffix : NULL);
        }
        
        if(show_unit) {
            if(show_global) printf("\n"); 
        }
        
        Probleme *p = generer_probleme_aleatoire(n);
        
        // --- Mesure Nord-Ouest ---
        clock_t t0 = clock();
        Solution *s_no = nord_ouest(p);
        clock_t t1 = clock();
        results[iter].theta_no = (double)(t1 - t0) / CLOCKS_PER_SEC;
        
        // --- Mesure Marche-Pied sur Nord-Ouest ---
        clock_t t2 = clock();
        algo_marche_pied(p, s_no);
        clock_t t3 = clock();
        results[iter].t_no = (double)(t3 - t2) / CLOCKS_PER_SEC;
        
        // --- Mesure Balas-Hammer ---
        clock_t t4 = clock();
        Solution *s_bh = balas_hammer(p);
        clock_t t5 = clock();
        results[iter].theta_bh = (double)(t5 - t4) / CLOCKS_PER_SEC;
        
        // --- Mesure Marche-Pied sur Balas-Hammer ---
        clock_t t6 = clock();
        algo_marche_pied(p, s_bh);
        clock_t t7 = clock();
        results[iter].t_bh = (double)(t7 - t6) / CLOCKS_PER_SEC;
        
        results[iter].total_no = results[iter].theta_no + results[iter].t_no;
        results[iter].total_bh = results[iter].theta_bh + results[iter].t_bh;
        
        if (show_unit && show_global) {
             printf("\033[1A"); 
        }

        liberer_solution(s_no, n);
        liberer_solution(s_bh, n);
        liberer_probleme(p);
    }
    
    if(show_global) {
        print_bar(iterations, iterations, &last_global_percent, "Globale", NULL);
        printf("\n");
    }
    
    printf("\033[?25h");
    printf("Terminé.\n");
    printf("Enregistrement de la trace dans : %s\n", nom_fichier);

    g_silent_mode = false; // Désactiver pour écrire dans le log à la fin
    if (g_fichier_trace) {
        fprintf(g_fichier_trace, "N;Iteration;Theta_NO;Theta_BH;T_NO;T_BH;Total_NO;Total_BH\n");
    }
    
    Resultat sum = {0};
    
    for(int i=0; i<iterations; i++) {
        if (g_fichier_trace) {
            fprintf(g_fichier_trace, "%d;%d;%.6f;%.6f;%.6f;%.6f;%.6f;%.6f\n", 
                   n, i+1, results[i].theta_no, results[i].theta_bh, results[i].t_no, results[i].t_bh, results[i].total_no, results[i].total_bh);
        }
               
        sum.theta_no += results[i].theta_no;
        sum.theta_bh += results[i].theta_bh;
        sum.t_no += results[i].t_no;
        sum.t_bh += results[i].t_bh;
        sum.total_no += results[i].total_no;
        sum.total_bh += results[i].total_bh;
    }
    
    if (g_fichier_trace) {
        fprintf(g_fichier_trace, "Moyenne;%d;%.6f;%.6f;%.6f;%.6f;%.6f;%.6f\n", 
               iterations, 
               sum.theta_no/iterations, sum.theta_bh/iterations, 
               sum.t_no/iterations, sum.t_bh/iterations, 
               sum.total_no/iterations, sum.total_bh/iterations);
        fclose(g_fichier_trace);
        
        char commande[256];
        snprintf(commande, sizeof(commande), "python3 plot_trace.py %s", nom_fichier);
        int ret = system(commande);
        if(ret != 0) printf("Erreur lors de la génération du graphique (code %d).\n", ret);
    }

    free(results);
    return 0;
}
