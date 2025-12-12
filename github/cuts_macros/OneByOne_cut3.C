// Last edited 09/12/2025
// Author: Anya Elvin
// Macro to apply 1e1gam selection cuts to ROOT data and simulation files
// This file is cut 3 only
    // Cut 3: Electron start vertex has to be within confined window
// input file: "/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut2_root.root"
// output file: "/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut3_root.root"
// NOTE: Need to have already applied cut 2 before we do this

#include <iostream>
#include <cmath>
#include <vector>
#include <TString.h>
#include <TFile.h> 
#include <TTree.h>
#include <TBranch.h>

void OneByOne_cut3(const char* inputFileName, const char* outputFileName) {
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
    std::vector<double>* first_vertex_x = nullptr;
    std::vector<double>* first_vertex_y = nullptr;
    tree->SetBranchAddress("electron_number", &electron_number);
    tree->SetBranchAddress("gamma_number", &gamma_number);
    tree->SetBranchAddress("first_vertex_x", &first_vertex_x);
    tree->SetBranchAddress("first_vertex_y", &first_vertex_y);

    // Begin the process of preparing to sort through the data
    // Setup counters
    Long64_t pass_cut3 = 0; // number surviving each cut
    Long64_t surviving_electrons = 0, surviving_gammas = 0; // remaining particles of each type after all cuts have been applied

    // Constants 
    int e_idx = 0; 
    int g_idx = 1;
    const double y_max_pos = 2494; 
    const double y_max_neg = -2494; 
    const double x_max_pos = 436; 
    const double x_max_neg = -436; 
    const double x_calo_buffer = 100.0; // mm
    const double x_foil_buffer = 60.0; // mm
    const double y_buffer = 30.0; // mm

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

        // CUT 5: avoid electron start vertices in buffer zones
        // Dodge x buffer zones
        if (first_vertex_x->at(e_idx) > -1*x_foil_buffer && first_vertex_x->at(e_idx) < x_foil_buffer) continue;
        if (first_vertex_x->at(e_idx) > x_max_pos - x_calo_buffer || first_vertex_x->at(e_idx) < x_max_neg + x_calo_buffer) continue;
        // Dodge y buffer zones
        if (first_vertex_y->at(e_idx) > y_max_pos - y_buffer || first_vertex_y->at(e_idx) < y_max_neg + y_buffer) continue;
        pass_cut3++;

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