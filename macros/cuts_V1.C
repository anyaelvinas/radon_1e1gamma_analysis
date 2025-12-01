// 27/11/2025
// Macro to apply 1e1gam election cuts to ROOT data and simulation files
    // Cut 1: 1 electron only and 1 photon only and nothing else
    // Cut 2: E_e > 50keV and E_gam > 50keV
    // Cut 3: E_e + E_gam ≤ 3.27keV
    // Cut 4: Timing cut: 
// This macro is to be used in pipelines/real_data_pipeline.py and pipelines/simulation_pipeline.py
// If everything seems way off, check that I have the right units for each of my constants, ie c is in 

#include <iostream>
#include <cmath>
#include <vector>
#include <TString.h>
#include <TFile.h> 
#include <TTree.h>
#include <TBranch.h>

void cuts_V1(const char* inputFileName, const char* outputFileName, bool x_tracking_electron = false) {
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
    Int_t electron_number, gamma_number, alpha_number;
    std::vector<double>* energy = nullptr;
    std::vector<double>* vertex_start_x = nullptr;
    std::vector<double>* vertex_start_y = nullptr;
    std::vector<double>* vertex_start_z = nullptr;
    std::vector<double>* vertex_end_x = nullptr;
    std::vector<double>* vertex_end_y = nullptr;
    std::vector<double>* vertex_end_z = nullptr;
    std::vector<double> *calo_tdc = nullptr;
    tree->SetBranchAddress("electron_number", &electron_number);
    tree->SetBranchAddress("gamma_number", &gamma_number);
    tree->SetBranchAddress("alpha_number", &alpha_number);
    tree->SetBranchAddress("energy", &energy);
    tree->SetBranchAddress("vertex_3D_start_x", &vertex_start_x);
    tree->SetBranchAddress("vertex_3D_start_y", &vertex_start_y);
    tree->SetBranchAddress("vertex_3D_start_z", &vertex_start_z);
    tree->SetBranchAddress("vertex_3D_end_x", &vertex_end_x);
    tree->SetBranchAddress("vertex_3D_end_y", &vertex_end_y);
    tree->SetBranchAddress("vertex_3D_end_z", &vertex_end_z);
    tree->SetBranchAddress("calo_tdc", &calo_tdc);


    // Begin the process of preparing to sort through the data
    // Setup counters
    Long64_t pass_cut1 = 0, pass_cut2 = 0, pass_cut3 = 0, pass_cut4 = 0; // number surviving each cut
    Long64_t surviving_electrons = 0, surviving_gammas = 0; // remaining particles of each type after all cuts have been applied

    // Constants 
    int e_idx = 0; 
    int g_idx = 1;
    const double Q_Bi214 = 3.27; // MeV
    double Xx = 0.0; // placeholder xvertex coordinates for non-tracked data
    double gam_end_x_pos = 435, gam_end_x_neg = -435; // gam_end_y = 0.0, gam_end_z = 0.0; // assume gamma end vertex is either (435,0,0) or (-435,0,0)
    const double c = 299.792458; // mm/ns
    const double m_e = 0.511; // MeV
    const double threshold = 4.0; // ns CHANGE THIS VALUE!!!!!!

    // Prepare output file to store cut data in
    TFile *outputFile = new TFile(outputFileName, "RECREATE");
    TTree *cutTree = tree->CloneTree(0); 

    // Now we can start sorting through the data and cutting it
    // Loop through events in Result_tree
    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);

        // CUT 1: 1 electron only and and 1 photon only and nothing else 
        if (!(electron_number == 1 && gamma_number == 1 && alpha_number == 0)) continue;
        pass_cut1++;
        std::cout << "Cut 1 complete";

        // CUT 2: electron energy > 50keV and photon energy > 50keV
        if (energy->size() < 2) continue; // check first cut worked
        double electron_energy = energy->at(e_idx); 
        double gamma_energy    = energy->at(g_idx);
        if (electron_energy < 0.05 || gamma_energy < 0.05) continue;
        pass_cut2++;
        std::cout << "Cut 2 complete";

        // CUT 3: total energy ≤ 3.27 MeV
        double Etot = electron_energy + gamma_energy;
        if (Etot > Q_Bi214) continue;
        pass_cut3++;
        std::cout << "Cut 3 complete";

        // CUT 4: timing cut
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
        double beta_e = sqrt(1.0 - pow(m_e / E_e, 2));
        if (beta_e <= 0 || beta_e >= 1) continue;
        // track lengths
        double L_e = 0.0;
        double L_g = 0.0;
        if (x_tracking_electron) {
            // electron track length
            double dx_e = vertex_end_x->at(e_idx) - vertex_start_x->at(e_idx);
            double dy_e = vertex_end_y->at(e_idx) - vertex_start_y->at(e_idx);
            double dz_e = vertex_end_z->at(e_idx) - vertex_start_z->at(e_idx);
            L_e = sqrt(dx_e*dx_e + dy_e*dy_e + dz_e*dz_e);
            // gamma track length
            double gam_end_x = (vertex_start_x->at(e_idx) >= 0 ? gam_end_x_pos : gam_end_x_neg); // choose whether it hits +435 wall or -435 wall
            double dx_g = gam_end_x - vertex_start_x->at(e_idx);
            double dy_g = vertex_end_y->at(e_idx) - vertex_start_y->at(e_idx); // AT THE MOMENT ASSUME GAMMA HITS THE SAME SPOT AS THE ELECTRON... change out for gam_end_y
            double dz_g = vertex_end_z->at(e_idx) - vertex_start_z->at(e_idx); // AT THE MOMENT ASSUME GAMMA HITS THE SAME SPOT AS THE ELECTRON... change out for gam_end_z
            L_g = sqrt(dx_g*dx_g + dy_g*dy_g + dz_g*dz_g);
        }
        else {
            // electron track length
            double dx_e = vertex_end_x->at(e_idx) - Xx;
            double dy_e = vertex_end_y->at(e_idx) - vertex_start_y->at(e_idx);
            double dz_e = vertex_end_z->at(e_idx) - vertex_start_z->at(e_idx);
            L_e = sqrt(dx_e*dx_e + dy_e*dy_e + dz_e*dz_e);
            // gamma track length
            double gam_end_x = (vertex_start_x->at(e_idx) >= 0 ? gam_end_x_pos : gam_end_x_neg); // choose whether it hits +435 wall or -435 wall
            double dx_g = gam_end_x - Xx;
            double dy_g = vertex_end_y->at(e_idx) - vertex_start_y->at(e_idx); // AT THE MOMENT ASSUME GAMMA HITS THE SAME SPOT AS THE ELECTRON... change out for gam_end_y
            double dz_g = vertex_end_z->at(e_idx) - vertex_start_z->at(e_idx); // AT THE MOMENT ASSUME GAMMA HITS THE SAME SPOT AS THE ELECTRON... change out for gam_end_z
            L_g = sqrt(dx_g*dx_g + dy_g*dy_g + dz_g*dz_g);
        }
        // expected dt
        double t_e_exp = L_e / (beta_e * c);
        double t_g_exp = L_g / c ; 
        double dt_exp = t_e_exp - t_g_exp;
        // Compare 1e1gam theory vs data
        double diff = fabs(dt_meas - dt_exp);
        if (diff > threshold) continue; // reject event
        pass_cut4++;
        std::cout << "Cut 4 complete";

        // END OF CUTS: count remaining particles
        surviving_electrons += electron_number;
        surviving_gammas += gamma_number;

        // Populate the cut dataset with remaining events
        cutTree->Fill();
    }

    // Print statements and save tree
    std::cout << "Passed cut 1 (1e1gamma): " << pass_cut1 << std::endl;
    std::cout << "Passed cut 2 (E>0.1 MeV): " << pass_cut2 << std::endl;
    std::cout << "Passed cut 3 (Etot < 3.27 MeV): " << pass_cut3 << std::endl;
    std::cout << "Passed cut 4 (timing): " << pass_cut4 << std::endl;
    std::cout << "Final surviving events: " << cutTree->GetEntries() << std::endl;
    std::cout << "Total surviving electrons: " << surviving_electrons << std::endl;
    std::cout << "Total surviving gammas: " << surviving_gammas << std::endl;

    outputFile->cd();
    cutTree->Write();
    outputFile->Close();
    inputFile->Close();
    std::cout << "\nSaved reduced dataset to " << outputFileName << std::endl;
}