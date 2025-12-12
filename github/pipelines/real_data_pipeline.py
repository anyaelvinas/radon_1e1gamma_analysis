# Last edited 01/12/2025
# Author: Anya Elvin
# Code to run data files through in order to apply cuts, calculate activity and append information to a csv
# csv headings: run | time | duration | phase | N_orig | N_cut | cut_file | activity | activity_uncertainty

# STILL TO DO: calculate proper errors
# Change the thresholds
# Before running this code, do "module load root" in the terminal window below

import subprocess
import uproot
import csv
import os
import pandas as pd

# Paths to files and constants
data_directory = "/sps/nemo/scratch/elvin/data"
cut_data_directory = "/sps/nemo/scratch/elvin/cut_data"
metadata_file_1 = "/sps/nemo/scratch/elvin/metadata_lists/UDD_betabeta_v1.list"
metadata_file_2 = "/sps/nemo/scratch/elvin/metadata_lists/UDD_betabeta_v1.list"
cut_macro = "/sps/nemo/scratch/elvin/cut_macros/cuts_V2.C" # Change this when cuts change
simulation_summary_csv = "/sps/nemo/scratch/elvin/csvs/simulation_summary_V2.csv"
output_csv = "/sps/nemo/scratch/elvin/csvs/real_data_summary_V2.csv" # CHANGE NAME WHEN WORKING WITH ALTERED CUTS - right now its for V1
detector_vol = 15.4 

def get_metadata(data_filepath):
    """
    Extract metadata for current datafile (run number, time, duration, phase)
    datafile_path is ".../elvin/data/run_1547.root"
    """
    print("\nExtracting metadata...")
    base_name = os.path.basename(data_filepath)  # ie. "run_1547.root"
    run_str = "".join([c for c in base_name if c.isdigit()])
    run = int(run_str) # to get just 1547

    # Find file in metadata list
    df1 = pd.read_csv(metadata_file_1, comment='#', header=None, names=['RUN','RUN_START','DURATION','STOP','COMMENT'], engine='python', sep=r'\s+', usecols=[0,1,2,3,4])
    df2 = pd.read_csv(metadata_file_1, comment='#', header=None, names=['RUN','RUN_START','DURATION','STOP','COMMENT'], engine='python', sep=r'\s+', usecols=[0,1,2,3,4])
    row = df1[df1['RUN'] == run] # check v1 first
    if row.empty:
        row = df2[df2['RUN'] == run] # if not it should be in v2
    if row.empty:
        raise ValueError(f"Run {run} not found in metadata files!")
    
    # Get time, duration and phase
    duration = float(row['DURATION'].values[0]) 
    time = float(row['RUN_START'].values[0]) + 0.5*duration
    if 1546 <= run <= 1798: # Radon injection
        phase = 0
    elif 2000 <= run <= 2672: # Post-injection
        phase = 1
    else:
        phase = 2 # Possibly with anti-radon air installed?
        print(f"\nPhase of run {run} not identified!")
    print("\nMetadata collected!")
    return run, time, duration, phase

def apply_cuts(data_filepath):
    """
    Run ROOT macro to apply cuts to dataset
    data_filepath is the full file path, ie. sps/nemo/scratch/elvin/data/run_1547.root
    """
    print("\nApplying cuts to root file...")
    # Get number of events before cuts applied
    original = uproot.open(data_filepath)
    original_tree = original["Result_tree"]
    N_orig = original_tree.num_entries
    
    # Setup input and output file to cut
    base_name = os.path.basename(data_filepath)
    cut_name = base_name.replace(".root", "_cut.root") # ie. run_1547_cut.root
    output_cut_file = os.path.join(cut_data_directory, cut_name) # full cut file path 

    # Apply cuts
    cmd = ["root", "-l", "-b", "-q", f'{cut_macro}("{data_filepath}", "{output_cut_file}")']
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"\nError running cut macro on {base_name}:\n{result.stderr}")
        raise RuntimeError(f"ROOT macro failed for {base_name}")
    
    # Get number of events after cuts applied, and cut_file
    cut = uproot.open(output_cut_file)
    cut_tree = cut["Result_tree"]
    N_cut = cut_tree.num_entries
    print("\nFile cut and new cuts file created!")
    return cut_name, N_orig, N_cut

def calculate_activity(duration, N_orig, N_cut):
    """
    Calculate activity from duration and number of events after selection cuts
    """
    print("\nCalculating Rn activity...")
    # Extract average efficiency calculation
    df = pd.read_csv(simulation_summary_csv)
    efficiency = df['efficiency'].mean()
    eff_unc = df['eff_uncertainty'].mean()

    # Calculate activity in mBq / m^3 and uncertainty
    A = 1000 * N_cut / (duration * detector_vol * efficiency)
    A_unc = A * ( 1/N_orig + (eff_unc/efficiency)**2 + (0.1/(2*(3**0.5)*duration))**2 + (0.1/(2*(3**0.5)*detector_vol))**2)**0.5
    A_unc = A * ( 1/N_orig + (eff_unc/efficiency)**2 + (0.1/(2*(3**0.5)*duration))**2 + (0.1/(2*(3**0.5)*detector_vol))**2)**0.5

    print("\nActivity calculated!")
    return A, A_unc

def update_csv(run_number, time, duration, phase, N_orig, N_cut, cut_file, A, A_unc):
    """
    Append all info to CSV file
    """
    print("\nAdding information to real_data_summary.csv...")

    # Load csv
    rows = [] # To load existing rows into
    if os.path.isfile(output_csv):
        with open(output_csv, "r", newline="") as f:
            reader = csv.reader(f)
            rows = list(reader) # rows are a list of lists where each list is a line in the csv
        header = rows[0]
        data_rows = rows[1:]
        data_rows = [r for r in data_rows if str(r[0]) != str(run_number)] # filter out old entries which have same run name
    # Create csv if not already made
    else:
        header = ["Run", "midrun_time_Unix", "Duration_s", "Phase", "N_orig", "N_cut", "cut_file", "A_mBq_m-3", "Delta_A"]
        data_rows = []
    
    # Write information
    data_rows.append([run_number, time, duration, phase, N_orig, N_cut, cut_file, A, A_unc])
    with open(output_csv, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(header)
        writer.writerows(data_rows)
    print("\ncsv file updated!")

def run_pipeline():
    """
    Run whole pipeline from beginning to end
    """
    print("\nStarting data pipeline...")

    # Loop through each root file in the data directory 
    for file in os.listdir(data_directory):
        data_filepath = os.path.join(data_directory, file) # ie. ".../elvin/data/run_1547.root"
        run, time, duration, phase = get_metadata(data_filepath)
        cut_file, N_orig, N_cut = apply_cuts(data_filepath)
        A, A_unc = calculate_activity(duration, N_orig, N_cut)
        update_csv(run, time, duration, phase, N_orig, N_cut, cut_file, A, A_unc)
        print(f"\nFinished run {run}, starting work on next dataset...")

    print("\nPipeline finished!")


if __name__ == "__main__":
    run_pipeline()
    # Make sure it's in the right order
    if os.path.isfile(output_csv):
        df = pd.read_csv(output_csv)
        df.sort_values(by="Run", inplace=True)
        df.to_csv(output_csv, index=False)