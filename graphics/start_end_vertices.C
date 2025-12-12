// Last edited 09/12/2025
// Author: Anya Elvin
// Macro to draw a 2D histogram showing the x and y coordinates of the start vertices of the electrons
// This helps figure out how big my margins should be around the edge of the detector
// This macro saves the output as a png file at the moment (graphics/start_vertices.png)
// Would ideally like to save as a root, but it is complicated in ssh!
// Change the input file to be the simulation whenI have it

#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TMath.h>
#include <iostream>
#include <vector>

void start_end_vertices() {
    // Open the ROOT file and get the tree
    //TFile *f = TFile::Open("/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut2_root.root"); // simulation
    TFile *f = TFile::Open("/sps/nemo/scratch/elvin/cut_data/OneByOne_cut2_1572_root.root"); // data 
    if (!f || f->IsZombie()) {
        std::cerr << "Error: could not open file" << std::endl;
        return;
    }
    TTree *t = (TTree*)f->Get("Result_tree"); 
    if (!t) {
        std::cerr << "Error: could not find tree" << std::endl;
        return;
    }

    // Set up branches
    std::vector<double>* first_vertex_x = nullptr;
    std::vector<double>* first_vertex_y = nullptr;
    std::vector<double> *second_vertex_x = nullptr;
    std::vector<double> *second_vertex_y = nullptr;
    t->SetBranchAddress("first_vertex_x", &first_vertex_x);
    t->SetBranchAddress("first_vertex_y", &first_vertex_y);
    t->SetBranchAddress("second_vertex_x", &second_vertex_x);
    t->SetBranchAddress("second_vertex_y", &second_vertex_y);

    // Binsize and detector size
    int N_bins_x = 50; 
    int N_bins_y = 200;

    double min_x = 1e6, max_x = -1e6;
    double min_y = 1e6, max_y = -1e6;
    // Start vertices histo
    TH2D *h_start = new TH2D("h_start", "Election Start Vertices;Y [mm];X [mm]", N_bins_y, -2600, 2600, N_bins_x, -450, 450);
    // End vertices histo
    TH2D *h_end   = new TH2D("h_end", "Electron End Vertices;Y [mm];X [mm]", N_bins_y, -2600, 2600, N_bins_x, -450, 450);

    Long64_t N = t->GetEntries();
    std::cout << "Number of events = " << N << std::endl;
    for (Long64_t i = 0; i < N; i++) {
        t->GetEntry(i);
        // first vertex
        if (!first_vertex_x || !first_vertex_y) continue;
        for (size_t j = 0; j < first_vertex_x->size(); j++) {
            // get vertex
            double vx = first_vertex_x->at(j);
            double vy = first_vertex_y->at(j);
            h_start->Fill(vy, vx);
            // check to see if it was maximum or minimum
            if (vx < min_x) min_x = vx;
            if (vx > max_x) max_x = vx;
            if (vy < min_y) min_y = vy;
            if (vy > max_y) max_y = vy;
        }
        // second vertex
        if (!second_vertex_x || !second_vertex_y) continue;
        for (size_t j = 0; j < second_vertex_x->size(); j++) {
            // get vertex
            double vx = second_vertex_x->at(j);
            double vy = second_vertex_y->at(j);
            h_end->Fill(vy, vx);
            // check to see if it was maximum or minimum
            if (vx < min_x) min_x = vx;
            if (vx > max_x) max_x = vx;
            if (vy < min_y) min_y = vy;
            if (vy > max_y) max_y = vy;
        }
    }

    std::cout << "Max x = " << max_x << std::endl;
    std::cout << "Min x = " << min_x << std::endl;
    std::cout << "Max y = " << max_y << std::endl;
    std::cout << "Min y = " << min_y << std::endl;

    TCanvas *c = new TCanvas("c", "Start and End Vertices Map", 1800, 600);
    c->Divide(2, 1);
    c->cd(1);
    gPad->SetRightMargin(0.10);
    h_start->SetStats(false);
    h_start->Draw("COLZ");

    c->cd(2);
    gPad->SetRightMargin(0.10);
    h_end->SetStats(false);
    h_end->Draw("COLZ");

    c->SaveAs("/sps/nemo/scratch/elvin/graphics_data/start_end_vertices.png");
    f->Close();
}