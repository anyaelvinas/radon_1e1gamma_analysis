# Last edited 05/12/2025
# Author: Anya Elvin
# Pipeline to run through various thresholds for a particular cut
# CURRENTlY WORKING WITH CUT 5
# run downloaded simulation file(s) through in order to apply cuts, calculate efficiency and append information to a csv
# csv headings: [edit] | simulation_file | total_events | selected_events | efficiency | eff_uncertainty

# To run for different cuts:
    # change line 4
    # change cut_number: line 25
    # change output_csv name: line 35
    # uncomment list thresholds list: lines 38-42
    # change number of sig figs to round to: line 43
    # edit name of header in update_csv(): line 96
    # edit name of header in if __name__ == "__main__": line 132
    # Go to cut_macros/all_cuts_for_optimisation.C and apply listed changes there

import subprocess
import uproot
import csv
import os
import pandas as pd
import numpy as np

cut_number = 5

# Directories, etc
original_simulation = "/sps/nemo/scratch/elvin/simulations/Bi214_wire_surface_50M.root"
original_alias = "Bi214_wire_surface_50M.root"
original_file = uproot.open(original_simulation)
original_tree = original_file["Result_tree"]
N_orig = original_tree.num_entries
output_directory = "/sps/nemo/scratch/elvin/cut_simulations"
cut_macro = "/sps/nemo/scratch/elvin/cut_macros/all_cuts_for_optimisation.C"
output_csv = "/sps/nemo/scratch/elvin/csvs/cut5_opimisation.csv"

# Thresholds to loop through, etc
#thresholds = [1, 2] # cut 1: vary the number of gammas - set sigfigs to 1
#thresholds = np.arange(0.05, 0.15 + 0.001, 0.01).tolist() # cut 2: vary the minimum energy threshold - set sigfigs to 3
#thresholds = np.arange(3.0, 4.0 + 0.001, 0.1).tolist() # cut 3: vary max E_tot - set sigfigs to 2
thresholds = np.arange(45, 90 + 0.001, 5).tolist() # cut 5: vary the buffer distance - set sigfigs to 2
#thresholds = np.arange(0, 5 + 0.001, 1).tolist() # cut 6: vary the threshold time - set sigfigs to 1
sigfigs = 2

def apply_cuts(cut_simulation, threshold):
    """
    Runs macro which cuts data based on 1e1gamma channel criteria
    This macro creates output cut file
    """
    print("\nApplying cuts to root file...")

    # Run cuts macro 
    cmd = ["root", "-l", "-b", "-q", f'{cut_macro}("{original_simulation}", "{cut_simulation}", {threshold})']
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error running {cut_macro} with threshold {threshold}")
        print(result.stderr)
        raise RuntimeError("Cuts macro failed")
    print("\nFile cut and new cuts file created!")

def calculate_efficiency_parameters(cut_simulation):
    """
    Gets total_events, selected_events, efficiency and efficiency uncertainty
    """
    print("\nOpening filtered simulation file...")

    # Get number of events after cuts 
    cut_file = uproot.open(cut_simulation) # after
    cut_tree = cut_file["Result_tree"]
    N_cut = cut_tree.num_entries
    
    # Calculate efficiency and uncertainty
    efficiency = N_cut / N_orig
    eff_uncertainty = (efficiency * (1 - efficiency) / N_orig) ** 0.5 
    print("\nEfficiency calculated!")
    return N_cut, efficiency, eff_uncertainty

def update_csv(edit, sim_alias, total, selected, efficiency, eff_uncertainty):
    """
    Appends one row to simulation_results.csv
    If analysis on this dataset has previously been done, it will be overwritten
    """
    print("\nAdding information to csv...")

    # Load csv
    rows = [] # To load existing rows into
    if os.path.isfile(output_csv):
        with open(output_csv, "r", newline="") as f:
            reader = csv.reader(f)
            rows = list(reader) # rows are a list of lists where each list is a line in the csv
        header = rows[0]
        data_rows = rows[1:]
        data_rows = [r for r in data_rows if str(r[0]) != str(edit)] # filter out old entries with same sim_alias
    # Create csv if not already made
    else:
        header = ["edge_buffer_mm", "simulation_file", "total_events", "selected_events", "efficiency", "eff_uncertainty"]
        data_rows = []
    
    # Write information
    data_rows.append([edit, sim_alias, total, selected, efficiency, eff_uncertainty])
    with open(output_csv, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(header)
        writer.writerows(data_rows)
    print("\ncsv file updated!")

def run_pipeline():
    print("\n Starting simulation pipeline")

    for threshold in thresholds:
        print("Current threshold: ", round(threshold, sigfigs))

        # Format strings and directories properly
        str_threshold = str(round(threshold, sigfigs))
        if isinstance(threshold, float) or '.' in str_threshold:
            str_threshold = str_threshold.replace('.', 'p')
        cut_basename = f"cut{cut_number}_{str_threshold}_{original_alias}"
        cut_sim_name = os.path.join(output_directory, cut_basename)

        # Go through pipeline
        apply_cuts(cut_sim_name, threshold)
        N_cut, efficiency, eff_uncertainty = calculate_efficiency_parameters(cut_sim_name)
        update_csv(round(threshold, sigfigs), original_alias, N_orig, N_cut, efficiency, eff_uncertainty)
    print("\nPipeline finished successfully!\n")


if __name__ == "__main__":
    run_pipeline()
    # Make sure it's in the right order
    if os.path.isfile(output_csv):
        df = pd.read_csv(output_csv)
        df.sort_values(by="edge_buffer_mm", inplace=True)
        df.to_csv(output_csv, index=False)