// Last edited 03/12/2025
// Author: Anya Elvin
// Macro to draw the energy distribution of all particles in the unprocessed simulation file
// Energy range in the plot is zoomed in on 0<E<100kev
// This helps figure out where to cut at the lower boundary (cut 2)
// This macro saves the output as a png file at the moment (/sps/nemo/scratch/elvin/graphics/Ee_low_range.png)
// Would ideally like to save as a root, but it is complicated in ssh!

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <iostream>
#include <vector>

void Ee_low_range() {
    // Open the ROOT file
    TFile *f = TFile::Open("/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut4_root.root"); // simulation
    //TFile *f = TFile::Open("/sps/nemo/scratch/elvin/cut_data/OneByOne_cut4_1572_root.root"); // data 
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

    int nbins = 100;
    double max_energy = 0.5;
    TH1D *h_elec = new TH1D("h_elec", "Electron Energy;Energy [MeV];Counts", nbins, 0, max_energy);

    Long64_t nentries = t->GetEntries();
    std::cout << "Number of events: " << nentries << std::endl;
    for (Long64_t i = 0; i < nentries; ++i) {
        t->GetEntry(i);
        if (!pid || !energy) continue;
        for (size_t j = 0; j < pid->size(); ++j) {
            if (pid->at(j) == 1) { // electron
                double en = energy->at(j);
                if (en <= max_energy) {
                    h_elec->Fill(en);
                }
            }
        }
    }

    // Draw and save
    TCanvas *c = new TCanvas("c", "Electron Energy", 800, 600);
    h_elec->SetLineColor(kRed);
    h_elec->Draw();
    c->SetLogy(); 
    c->Update();
    c->SaveAs("/sps/nemo/scratch/elvin/graphics_simulation/Ee_low_range.png");
    f->Close();
}