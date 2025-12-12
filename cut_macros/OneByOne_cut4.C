// Last edited 03/12/2025
// Author: Anya Elvin
// Macro to apply 1e1gam selection cuts to ROOT data and simulation files
// This file is cut 4 only
    // Cut 4: Quantity Dt/L_e < 0.0323
// This macro is to be used to produce graphics/BestCuts_two_energiey_histos.C
// input file: "/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut3_root.root""
// output file: "/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut4_root.root"
// NOTE: Need to have already applied cut 3 before we do this

// .x cut_macros/OneByOne_cut4.C("/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut3_root.root", "/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut4_root.root")
// .x cut_macros/OneByOne_cut2.C("/sps/nemo/scratch/elvin/cut_data/OneByOne_cut1_1572_root.root", "/sps/nemo/scratch/elvin/cut_data/OneByOne_cut2_1572_root.root")

#include <iostream>
#include <cmath>
#include <vector>
#include <TString.h>
#include <TFile.h> 
#include <TTree.h>
#include <TBranch.h>

void OneByOne_cut4(const char* inputFileName, const char* outputFileName) {
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
    std::vector<double>* first_vertex_x = nullptr;
    std::vector<double>* first_vertex_y = nullptr;
    std::vector<double>* first_vertex_z = nullptr;
    std::vector<double>* second_vertex_x = nullptr;
    std::vector<double>* second_vertex_y = nullptr;
    std::vector<double>* second_vertex_z = nullptr;
    std::vector<int>* om_number = nullptr;
    std::vector<double>* calo_tdc = nullptr;
    std::vector<double>* gamma_om_x = nullptr;
    std::vector<double>* gamma_om_y = nullptr;
    std::vector<double>* gamma_om_z = nullptr;
    tree->SetBranchAddress("electron_number", &electron_number);
    tree->SetBranchAddress("gamma_number", &gamma_number);
    tree->SetBranchAddress("energy", &energy);
    tree->SetBranchAddress("first_vertex_x", &first_vertex_x);
    tree->SetBranchAddress("first_vertex_y", &first_vertex_y);
    tree->SetBranchAddress("first_vertex_z", &first_vertex_z);
    tree->SetBranchAddress("second_vertex_x", &second_vertex_x);
    tree->SetBranchAddress("second_vertex_y", &second_vertex_y);
    tree->SetBranchAddress("second_vertex_z", &second_vertex_z);
    tree->SetBranchAddress("om_number", &om_number);
    tree->SetBranchAddress("calo_tdc", &calo_tdc);
    tree->SetBranchAddress("gamma_om_x", &gamma_om_x);
    tree->SetBranchAddress("gamma_om_y", &gamma_om_y);
    tree->SetBranchAddress("gamma_om_z", &gamma_om_z);

    // Begin the process of preparing to sort through the data
    // Setup counters
    Long64_t pass_cut4 = 0; // number surviving each cut
    Long64_t surviving_electrons = 0, surviving_gammas = 0; // remaining particles of each type after all cuts have been applied

    // Constants 
    int e_idx = 0; 
    int g_idx = 1;
    const double c = 299.792458; // mm/ns
    const double m_e = 0.511; // MeV

    // Thresholds to change
    const double t_threshold = 0.05331; // ns/mm

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

        // CUT 6: timing cut
        if (calo_tdc->size() < 2) continue; // check first cut worked
        // Data: 
        double t_e_meas = calo_tdc->at(e_idx);
        double t_g_meas = calo_tdc->at(g_idx);
        double dt_meas = t_e_meas - t_g_meas;
        // 1e1gam theory:
        // electron energy
        double T_e = energy->at(e_idx);
        double E_e = T_e + m_e;
        if (E_e <= m_e) continue;
        double beta_e = sqrt(1.0 - (m_e/E_e)*(m_e/E_e));
        if (beta_e <= 0 || beta_e >= 1) continue;
        // track lengths
        // electron track length
        double dx_e = first_vertex_x->at(0) - second_vertex_x->at(0);
        double dy_e = first_vertex_y->at(0) - second_vertex_y->at(0);
        double dz_e = first_vertex_z->at(0) - second_vertex_z->at(0);
        double L_e = sqrt(dx_e*dx_e + dy_e*dy_e + dz_e*dz_e);
        // gamma track length
        double dx_g = first_vertex_x->at(0) - gamma_om_x->at(0);
        double dy_g = first_vertex_y->at(0) - gamma_om_y->at(0); 
        double dz_g = first_vertex_z->at(0) - gamma_om_z->at(0); 
        double L_g = sqrt(dx_g*dx_g + dy_g*dy_g + dz_g*dz_g);
        // expected dt
        double t_e_exp = L_e / (beta_e * c);
        double t_g_exp = L_g / c ; 
        double dt_exp = t_e_exp - t_g_exp;
        // Compare 1e1gam theory vs data
        double diff = fabs(dt_meas - dt_exp);
        double normalised_diff = diff / L_e;
        if (normalised_diff > t_threshold) continue; // reject event
        pass_cut4++;

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