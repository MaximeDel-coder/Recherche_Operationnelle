import pandas as pd
import matplotlib.pyplot as plt
import sys
import os
import io

def generer_graphique(fichier_trace):
    try:
        # Lecture et filtrage du fichier
        # On ne garde que les lignes qui ressemblent à du CSV (commencent par N; ou des chiffres)
        csv_buffer = io.StringIO()
        with open(fichier_trace, 'r') as f:
            for line in f:
                stripped = line.strip()
                if stripped.startswith("N;") or (len(stripped) > 0 and stripped[0].isdigit() and ";" in stripped):
                    csv_buffer.write(line)
                elif stripped.startswith("Moyenne;"):
                     pass

        csv_buffer.seek(0)
        
        # Lecture du buffer avec pandas
        df = pd.read_csv(csv_buffer, sep=';')
            
        # Conversion des colonnes numériques
        cols_numeriques = ['N', 'Iteration', 'Theta_NO', 'Theta_BH', 'T_NO', 'T_BH', 'Total_NO', 'Total_BH']
        for col in cols_numeriques:
            if col in df.columns:
                df[col] = pd.to_numeric(df[col], errors='coerce')
            
        df = df.dropna()

        if df.empty:
            print("Aucune donnée valide trouvée pour le graphique.")
            return

        # Détermination de l'axe X
        unique_n = df['N'].unique()
        multiple_n = len(unique_n) > 1
        
        x_col = 'N' if multiple_n else 'Iteration'
        x_label = 'Taille n' if multiple_n else 'Itération'
        
        # Configuration de la figure (3 lignes x 2 colonnes)
        fig, axes = plt.subplots(3, 2, figsize=(16, 10))
        fig.suptitle('Étude de Complexité : Nuages de points', fontsize=16)
        
        # Définition des sous-graphiques
        # Format: (ax, y_col, title, color)
        plots_config = [
            (axes[0, 0], 'Theta_NO', r'$\theta_{NO}(n)$ : Init Nord-Ouest', 'blue'),
            (axes[0, 1], 'Theta_BH', r'$\theta_{BH}(n)$ : Init Balas-Hammer', 'blue'),
            (axes[1, 0], 'T_NO', r'$t_{NO}(n)$ : Optimisation après NO', 'blue'),
            (axes[1, 1], 'T_BH', r'$t_{BH}(n)$ : Optimisation après BH', 'blue'),
            (axes[2, 0], 'Total_NO', 'Total Nord-Ouest', 'red'),
            (axes[2, 1], 'Total_BH', 'Total Balas-Hammer', 'red')
        ]

        for ax, y_col, title, color in plots_config:
            if multiple_n:
                # Si plusieurs N, on utilise une échelle log pour X si l'écart est grand
                ax.set_xscale('log')
                # Scatter plot pour voir la distribution pour chaque N
                ax.scatter(df[x_col], df[y_col], s=10, c=color, alpha=0.5)
            else:
                # Sinon on trace simplement l'évolution par itération
                ax.scatter(df[x_col], df[y_col], s=10, c=color, alpha=0.5)
            
            ax.set_title(title)
            ax.set_ylabel('Temps (s)')
            ax.grid(True, which="both", ls="--", alpha=0.6)
            
            # Seulement afficher le label X sur la dernière ligne
            if ax in axes[2, :]:
                ax.set_xlabel(x_label)

        plt.tight_layout(rect=[0, 0.03, 1, 0.95]) # Ajustement pour le titre principal
        
        nom_image = fichier_trace.replace('.txt', '.png')
        # Si le fichier d'entrée a un chemin, on garde le même dossier pour la sortie
        if os.path.dirname(fichier_trace):
             nom_image = os.path.join(os.path.dirname(fichier_trace), os.path.basename(nom_image))

        plt.savefig(nom_image, dpi=100)
        print(f"Graphique généré : {nom_image}")
        
    except Exception as e:
        print(f"Erreur lors de la génération du graphique : {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 plot_trace.py <fichier_trace.txt>")
    else:
        generer_graphique(sys.argv[1])
