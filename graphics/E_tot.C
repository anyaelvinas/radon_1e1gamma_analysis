// Last edited 07/12/2025
// Author: Anya Elvin
// Macro to draw the energy distribution of summed electron and photon energies
// This helps check whether the total energy of events is adding up to the expected distribution
// This macro saves the output as a png file at the moment (/sps/nemo/scratch/elvin/graphics/Ee_low_range.png)
// Would ideally like to save as a root, but it is complicated in ssh!

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <iostream>
#include <vector>

void E_tot() {
    // Open the ROOT file
    //TFile *f = TFile::Open("/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut5_root.root"); // simulation
    TFile *f = TFile::Open("/sps/nemo/scratch/elvin/cut_data/OneByOne_cut5_1572_root.root"); // data 
    if (!f || f->IsZombie()) {
        std::cerr << "Error: could not open file" << std::endl;
        return;
    }
    TTree *t = (TTree*)f->Get("Result_tree");
    if (!t) {
        std::cerr << "Error: could not find tree 'Result_tree'" << std::endl;
        return;
    }

    // Sort branches
    std::vector<int> *pid = nullptr; 
    std::vector<double> *energy = nullptr;
    t->SetBranchAddress("pid", &pid);
    t->SetBranchAddress("energy", &energy);

    int nbins = 35;
    double max_energy = 3.5;
    TH1D *h_tot = new TH1D("h_tot", "Total Electron + Photon Energy;E_{tot} [MeV];Counts", nbins, 0, max_energy);

    Long64_t nentries = t->GetEntries();
    std::cout << "Number of events: " << nentries << std::endl;

    for (Long64_t i = 0; i < nentries; ++i) {
        t->GetEntry(i);
        if (!pid || !energy) continue;
        double Ee = -1;
        double Eg = -1;
        for (size_t j = 0; j < pid->size(); ++j) {
            if (pid->at(j) == 1) Ee = energy->at(j);
            if (pid->at(j) == 0) Eg = energy->at(j);
        }
        if (Ee > 0 && Eg > 0) {
            double E_tot = Ee + Eg;
            h_tot->Fill(E_tot);
        }
    }

    // Draw and save
    TCanvas *c = new TCanvas("c", "Total Energy", 800, 600);
    h_tot->SetLineColor(kBlue);
    h_tot->Draw();
    c->SetLogy(); 
    c->Update();
    c->SaveAs("/sps/nemo/scratch/elvin/graphics_data/E_tot.png");
    f->Close();
}