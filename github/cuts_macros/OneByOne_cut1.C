// Last edited 03/12/2025
// Author: Anya Elvin
// Macro to apply 1e1gam selection cuts to ROOT data and simulation files
// This file is cut 1 only
    // Cut 1: 1 electron only and 1 photon only and nothing else
// This macro is to be used to produce graphics/BestCuts_two_energiey_histos.C
// input file: "/sps/nemo/scratch/elvin/simulations/Bi214_wire_surface_50M.root""
// output file: "/sps/nemo/scratch/elvin/cut_data/OneByOne_cut1_1572_root.root"

#include <iostream>
#include <cmath>
#include <vector>
#include <TString.h>
#include <TFile.h> 
#include <TTree.h>
#include <TBranch.h>

void OneByOne_cut1(const char* inputFileName, const char* outputFileName) {
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
    tree->SetBranchAddress("electron_number", &electron_number);
    tree->SetBranchAddress("gamma_number", &gamma_number);

    // Begin the process of preparing to sort through the data
    // Setup counters
    Long64_t pass_cut1 = 0;
    Long64_t surviving_electrons = 0, surviving_gammas = 0; // remaining particles of each type after all cuts have been applied

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

        // CUT 1: 1 electron only and and 1 photon only and nothing else 
        if (!(electron_number == 1 && gamma_number == 1)) continue;
        pass_cut1++;

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