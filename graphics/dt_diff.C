// Last edited 04/12/2025
// Author: Anya Elvin
// Macro to draw a histogram showing the diff = |dt_meas - dt_exp|
// This helps figure out my thresholds for cut 6
// This macro saves the output as a png file at the moment (graphics/dt_diff.png)
// Would ideally like to save as a root, but it is complicated in ssh!
// Change the input file to be the simulation when I have it

#include <iostream>
#include <cmath>
#include <vector>
#include <TString.h>
#include <TFile.h> 
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TBranch.h>

void dt_diff() {
    // Open cut file
    //TFile *f = TFile::Open("/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut3_root.root"); // simulation
    TFile *f = TFile::Open("/sps/nemo/scratch/elvin/cut_data/OneByOne_cut3_1572_root.root"); // data 
    if (!f || f->IsZombie()) {
        std::cerr << "Error: Cannot open file" << std::endl;
        return;
    }
    TTree *tree = (TTree*) f->Get("Result_tree"); 
    if (!tree) {
        std::cerr << "Error: Cannot find 'Result_tree' in file." << std::endl;
        return;
    }
    // Branches
    Int_t electron_number, gamma_number;
    std::vector<double>* calo_tdc = nullptr;
    std::vector<double>* energy = nullptr;
    std::vector<double>* first_vertex_x = nullptr;
    std::vector<double>* first_vertex_y = nullptr;
    std::vector<double>* first_vertex_z = nullptr;
    std::vector<double>* second_vertex_x = nullptr;
    std::vector<double>* second_vertex_y = nullptr;
    std::vector<double>* second_vertex_z = nullptr;
    std::vector<double>* gamma_om_x = nullptr;
    std::vector<double>* gamma_om_y = nullptr;
    std::vector<double>* gamma_om_z = nullptr;
    tree->SetBranchAddress("electron_number", &electron_number);
    tree->SetBranchAddress("gamma_number", &gamma_number);
    tree->SetBranchAddress("calo_tdc", &calo_tdc);
    tree->SetBranchAddress("energy", &energy);
    tree->SetBranchAddress("first_vertex_x", &first_vertex_x);
    tree->SetBranchAddress("first_vertex_y", &first_vertex_y);
    tree->SetBranchAddress("first_vertex_z", &first_vertex_z);
    tree->SetBranchAddress("second_vertex_x", &second_vertex_x);
    tree->SetBranchAddress("second_vertex_y", &second_vertex_y);
    tree->SetBranchAddress("second_vertex_z", &second_vertex_z);
    tree->SetBranchAddress("gamma_om_x", &gamma_om_x);
    tree->SetBranchAddress("gamma_om_y", &gamma_om_y);
    tree->SetBranchAddress("gamma_om_z", &gamma_om_z);

    // Constants
    const int e_idx = 0;
    const int g_idx = 1;
    const double m_e = 0.511; // MeV
    const double c = 300.0; // mm/ns

    // Histogram
    TH1D* h_diff = new TH1D("h_diff", "#Delta t;#Delta t[ns];Events", 200, 0, 20);

    Long64_t nEntries = tree->GetEntries();
    std::cout << "Total rows: " << nEntries << std::endl;
    // Loop over rows: cut file guarantees 1 electron + 1 gamma per event
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);

        if (!calo_tdc || calo_tdc->size() < 2) {
        std::cout << "Row " << i << " has less than 2 TDC entries. Skipping.\n";
        continue;
    }

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
        // Gamma row
        double dx_g = first_vertex_x->at(0) - gamma_om_x->at(0);
        double dy_g = first_vertex_y->at(0) - gamma_om_y->at(0);
        double dz_g = first_vertex_z->at(0) - gamma_om_z->at(0);
        double L_g = sqrt(dx_g*dx_g + dy_g*dy_g + dz_g*dz_g);
        // expected dt
        double t_e_exp = L_e / (beta_e * c);
        double t_g_exp = L_g / c;
        double dt_exp = t_e_exp - t_g_exp;
        // Compare 1e1gam theory vs data
        double diff = fabs(dt_meas - dt_exp);
        //double normalised_diff = diff / L_e;
        h_diff->Fill(diff);
        //h_diff->Fill(normalised_diff);
    }

    // Draw and save
    TCanvas *c1 = new TCanvas("c1", "Timing Difference", 800, 600);
    h_diff->SetLineColor(kBlue);
    h_diff->SetLineWidth(2);
    h_diff->Draw();
    c1->SaveAs("/sps/nemo/scratch/elvin/graphics_data/dt_diff.png");

    f->Close();
    std::cout << "Saved to graphics_data/dt_diff.png" << std::endl;
}