// Last edited 03/12/2025
// Author: Anya Elvin
// Macro to apply 1e1gam selection cuts to ROOT data and simulation files
// This file is cut 6 only
    // Cut 6: E_e + E_gam ≤ 2.66<eV
// This macro is to be used to produce graphics/BestCuts_two_energiey_histos.C
// input file: "/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut5_root.root""
// output file: "/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut6_root.root"
// NOTE: Need to have already applied cut 5 before we do this

#include <iostream>
#include <cmath>
#include <vector>
#include <TString.h>
#include <TFile.h> 
#include <TTree.h>
#include <TBranch.h>

void OneByOne_cut6(const char* inputFileName, const char* outputFileName) {
    // Load the dataset 
    TFile *inputFile = TFile::Open(inputFileName, "READ");
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "Error: Cannot open input file " << inputFileName << std::endl;
        return;
    }
    TTree *tree = (TTree*) inputFile->Get("Result_tree");
    if (!tree) {
        std::cerr << "Error: Cannot find TTree 'Result_tree' in file." << std::endl;
        return;
    }

    // Set up branches
    Int_t electron_number, gamma_number;
    std::vector<double>* energy = nullptr;
    tree->SetBranchAddress("electron_number", &electron_number);
    tree->SetBranchAddress("gamma_number", &gamma_number);
    tree->SetBranchAddress("energy", &energy);

    // Begin the process of preparing to sort through the data
    // Setup counters
    Long64_t pass_cut6 = 0; // number surviving each cut
    Long64_t surviving_electrons = 0, surviving_gammas = 0; // remaining particles of each type after all cuts have been applied

    // Constants 
    int e_idx = 0; 
    int g_idx = 1;

    // Thresholds to change
    const double max_E_tot = 3.00; // MeV

    // Prepare output file to store cut data in
    TFile *outputFile = new TFile(outputFileName, "RECREATE");
    TTree *cutTree = tree->CloneTree(0); 

    // Now we can start sorting through the data and cutting it
    // Loop through events in Result_tree
    std::cout << "\nStarting to loop through simulation file " << std::endl;
    Long64_t nEntries = tree->GetEntries();
    std::cout << "Number of events: " << nEntries << std::endl;
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);

        // CUT 3: total energy ≤ 2.66 MeV
        if (energy->size() < 2) continue; // check first cut worked
        double electron_energy = energy->at(e_idx); 
        double gamma_energy = energy->at(g_idx);
        double E_tot = electron_energy + gamma_energy;
        if (E_tot > max_E_tot) continue;
        pass_cut6++;

        // END OF CUTS: count remaining particles
        surviving_electrons += electron_number;
        surviving_gammas += gamma_number;

        // Populate the cut dataset with remaining events
        cutTree->Fill();
    }

    outputFile->cd();
    cutTree->Write();
    outputFile->Write();
    outputFile->Close();
    inputFile->Close();
    std::cout << "\nSaved reduced dataset to " << outputFileName << std::endl;
}
