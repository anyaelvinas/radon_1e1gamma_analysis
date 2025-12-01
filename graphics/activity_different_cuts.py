import pandas as pd
import matplotlib.pyplot as plt
from datetime import datetime

#def plot_graph(V1_csv_path, V2_csv_path ..., vN_csv_path):
def plot_graph(V1_csv_path):
    """
    Function to plot the activity values obtained from my analysis alongside Antoine's analysis
    """
    # Cuts V1 data
    df1 = pd.read_csv(V1_csv_path)
    df1['date'] = pd.to_datetime(df1['time_Unix'], unit='s')

    # Cuts V2 data
    #df2 = pd.read_csv(V2_csv_path)
    #df2['date'] = pd.to_datetime(df2['time_Unix'], unit='s')
    # ... etc for different versions

    # Plot activity with uncertainties
    plt.figure(figsize=(10,6))
    plt.errorbar(df1['date'], df1['A_mBq_m-3'], yerr=df1['Delta_A'], fmt='o', capsize=3, label=r"Cuts_V1")
    #plt.errorbar(df2['date'], df2["A_mBq_m-3"], yerr=df2["Delta_A"], fmt='s', capsize=3, label='1e1alpha channel analysis')
    plt.xlabel("Date", fontsize=12)
    plt.ylabel("Activity [mBq/m³]", fontsize=12)
    plt.title("Radon Activity vs Date: Various Cuts", fontsize=14)
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.legend()
    plt.grid(True)

    # Save plot as PNG
    plt.savefig("/sps/nemo/scratch/elvin/graphics/activity_different_cuts.png")

if __name__ == "__main__":
    plot_graph("/sps/nemo/scratch/elvin/csvs/real_data_summary_V1.csv")