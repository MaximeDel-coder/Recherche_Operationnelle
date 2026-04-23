#include "transport.h"

int main() {
    char nom_trace[64];
    obtenir_prochain_nom_fichier_trace(nom_trace, sizeof(nom_trace));
    g_fichier_trace = fopen(nom_trace, "w");
    if (g_fichier_trace) {
        printf("Enregistrement de la session dans : %s\n", nom_trace);
    }

    char *fichier_source = choixTxt();
    if(!fichier_source) {
        if(g_fichier_trace) fclose(g_fichier_trace);
        return 1;
    }

    int count = 0;
    Probleme **problemes = charger_problemes(fichier_source, &count);
    free(fichier_source); 
    
    if(!problemes) {
        if(g_fichier_trace) fclose(g_fichier_trace);
        return 1;
    }
    
    journaliser("Nombre de problemes charges: %d\n", count);
    if(count == 0) {
        journaliser("Erreur: Aucun probleme valide trouve dans ce fichier.\n");
        free(problemes);
        if(g_fichier_trace) fclose(g_fichier_trace);
        return 0;
    }
    
    char buffer[20];
    while(true) {
        journaliser("\n================ MENU ==================\n");
        for(int i=0; i<count; i++) {
            journaliser("\n--- Probleme %d ---\n", i+1);
            afficher_matrice_couts(problemes[i]);
        }
        journaliser("Q. Quitter\n");
        journaliser("Votre Choix (Numero du Probleme) > ");
        if (scanf("%s", buffer) != 1) break;
        journaliser("%s\n", buffer);
        
        if(buffer[0] == 'Q' || buffer[0] == 'q') break;
        
        int choix = atoi(buffer);
        if(choix < 1 || choix > count) {
            journaliser("Choix invalide.\n"); continue;
        }
        
        Probleme *p = problemes[choix-1];
        journaliser("\nVous avez choisi le Probleme %d :\n", choix);
        afficher_matrice_couts(p);
        
        journaliser("1. Nord-Ouest\n2. Balas-Hammer\nAlgo > ");
        int algo;
        if (scanf("%d", &algo) != 1) algo = 1;
        journaliser("%d\n", algo);
        
        Solution *init = NULL;
        if(algo == 1) init = nord_ouest(p);
        else init = balas_hammer(p);
        
        if(init) {
            journaliser("\n--- Solution Initiale Générée ---\n");
            afficher_solution(p, init);

            char choix_opt[10];
            journaliser("\nLa solution initiale est affichée ci-dessus (Cout: %d).\n", init->cout_total);
            journaliser("Voulez-vous lancer l'optimisation (Marche-Pied) ? (o/n) : ");
            if (scanf("%s", choix_opt) != 1) choix_opt[0] = 'n';
            journaliser("%s\n", choix_opt);

            if (choix_opt[0] == 'o' || choix_opt[0] == 'O') {
                journaliser("\n>>> Lancement Optimisation (Marche-Pied) <<<\n");
                algo_marche_pied(p, init);
            } else {
                journaliser("\nOptimisation annulée. Retour au menu.\n");
            }
            
            liberer_solution(init, p->n);
        }

        journaliser("\nVoulez-vous traiter un autre graphe ? (o/n) : ");
        char rep[10];
        if (scanf("%s", rep) != 1) break;
        journaliser("%s\n", rep);
        if (rep[0] == 'n' || rep[0] == 'N') break;
    }
    
    for(int i=0; i<count; i++) liberer_probleme(problemes[i]);
    free(problemes);
    
    if(g_fichier_trace) fclose(g_fichier_trace);
    
    return 0;
}
