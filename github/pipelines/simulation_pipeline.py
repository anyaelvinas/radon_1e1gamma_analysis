# Last edited 30/11/2025
# Author: Anya Elvin
# Pipeline to run downloaded simulation file(s) through in order to apply cuts, calculate efficiency and append information to a csv
# csv headings: simulation_file | total_events | selected_events | efficiency | eff_uncertainty

import subprocess
import uproot
import csv
import os

# Directories, etc
original_simulation = "/sps/nemo/scratch/elvin/simulations/Bi214_wire_surface_50M.root" # change to the source foil one when the tracking one is finished
cut_simulation = "/sps/nemo/scratch/elvin/cut_simulations/Best_V3_cut_Bi214_wire_surface_50M.root"
original_alias = "Bi214_wire_surface_50M.root"
cut_macro = "/sps/nemo/scratch/elvin/cut_macros/cuts_V2.C"
output_csv = "/sps/nemo/scratch/elvin/csvs/simulation_summary_V3.csv"

def apply_cuts():
    """
    Runs macro which cuts data based on 1e1gamma channel criteria
    This macro creates output cut file
    """
    print("\nApplying cuts to root file...")

    # Run cuts macro 
    cmd = ["root", "-l", "-b", "-q", f'{cut_macro}("{original_simulation}", "{cut_simulation}")']
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print("Error running cuts_V1.C")
        print(result.stderr)
        raise RuntimeError("Cuts macro failed")
    print("\nFile cut and new cuts file created!")

def calculate_efficiency_parameters():
    """
    Gets total_events, selected_events, efficiency and efficiency uncertainty
    """
    print("\nOpening filtered simulation file...")

    # Get number of events before and after cuts 
    n_original_events = 100000000 # Originally 100M events in the simulation file before any 
    cut_file = uproot.open(cut_simulation) # after
    cut_tree = cut_file["Result_tree"]
    n_cut_events = cut_tree.num_entries
    
    # Calculate efficiency and uncertainty
    efficiency = n_cut_events / n_original_events 
    eff_uncertainty = (efficiency * (1 - efficiency) / n_original_events) ** 0.5 
    print("\nEfficiency calculated!")
    return n_original_events, n_cut_events, efficiency, eff_uncertainty

def update_csv(sim_alias, total, selected, efficiency, eff_uncertainty):
    """
    Appends one row to simulation_results.csv
    If analysis on this dataset has previously been done, it will be overwritten
    """
    print("\nAdding information to simulation_summary.csv...")

    # Load csv
    rows = [] # To load existing rows into
    if os.path.isfile(output_csv):
        with open(output_csv, "r", newline="") as f:
            reader = csv.reader(f)
            rows = list(reader) # rows are a list of lists where each list is a line in the csv
        header = rows[0]
        data_rows = rows[1:]
        data_rows = [r for r in data_rows if r[0] != sim_alias] # filter out old entries with same sim_alias
    # Create csv if not already made
    else:
        header = ["simulation_file", "total_events", "selected_events", "efficiency", "eff_uncertainty"]
        data_rows = []
    
    # Write information
    data_rows.append([sim_alias, total, selected, efficiency, eff_uncertainty])
    with open(output_csv, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(header)
        writer.writerows(data_rows)
    print("\ncsv file updated!")

def run_pipeline():
    print("\n Starting simulation pipeline")
    apply_cuts()
    total, selected, efficiency, eff_uncertainty = calculate_efficiency_parameters()
    update_csv(original_alias, total, selected, efficiency, eff_uncertainty)
    print("\nPipeline finished successfully!\n")


if __name__ == "__main__":
    run_pipeline()

