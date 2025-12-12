// Last edited 07/12/2025
// Author: Anya Elvin
// Macro to draw a histogram showing the x coordinates of the start vertices of the electrons
// This helps figure out how big my margins should be around the edge of the detector
// This macro saves the output as a png file at the moment (graphics/x_start_end_vertices.png)
// Would ideally like to save as a root, but it is complicated in ssh!
// Change the input file to be the simulation whenI have it

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <iostream>
#include <vector>

void y_start_end_vertices() {
    // Open the ROOT file and get the tree
    //TFile *f = TFile::Open("/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut2_root.root"); // simulation
    TFile *f = TFile::Open("/sps/nemo/scratch/elvin/cut_data/OneByOne_cut2_1572_root.root"); // data 
    if (!f || f->IsZombie()) { 
        std::cerr << "File error\n"; 
        return; 
    }
    TTree *t = (TTree*)f->Get("Result_tree");
    if (!t) { 
        std::cerr << "Tree error\n"; 
        return; 
    }

    // Set up branches
    std::vector<double> *first_vertex_y = nullptr;
    std::vector<double> *second_vertex_y = nullptr;
    t->SetBranchAddress("first_vertex_y", &first_vertex_y);
    t->SetBranchAddress("second_vertex_y", &second_vertex_y);

    // Histogram ranges matching detector geometry
    TH1D *h_startY = new TH1D("h_startY", "Start Vertex Y;Y [mm];Counts", 2500, -2500, 2500);
    TH1D *h_endY   = new TH1D("h_endY", "End Vertex Y;Y [mm];Counts", 2500, -2500, 2500);

    Long64_t N = t->GetEntries();
    std::cout << "Number of events = " << N << std::endl;
    for (Long64_t i = 0; i < N; i++) {
        t->GetEntry(i);
        if (first_vertex_y) {
            for (double y : *first_vertex_y) h_startY->Fill(y);
        }
        if (second_vertex_y) {
            for (double y : *second_vertex_y) h_endY->Fill(y);
        }
    }

    TCanvas *c = new TCanvas("cY", "Y Vertex Distributions", 1800, 600);
    c->Divide(2, 1);

    c->cd(1);
    h_startY->SetLineColor(kBlue);
    h_startY->SetStats(false);
    h_startY->Draw();

    c->cd(2);
    h_endY->SetLineColor(kRed);
    h_endY->SetStats(false);
    h_endY->Draw();

    c->SaveAs("/sps/nemo/scratch/elvin/graphics_data/y_start_end_vertices.png");
    f->Close();
}