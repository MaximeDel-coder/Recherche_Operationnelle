#include "../include/transport.h"
#include <assert.h>

void test_nord_ouest() {
    Probleme *p = creer_probleme(3, 4, 1);
    
    int couts[3][4] = {
        {10, 2, 20, 11},
        {12, 7, 9, 20},
        {4, 14, 16, 18}
    };
    int provisions[3] = {15, 25, 10};
    int commandes[4] = {5, 15, 15, 15};
    
    for (int i=0; i<3; i++) {
        p->provisions[i] = provisions[i];
        for (int j=0; j<4; j++) {
            p->couts[i][j] = couts[i][j];
        }
    }
    for (int j=0; j<4; j++) p->commandes[j] = commandes[j];
    
    g_silent_mode = true;
    Solution *s = nord_ouest(p);
    
    // Nord Ouest transport:
    // P1: C1=5, C2=10
    // P2: C2=5, C3=15, C4=5
    // P3: C4=10
    assert(s->transport[0][0] == 5);
    assert(s->transport[0][1] == 10);
    assert(s->transport[1][1] == 5);
    assert(s->transport[1][2] == 15);
    assert(s->transport[1][3] == 5);
    assert(s->transport[2][3] == 10);
    
    liberer_solution(s, 3);
    liberer_probleme(p);
    printf("test_nord_ouest: PASS\n");
}

void test_balas_hammer() {
    Probleme *p = creer_probleme(3, 4, 1);
    
    int couts[3][4] = {
        {10, 2, 20, 11},
        {12, 7, 9, 20},
        {4, 14, 16, 18}
    };
    int provisions[3] = {15, 25, 10};
    int commandes[4] = {5, 15, 15, 15};
    
    for (int i=0; i<3; i++) {
        p->provisions[i] = provisions[i];
        for (int j=0; j<4; j++) {
            p->couts[i][j] = couts[i][j];
        }
    }
    for (int j=0; j<4; j++) p->commandes[j] = commandes[j];
    
    g_silent_mode = true;
    Solution *s = balas_hammer(p);
    
    assert(s->cout_total > 0);
    
    liberer_solution(s, 3);
    liberer_probleme(p);
    printf("test_balas_hammer: PASS\n");
}

void test_marche_pied() {
    Probleme *p = creer_probleme(3, 4, 1);
    
    int couts[3][4] = {
        {10, 2, 20, 11},
        {12, 7, 9, 20},
        {4, 14, 16, 18}
    };
    int provisions[3] = {15, 25, 10};
    int commandes[4] = {5, 15, 15, 15};
    
    for (int i=0; i<3; i++) {
        p->provisions[i] = provisions[i];
        for (int j=0; j<4; j++) {
            p->couts[i][j] = couts[i][j];
        }
    }
    for (int j=0; j<4; j++) p->commandes[j] = commandes[j];
    
    g_silent_mode = true;
    Solution *s = balas_hammer(p);
    algo_marche_pied(p, s);
    
    assert(s->cout_total > 0); // S'assurer que ça tourne sans erreur
    
    liberer_solution(s, 3);
    liberer_probleme(p);
    printf("test_marche_pied: PASS\n");
}

int main() {
    test_nord_ouest();
    test_balas_hammer();
    test_marche_pied();
    printf("All tests passed successfully.\n");
    return 0;
}
