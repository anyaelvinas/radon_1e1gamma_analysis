This repository contains a selection of the most important files used to carry out analysis for my MSc dissertation project: "Off-Plane 1e1γ Channel Analysis for Radon Background Reduction at the SuperNEMO Experiment"

The repository contains 4 folders: 
1. metadata_lists - containing .list files with information about detector runs.
2. cut_macros - containing C macros used to apply selection cuts to the data. OneByOne_[X].C applies one cut only, whereas cuts_V2.C combines all these cuts into one process.
3. pipelines - containing the code used to apply cuts to simulation and data files, calculating efficiencies and activities to store in csv files.
4. csvs - separate csv files for simulation and data, containing various relevant information.

Without access to the data and simulation files, the pipelines and cut macros will not work.
