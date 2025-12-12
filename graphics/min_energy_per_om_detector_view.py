# Last edited 03/12/2025
# Author: Anya Elvin
# Script to plot the minimum energies recorded by each optical module after the first cut has been applied (1 electron and 1 photon)
# Used for checking the minimum energy each OM can read, so that I can check where to put my minimum energy cut (cut 2)
# Saved output is graphics/min_energy_per_om_detector_view.png. I have annotated it with France and Italy, but I need to also annotate it with mountain and tunnel

import numpy as np 
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
import uproot
import csv

# Import ROOT file 
# Simulation
file = uproot.open("/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut4_root.root") # simulation 
#file = uproot.open("/sps/nemo/scratch/elvin/cut_data/OneByOne_cut1_1572_root.root") # data
tree = file["Result_tree"]
om_numbers = tree["om_number"].array(library="np")
energies = tree["energy"].array(library="np")

# Flatten all OM hits across all events
all_oms = np.concatenate(om_numbers)
all_energies = np.concatenate(energies)
max_om_data = int(np.nanmax(all_oms))

# Calculate minimum energy per OM
min_energy = np.full(712, np.nan) # temporarily store each minimum energy to be np.nan, until you overwrite them
for om, energy in zip(all_oms, all_energies):
    if np.isnan(min_energy[om]):
        min_energy[om] = energy
    else:
        if energy < min_energy[om]:
            min_energy[om] = energy
min_energy_no_nan = np.nan_to_num(min_energy, nan=0.0)

# Import detector layout CSV
grid_file = "csvs/detector_om_layout_wide.csv" 
custom_grid = []
with open(grid_file, newline='', encoding='utf-8-sig') as csvfile:
    reader = csv.reader(csvfile)
    for row in reader:
        cleaned = []
        for cell in row:
            cell_clean = cell.strip()
            if cell_clean == "None" or cell_clean == "":
                cleaned.append(None)
            else:
                cleaned.append(int(cell_clean))
        custom_grid.append(cleaned)

# Create hit intensity grid
rows, cols = len(custom_grid), len(custom_grid[0])

colors = [(1, 1, 1), (1, 0.8, 0.8), (0.8, 0, 0)]
custom_cmap = LinearSegmentedColormap.from_list("white_to_red", colors, N=256)

# Prepare grid for plotting
grid_rgb = np.zeros((rows, cols, 3))  # RGB grid manually built

# Fill RGB grid
max_energy = np.nanmax(min_energy_no_nan)
min_nonzero = np.nanmin(min_energy_no_nan[min_energy_no_nan > 0])
for r, row in enumerate(custom_grid):
    for c, om in enumerate(row):
        if om is None:
            grid_rgb[r, c] = [0, 0, 0] # Black for 'None'
        else:
            E_min = min_energy_no_nan[om]
            if E_min == 0:
                grid_rgb[r, c] = [0.6, 0.8, 1.0]  # Pale blue for no events in that OM
            else:
                norm_val = (E_min - min_nonzero) / (max_energy - min_nonzero)
                norm_val = max(0, min(norm_val, 1))
                grid_rgb[r, c] = custom_cmap(norm_val)[:3]

# Plot: Minimum energy per OM in detector layout
fig = plt.figure(figsize=(20, 8))
ax_grid = plt.gca()  # get current axes
ax_grid.imshow(grid_rgb, origin="upper", interpolation="none")
ax_grid.set_title("Minimum Energy Recorded per OM (MeV)", fontsize=14)
ax_grid.set_xticks(np.arange(-0.5, cols, 1))
ax_grid.set_yticks(np.arange(-0.5, rows, 1))
ax_grid.grid(color="black", linewidth=0.3, alpha=0.5)
ax_grid.set_xticklabels([])
ax_grid.set_yticklabels([])


# Label the minimum energy
for r, row in enumerate(custom_grid):
    for c, om in enumerate(row):
        if om is not None:
            E_min = min_energy_no_nan[om]
            if E_min == 0:
                label = ""  # or "0" if you prefer
            else:
                label = f"{E_min:.2f}"  # format to 2 decimals
            ax_grid.text(c, r, label, color='black', ha='center', va='center', fontsize=6)

"""
# Label the OM number
for r, row in enumerate(custom_grid): 
    for c, om in enumerate(row): 
        if om is not None: 
            ax_grid.text(c, r, str(om),color='black', ha='center', va='center', fontsize=6)
"""
            
# Add colour bar on the left (only for red scale)
sm = plt.cm.ScalarMappable(cmap=custom_cmap, norm=plt.Normalize(vmin=min_nonzero, vmax=max_energy))
sm.set_array([])
cbar = plt.colorbar(sm, ax=ax_grid, fraction=0.040, pad=0.05, location='left')
cbar.set_label("Minimum Energy (MeV)", fontsize=12)

# Layout tweaks
plt.tight_layout()
plt.savefig("/sps/nemo/scratch/elvin/graphics_data/min_energy_per_om_detector_view.png", dpi=300)
plt.show()