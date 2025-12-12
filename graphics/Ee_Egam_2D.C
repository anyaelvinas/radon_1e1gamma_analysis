// Last edited 03/12/2025
// Author: Anya Elvin
// Macro to draw a 2D histogram of the energy of electrons vs the energy of gammas after the simulation has been cut 2x
// This helps figure out how what E_tot threshold I should use for cut 3
// This macro saves the output as a png file at the moment (graphics/Ee_Egam_2D.png)
// Would ideally like to save as a root, but it is complicated in ssh!
// Also it looks really weird at the moment but it will look much better with more statistics

#include <vector>
#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include <iostream>
#include "TPaveStats.h"

void Ee_Egam_2D() {
    // Open the ROOT file and get the tree
    //TFile *f = TFile::Open("/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut5_root.root"); // simulation
    TFile *f = TFile::Open("/sps/nemo/scratch/elvin/cut_data/OneByOne_cut5_1572_root.root"); // data 
    if (!f || f->IsZombie()) { 
        std::cerr << "Error opening file\n"; 
        return; 
    }
    TTree *t = (TTree*)f->Get("Result_tree");
    if (!t) { 
        std::cerr << "Tree not found\n"; 
        return; 
    }

    // Create canvas
    TCanvas *c = new TCanvas("c", "2D Histogram Ee vs Eg", 800, 600);

    // Create 2D histogram: 200 bins from 0–5 on both axes
    TH2F *h2 = new TH2F("h2", "Electron Energy vs Gamma Energy (MeV);E_e;E_#gamma", 35, 0, 3.5, 35, 0, 3.5);

    // Create gradient red color palette
    const Int_t Number = 2; // two anchors
    Double_t Red[Number] = {1.00, 0.50}; // dark red to pale pink
    Double_t Green[Number] = {0.80, 0.00}; // small amount in order for pink to show
    Double_t Blue[Number] = {0.80, 0.00}; // small amount in order for prink to show
    Double_t Length[Number] = {0.00, 1.00}; // positions along gradient
    Int_t nb = 50; // number of color steps
    TColor::CreateGradientColorTable(Number, Length, Red, Green, Blue, nb);
    gStyle->SetNumberContours(nb); // apply gradient to histogram

    std::vector<double> *energy = nullptr;
    std::vector<int> *pid = nullptr;
    t->SetBranchAddress("energy", &energy);
    t->SetBranchAddress("pid", &pid);

    Long64_t N = t->GetEntries();
    for (Long64_t i = 0; i < N; i++) {
        t->GetEntry(i);
        double Ee = -1;
        double Eg = -1;
        // Get energies
        for (size_t j = 0; j < energy->size(); j++) {
            if (pid->at(j) == 1) Ee = energy->at(j); // electron
            if (pid->at(j) == 0) Eg = energy->at(j); // gamma
        }
        if (Ee > 0 && Eg > 0) // make sure both exist
            h2->Fill(Ee, Eg);
    }
    
    h2->SetStats(kFALSE);
    h2->Draw("COLZ");
    gPad->SetLogz();
    gPad->Update();
    c->SaveAs("/sps/nemo/scratch/elvin/graphics_data/Ee_Egam_2D.png");
    c->Update();
}
