# Last edited 03/12/2025
# Author: Anya Elvin
# Macro to plot the efficiency values for different variations of the same cut
# Left flexible so that it can be used to analyse all different cuts
    # Cut 1: 1 electron only and 1 photon only and nothing else
    # Cut 2: E_e > 50keV and E_gam > 50keV
    # Cut 3: E_e + E_gam ≤ 3.27keV
    # Cut 4: gam OM number ≤ 512
    # Cut 5: Electron start vertex has to be ≥45mm away from edges 
    # Cut 6: Timing cut: measured dt is within a threshold window of dt_exp
# Saved outputs are graphics/cut1_eff_vs_thresh.png, ..., graphics/cut6_eff_vs_thresh.png
# Need to change when I have simulation data - at the moment I am working with run_1572.root

import pandas as pd
import matplotlib.pyplot as plt

# To change for each cut:
    # x variable to plot: line 32
    # x label: line 33
    # name to save the graph as: line 41
    # input file name: line 44

def plot_graph(csv_path):
    """
    Function to plot the activity values obtained from my analysis alongside Antoine's analysis
    """
    # Read relevant csv
    df1 = pd.read_csv(csv_path)

    # Plot efficiency vs threshold
    plt.figure(figsize=(10,6))
    plt.scatter(df1["t_threshold_ns"], df1["efficiency"])
    plt.xlabel("t_threshold_ns", fontsize=12)
    plt.ylabel("efficiency", fontsize=12)
    plt.title("Efficiency for each threshold change", fontsize=14)
    plt.tight_layout()
    plt.legend()
    plt.grid(True)

    # Save plot as PNG
    plt.savefig("/sps/nemo/scratch/elvin/graphics/cut6_eff_vs_thresh.png")

if __name__ == "__main__":
    plot_graph("/sps/nemo/scratch/elvin/csvs/cut6_opimisation.csv")