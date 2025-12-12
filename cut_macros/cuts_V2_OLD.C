// Last edited 01/12/2025
// Author: Anya Elvin
// Macro to apply 1e1gam selection cuts to ROOT data and simulation files
    // Cut 1: 1 electron only and 1 photon only and nothing else
    // Cut 2: E_e > 10keV and E_gam > 10keV
    // Cut 3: E_e + E_gam ≤ 2.66keV
    // Cut 4: gam OM number ≤ 519
    // Cut 5: Electron start vertex has to be ≥45mm away from edges 
    // Cut 6: Timing cut: measured dt is within a threshold window of dt_exp
// This macro is to be used in pipelines/real_data_pipeline.py and pipelines/simulation_pipeline.py
// If everything seems way off, check that I have the right units for each of my constants, ie c is in 

#include <iostream>
#include <cmath>
#include <vector>
#include <TString.h>
#include <TFile.h> 
#include <TTree.h>
#include <TBranch.h>

void cuts_V2(const char* inputFileName, const char* outputFileName) {
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
    Long64_t pass_cut1 = 0, pass_cut2 = 0, pass_cut3 = 0, pass_cut4 = 0, pass_cut5 = 0, pass_cut6 = 0; // number surviving each cut
    Long64_t surviving_electrons = 0, surviving_gammas = 0; // remaining particles of each type after all cuts have been applied

    // Constants 
    int e_idx = 0; 
    int g_idx = 1;
    const double c = 299.792458; // mm/ns
    const double m_e = 0.511; // MeV

    // Thresholds to change
    const double min_E = 0.05; // 10keV
    const double max_E_tot = 3.00; // MeV
    const double y_max_pos = 2494; 
    const double y_max_neg = -2494; 
    const double x_max_pos = 436; 
    const double x_max_neg = -436; 
    const double x_calo_buffer = 100.0; // mm
    const double x_foil_buffer = 60.0; // mm
    const double y_buffer = 30.0; // mm
    const double t_threshold = 0.03230; // ns/mm

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

        // CUT 2: electron energy > 50keV and photon energy > 50keV
        if (energy->size() < 2) continue; // check first cut worked
        double electron_energy = energy->at(e_idx); 
        double gamma_energy = energy->at(g_idx);
        if (electron_energy < min_E || gamma_energy < min_E) continue;
        pass_cut2++;

        // CUT 3: total energy ≤ 3.27 MeV
        double E_tot = electron_energy + gamma_energy;
        if (E_tot > max_E_tot) continue;
        pass_cut3++;

        // CUT 4: gamma OM number ≤ 519
        if (gamma_om_x->empty()) continue; // if there is no entries, skip
        if (om_number->at(0) > 519) continue; 
        pass_cut4++;

        // CUT 5: avoid electron start vertices in buffer zones
        // Dodge x buffer zones
        if (first_vertex_x->at(e_idx) > -1*x_foil_buffer && first_vertex_x->at(e_idx) < x_foil_buffer) continue;
        if (first_vertex_x->at(e_idx) > x_max_pos - x_calo_buffer || first_vertex_x->at(e_idx) < x_max_neg + x_calo_buffer) continue;
        // Dodge y buffer zones
        if (first_vertex_y->at(e_idx) > y_max_pos - y_buffer || first_vertex_y->at(e_idx) < y_max_neg + y_buffer) continue;
        pass_cut5++;

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
        double beta_e = sqrt(1.0 - pow(m_e/E_e, 2));
        if (beta_e <= 0 || beta_e >= 1) continue;
        // track lengths
        // electron track length
        double dx_e = first_vertex_x->at(e_idx) - second_vertex_x->at(e_idx);
        double dy_e = first_vertex_y->at(e_idx) - second_vertex_y->at(e_idx);
        double dz_e = first_vertex_z->at(e_idx) - second_vertex_z->at(e_idx);
        double L_e = sqrt(dx_e*dx_e + dy_e*dy_e + dz_e*dz_e);
        // gamma track length
        double dx_g = first_vertex_x->at(e_idx) - gamma_om_x->at(e_idx);
        double dy_g = first_vertex_y->at(e_idx) - gamma_om_y->at(e_idx); 
        double dz_g = first_vertex_z->at(e_idx) - gamma_om_z->at(e_idx); 
        double L_g = sqrt(dx_g*dx_g + dy_g*dy_g + dz_g*dz_g);
        // expected dt
        double t_e_exp = L_e / (beta_e * c);
        double t_g_exp = L_g / c ; 
        double dt_exp = t_e_exp - t_g_exp;
        // Compare 1e1gam theory vs data
        double diff = fabs(dt_meas - dt_exp);
        double normalised_diff = diff / L_e;
        if (normalised_diff > t_threshold) continue; // reject event
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
