// Last edited 03/12/2025
// Author: Anya Elvin
// Macro to draw a 2D histogram of the number of electrons vs the number of gammas for the full simulation
// This helps figure out how many electrons and gammas should be used in the cut
// This macro saves the output as a png file at the moment (graphics/Ne_Ngam_2D.png)
// Would ideally like to save as a root, but it is complicated in ssh!

#include <vector>
#include <string>
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include <iostream>
#include "TPaveStats.h"

void Ne_Ngam_2D() {
    // Open the ROOT file and get the tree
    //TFile *f = TFile::Open("/sps/nemo/scratch/elvin/simulations/Bi214_wire_surface_50M.root"); // simulation
    TFile *f = TFile::Open("/sps/nemo/scratch/elvin/data/run_1572.root"); // data 
    if (!f || f->IsZombie()) {
        std::cerr << "Error: could not open file" << std::endl;
        return;
    }
    TTree *t = (TTree*)f->Get("Result_tree"); 
    if (!t) {
        std::cerr << "Error: could not find tree" << std::endl;
        return;
    }

    // Create canvas
    TCanvas *c = new TCanvas("c", "2D Histogram", 800, 600);

    // Create 2D histogram: 20 bins for gammas and 10 bins for electrons
    TH2F *h2 = new TH2F("h2", "Electrons vs Gammas;# Electrons;# Gammas", 10, 0, 10, 10, 0, 10);

    // Create gradient red color palette
    const Int_t Number = 2; // two anchors
    Double_t Red[Number] = {1.00, 0.50}; // dark red to pale pink
    Double_t Green[Number] = {0.80, 0.00}; // small amount in order for pink to show
    Double_t Blue[Number] = {0.80, 0.00}; // small amount in order for prink to show
    Double_t Length[Number] = {0.00, 1.00}; // positions along gradient
    Int_t nb = 50; // number of color steps
    TColor::CreateGradientColorTable(Number, Length, Red, Green, Blue, nb);
    gStyle->SetNumberContours(nb); // apply gradient to histogram

    // Fill histogram
    t->Draw("gamma_number:electron_number >> h2", "", "COLZ");
    gPad->SetLogz();

    // ATTEMPTS TO MOVE THE STATS BOX
    gPad->Update();  // necessary to ensure the stats box exists
    TPaveStats *st = (TPaveStats*)h2->GetListOfFunctions()->FindObject("stats");
    if (st) {
        st->SetX1NDC(0.7); // left side of box (0–1 in NDC)
        st->SetX2NDC(0.9); // right side
        st->SetY1NDC(0.7); // bottom
        st->SetY2NDC(0.9); // top
    }

    gPad->Update();
    h2->Draw("COLZ SAME");
    c->SaveAs("/sps/nemo/scratch/elvin/graphics_data/Ne_Ngam_2D.png");
    c->Update();
}