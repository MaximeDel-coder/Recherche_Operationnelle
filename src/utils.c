#include "transport.h"

FILE *g_fichier_trace = NULL;
bool g_silent_mode = false;

void journaliser(const char *format, ...) {
    if (g_silent_mode) return;

    va_list args;

    // Affichage sur stdout
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    // Écriture dans le fichier de trace si ouvert
    if (g_fichier_trace != NULL) {
        va_start(args, format);
        vfprintf(g_fichier_trace, format, args);
        va_end(args);
    }
}

void obtenir_prochain_nom_fichier_trace(char *buffer, size_t size) {
    int i = 1;
    while (1) {
        snprintf(buffer, size, "trace_t_%d.txt", i);
        FILE *f = fopen(buffer, "r");
        if (f == NULL) {
            return; // Le fichier n'existe pas, on peut l'utiliser
        }
        fclose(f);
        i++;
    }
}

int nombre_chiffres(int n) {
    char buffer[32];
    return snprintf(buffer, sizeof(buffer), "%d", n);
}

int estTxt(const char *nom){
    size_t len = strlen(nom);
    if (len < 4) {
        return 0;
    }
    return (strcmp(nom + len - 4, ".txt") == 0);
}
