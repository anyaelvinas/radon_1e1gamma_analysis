import pandas as pd
import matplotlib.pyplot as plt
from datetime import datetime

def plot_graph(my_csv_path, antoine_csv_path):
#def plot_graph(my_csv_path):
    """
    Function to plot the activity values obtained from my analysis alongside Antoine's analysis
    """
    # My data
    df1 = pd.read_csv(my_csv_path)
    df1['date'] = pd.to_datetime(df1['midrun_time_Unix'], unit='s')

    # Subtract 150 from activity values
    df1['A_mBq_m-3'] = df1['A_mBq_m-3'] - 150
    #df1['Delta_A'] = df1['Delta_A'] / 5 

    # Antoine's data
    df2 = pd.read_csv(antoine_csv_path)
    df2['date'] = pd.to_datetime(df2['UnixTime_midRun'], unit='s')

    # Only plot runs both Antoine and I have processed
    common = df1.merge(df2, on="Run", how="inner", suffixes=("_me", "_antoine"))

    # Plot activity with uncertainties
    plt.figure(figsize=(10,6))
    plt.errorbar(common['date_me'], common['A_mBq_m-3'], yerr=common['Delta_A'], fmt='o', capsize=3, label=r"1e1$\gamma$ channel analysis (with 150 subtracted)")
    plt.errorbar(common['date_antoine'], common[" A_stat"], yerr=common[" SE_A_stat"], fmt='s', capsize=3, label=r"1e1$\alpha$ channel analysis")
    plt.xlabel("Date", fontsize=12)
    plt.ylabel("Activity [mBq/m³]", fontsize=12)
    plt.title(r"Radon Activity vs Date: 1e1$\gamma$ vs 1e1$\alpha$ Channels", fontsize=14)
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.legend()
    plt.grid(True)

    # Save plot as PNG
    plt.savefig("/sps/nemo/scratch/elvin/graphics_data/activity_me_vs_antoine_Scaled.png")

if __name__ == "__main__":
    plot_graph("/sps/nemo/scratch/elvin/csvs/real_data_summary_V2.csv", "/sps/nemo/scratch/elvin/csvs/Antoine_BiPo.csv")