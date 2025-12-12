// Last edited 03/12/2025
// Author: Anya Elvin
// Macro to draw two side my side histograms: one for electron energy and one for photon energy
// For each histogram, there are different distributions corresponding to the events remaining after layers of cuts
    // Cut 1: 1 electron only and 1 photon only and nothing else
    // Cut 2: electron and gamma OM number ≤ 519
    // Cut 3: Electron start vertices away from edges/foil
    // Cut 4: Timing cut: measured dt is within a threshold window of dt_exp
    // Cut 5: E_e > 50keV and E_gam > 50keV
    // Cut 6: E_e + E_gam ≤ 3.00keV
// This macro saves the output as a png file at the moment (/sps/nemo/scratch/elvin/graphics/BestCuts_1572_two_energy_histos.png)
// Would ideally like to save as a root, but it is complicated in ssh!

#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include <iostream>

void BestCuts_two_energy_histos() {
    // Open the ROOT file and get the tree
    TFile *f = TFile::Open("/sps/nemo/scratch/elvin/simulations/Bi214_wire_surface_50M.root"); 
    TFile *f_cut1  = TFile::Open("/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut1_root.root");
    TFile *f_cut2  = TFile::Open("/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut2_root.root");
    TFile *f_cut3  = TFile::Open("/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut3_root.root");
    TFile *f_cut4  = TFile::Open("/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut4_root.root");
    TFile *f_cut5  = TFile::Open("/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut5_root.root");
    TFile *f_cut6  = TFile::Open("/sps/nemo/scratch/elvin/cut_simulations/OneByOne_cut6_root.root");
    if (!f || f->IsZombie() || !f_cut1 || f_cut1->IsZombie() || !f_cut2 || f_cut2->IsZombie() || !f_cut3 || f_cut3->IsZombie() || !f_cut4 || f_cut4->IsZombie() || !f_cut5 || f_cut5->IsZombie() || !f_cut6 || f_cut6->IsZombie()) {
        std::cerr << "Error: could not open one or both files" << std::endl;
        return;
    }
    TTree *t = (TTree*)f->Get("Result_tree");
    TTree *t_cut1  = (TTree*)f_cut1->Get("Result_tree");
    TTree *t_cut2  = (TTree*)f_cut2->Get("Result_tree");
    TTree *t_cut3  = (TTree*)f_cut3->Get("Result_tree");
    TTree *t_cut4  = (TTree*)f_cut4->Get("Result_tree");
    TTree *t_cut5  = (TTree*)f_cut5->Get("Result_tree");
    TTree *t_cut6  = (TTree*)f_cut6->Get("Result_tree");
    if (!t || !t_cut1 || !t_cut2 || !t_cut3 || !t_cut4 || !t_cut5 || !t_cut6) {
        std::cerr << "Error: could not find 'Result_tree' in one of the files" << std::endl;
        return;
    }

    // Define histograms
    gStyle->SetOptStat(0);
    TH1F *h_elec = new TH1F("h_elec", "Electron Energy Distribution;Energy [MeV];Counts", 50, 0, 4.5);
    TH1F *h_gamma = new TH1F("h_gamma", "Gamma Energy Distribution;Energy [MeV];Counts", 50, 0, 4.5);
    TH1F *h_elec_cut1 = new TH1F("h_elec_cut1", "", 50, 0, 4.5);
    TH1F *h_gamma_cut1 = new TH1F("h_gamma_cut1", "", 50, 0, 4.5);
    TH1F *h_elec_cut2 = new TH1F("h_elec_cut2", "", 50, 0, 4.5);
    TH1F *h_gamma_cut2 = new TH1F("h_gamma_cut2", "", 50, 0, 4.5);
    TH1F *h_elec_cut3 = new TH1F("h_elec_cut3", "", 50, 0, 4.5);
    TH1F *h_gamma_cut3 = new TH1F("h_gamma_cut3", "", 50, 0, 4.5);
    TH1F *h_elec_cut4 = new TH1F("h_elec_cut4", "", 50, 0, 4.5);
    TH1F *h_gamma_cut4 = new TH1F("h_gamma_cut4", "", 50, 0, 4.5);
    TH1F *h_elec_cut5 = new TH1F("h_elec_cut5", "", 50, 0, 4.5);
    TH1F *h_gamma_cut5 = new TH1F("h_gamma_cut5", "", 50, 0, 4.5);
    TH1F *h_elec_cut6 = new TH1F("h_elec_cut6", "", 50, 0, 4.5);
    TH1F *h_gamma_cut6 = new TH1F("h_gamma_cut6", "", 50, 0, 4.5);

    // Histogram styling
    h_elec->SetLineColor(kBlack);
    h_elec->SetLineWidth(2);
    h_gamma->SetLineColor(kBlack);
    h_gamma->SetLineWidth(2);

    // Overlay histograms with cut_v1
    h_elec_cut1->SetLineColor(kBlue);
    h_elec_cut1->SetLineWidth(2);
    h_gamma_cut1->SetLineColor(kBlue);
    h_gamma_cut1->SetLineWidth(2);

    // Overlay histograms with cut_v2
    h_elec_cut2->SetLineColor(kGreen);
    h_elec_cut2->SetLineWidth(2);
    h_gamma_cut2->SetLineColor(kGreen);
    h_gamma_cut2->SetLineWidth(2);

    // Overlay histograms with cut_v3
    h_elec_cut3->SetLineColor(kRed);
    h_elec_cut3->SetLineWidth(2);
    h_gamma_cut3->SetLineColor(kRed);
    h_gamma_cut3->SetLineWidth(2);

    // Overlay histograms with cut_v4
    h_elec_cut4->SetLineColor(kViolet);
    h_elec_cut4->SetLineWidth(2);
    h_gamma_cut4->SetLineColor(kViolet);
    h_gamma_cut4->SetLineWidth(2);

    // Overlay histograms with cut_v5
    h_elec_cut5->SetLineColor(kOrange);
    h_elec_cut5->SetLineWidth(2);
    h_gamma_cut5->SetLineColor(kOrange);
    h_gamma_cut5->SetLineWidth(2);

    // Overlay histograms with cut_v6
    h_elec_cut6->SetLineColor(kPink);
    h_elec_cut6->SetLineWidth(2);
    h_gamma_cut6->SetLineColor(kPink);
    h_gamma_cut6->SetLineWidth(2);

    // Branches
    std::vector<int> *pid = nullptr;
    std::vector<double> *energy = nullptr;
    std::vector<int> *om_number = nullptr; 
    t->SetBranchAddress("pid", &pid);
    t->SetBranchAddress("energy", &energy);
    t->SetBranchAddress("om_number", &om_number);

    Long64_t n_full_elec = 0, n_full_gamma = 0;
    Long64_t n_cut1_elec = 0, n_cut1_gamma = 0;
    Long64_t n_cut2_elec = 0, n_cut2_gamma = 0;
    Long64_t n_cut3_elec = 0, n_cut3_gamma = 0;
    Long64_t n_cut4_elec = 0, n_cut4_gamma = 0;
    Long64_t n_cut5_elec = 0, n_cut5_gamma = 0;
    Long64_t n_cut6_elec = 0, n_cut6_gamma = 0;

    Long64_t nentries = t->GetEntries();

    // Original dataset histogram
    for (Long64_t i = 0; i < nentries; ++i) {
        t->GetEntry(i);
        for (size_t j = 0; j < pid->size(); ++j) {
            int this_pid = pid->at(j);
            double en = energy->at(j);
            if (this_pid == 1) {
                h_elec->Fill(en);  // electron
                n_full_elec++;
            }
            else if (this_pid == 0) {
                h_gamma->Fill(en); // gamma
                n_full_gamma++;
            }
        }
    }

    // Cut v1 dataset histogram
    if (t_cut1) {
        std::vector<int> *identity_cut = nullptr;
        std::vector<double> *energy_cut = nullptr;

        t_cut1->SetBranchAddress("pid", &identity_cut);
        t_cut1->SetBranchAddress("energy", &energy_cut);

        Long64_t nentries_cut = t_cut1->GetEntries();
        for (Long64_t i = 0; i < nentries_cut; ++i) {
            t_cut1->GetEntry(i);
            for (size_t j = 0; j < identity_cut->size(); ++j) {
                int this_pid = identity_cut->at(j);
                double en = energy_cut->at(j);
                if (this_pid == 1) {
                    h_elec_cut1->Fill(en);
                    n_cut1_elec++;
                }
                else if (this_pid == 0) {
                    h_gamma_cut1->Fill(en);
                    n_cut1_gamma++;
                }
            }
        }
    }

    // Cut v2 dataset histogram
    if (t_cut2) {
        std::vector<int> *identity_cut = nullptr;
        std::vector<double> *energy_cut = nullptr;

        t_cut2->SetBranchAddress("pid", &identity_cut);
        t_cut2->SetBranchAddress("energy", &energy_cut);

        Long64_t nentries_cut = t_cut2->GetEntries();
        for (Long64_t i = 0; i < nentries_cut; ++i) {
            t_cut2->GetEntry(i);
            for (size_t j = 0; j < identity_cut->size(); ++j) {
                int this_pid = identity_cut->at(j);
                double en = energy_cut->at(j);
                if (this_pid == 1) {
                    h_elec_cut2->Fill(en);
                    n_cut2_elec++;
                }
                else if (this_pid == 0) {
                    h_gamma_cut2->Fill(en);
                    n_cut2_gamma++;
                }
            }
        }
    }

    // Cut v3 dataset histogram
    if (t_cut3) {
        std::vector<int> *identity_cut = nullptr;
        std::vector<double> *energy_cut = nullptr;

        t_cut3->SetBranchAddress("pid", &identity_cut);
        t_cut3->SetBranchAddress("energy", &energy_cut);

        Long64_t nentries_cut = t_cut3->GetEntries();
        for (Long64_t i = 0; i < nentries_cut; ++i) {
            t_cut3->GetEntry(i);
            for (size_t j = 0; j < identity_cut->size(); ++j) {
                int this_pid = identity_cut->at(j);
                double en = energy_cut->at(j);
                if (this_pid == 1) {
                    h_elec_cut3->Fill(en);
                    n_cut3_elec++;
                }
                else if (this_pid == 0) {
                    h_gamma_cut3->Fill(en);
                    n_cut3_gamma++;
                }
            }
        }
    }

    // Cut v4 dataset histogram
    if (t_cut4) {
        std::vector<int> *identity_cut = nullptr;
        std::vector<double> *energy_cut = nullptr;

        t_cut4->SetBranchAddress("pid", &identity_cut);
        t_cut4->SetBranchAddress("energy", &energy_cut);

        Long64_t nentries_cut = t_cut4->GetEntries();
        for (Long64_t i = 0; i < nentries_cut; ++i) {
            t_cut4->GetEntry(i);
            for (size_t j = 0; j < identity_cut->size(); ++j) {
                int this_pid = identity_cut->at(j);
                double en = energy_cut->at(j);
                if (this_pid == 1) {
                    h_elec_cut4->Fill(en);
                    n_cut4_elec++;
                }
                else if (this_pid == 0) {
                    h_gamma_cut4->Fill(en);
                    n_cut4_gamma++;
                }
            }
        }
    }

    // Cut v5 dataset histogram
    if (t_cut5) {
        std::vector<int> *identity_cut = nullptr;
        std::vector<double> *energy_cut = nullptr;

        t_cut5->SetBranchAddress("pid", &identity_cut);
        t_cut5->SetBranchAddress("energy", &energy_cut);

        Long64_t nentries_cut = t_cut5->GetEntries();
        for (Long64_t i = 0; i < nentries_cut; ++i) {
            t_cut5->GetEntry(i);
            for (size_t j = 0; j < identity_cut->size(); ++j) {
                int this_pid = identity_cut->at(j);
                double en = energy_cut->at(j);
                if (this_pid == 1) {
                    h_elec_cut5->Fill(en);
                    n_cut5_elec++;
                }
                else if (this_pid == 0) {
                    h_gamma_cut5->Fill(en);
                    n_cut5_gamma++;
                }
            }
        }
    }

    // Cut v6 dataset histogram
    if (t_cut6) {
        std::vector<int> *identity_cut = nullptr;
        std::vector<double> *energy_cut = nullptr;

        t_cut6->SetBranchAddress("pid", &identity_cut);
        t_cut6->SetBranchAddress("energy", &energy_cut);

        Long64_t nentries_cut = t_cut6->GetEntries();
        for (Long64_t i = 0; i < nentries_cut; ++i) {
            t_cut6->GetEntry(i);
            for (size_t j = 0; j < identity_cut->size(); ++j) {
                int this_pid = identity_cut->at(j);
                double en = energy_cut->at(j);
                if (this_pid == 1) {
                    h_elec_cut6->Fill(en);
                    n_cut6_elec++;
                }
                else if (this_pid == 0) {
                    h_gamma_cut6->Fill(en);
                    n_cut6_gamma++;
                }
            }
        }
    }

    // Create a canvas with 2 pads
    TCanvas *c = new TCanvas("c", "Energy Distributions", 1200, 400);
    c->Divide(2, 1);

    // Pad 1: Electrons
    c->cd(1);
    gPad->SetLogy();
    h_elec->Draw("HIST");
    if (t_cut1) h_elec_cut1->Draw("HIST SAME");
    if (t_cut2) h_elec_cut2->Draw("HIST SAME");
    if (t_cut3) h_elec_cut3->Draw("HIST SAME");
    if (t_cut4) h_elec_cut4->Draw("HIST SAME");
    if (t_cut5) h_elec_cut5->Draw("HIST SAME");
    if (t_cut6) h_elec_cut6->Draw("HIST SAME");

    // Format legend
    TLegend *leg1 = new TLegend(0.50, 0.65, 0.90, 0.90);;
    std::string full_label = "Full dataset: " + std::to_string(n_full_elec) + " remaining";
    std::string cut1_label = "1e1#gamma only: " + std::to_string(n_cut1_elec) + " remaining";
    std::string cut2_label = "e and #gamma OM < 520: "  + std::to_string(n_cut2_elec) + " remaining";
    std::string cut3_label = "Edge vertices trimmed: "  + std::to_string(n_cut3_elec) + " remaining";
    std::string cut4_label = "Timing cuts: "  + std::to_string(n_cut4_elec) + " remaining";
    std::string cut5_label = "E > 50 keV: " + std::to_string(n_cut5_elec) + " remaining";
    std::string cut6_label = "ETot < 3.00 MeV: "  + std::to_string(n_cut6_elec) + " remaining";
    leg1->AddEntry(h_elec, full_label.c_str(), "l");
    leg1->AddEntry(h_elec_cut1, cut1_label.c_str(), "l");
    leg1->AddEntry(h_elec_cut2, cut2_label.c_str(), "l");
    leg1->AddEntry(h_elec_cut3, cut3_label.c_str(), "l");
    leg1->AddEntry(h_elec_cut4, cut4_label.c_str(), "l");
    leg1->AddEntry(h_elec_cut5, cut5_label.c_str(), "l");
    leg1->AddEntry(h_elec_cut6, cut6_label.c_str(), "l");
    leg1->Draw();

    // Pad 2: Gammas
    c->cd(2);
    gPad->SetLogy();
    h_gamma->Draw("HIST");
    if (t_cut1) h_gamma_cut1->Draw("HIST SAME");
    if (t_cut2) h_gamma_cut2->Draw("HIST SAME");
    if (t_cut3) h_gamma_cut3->Draw("HIST SAME");
    if (t_cut4) h_gamma_cut4->Draw("HIST SAME");
    if (t_cut5) h_gamma_cut5->Draw("HIST SAME");
    if (t_cut6) h_gamma_cut6->Draw("HIST SAME");

    // Format legend
    TLegend *leg2 = new TLegend(0.50, 0.65, 0.90, 0.90);
    std::string full_label_g = "Full dataset: " + std::to_string(n_full_gamma) + " remaining";
    std::string cut1_label_g = "1e1#gamma only: " + std::to_string(n_cut1_gamma) + " remaining";
    std::string cut2_label_g = "e and #gamma OM < 520: "  + std::to_string(n_cut2_gamma) + " remaining";
    std::string cut3_label_g = "Edge vertices trimmed: "  + std::to_string(n_cut3_gamma) + " remaining";
    std::string cut4_label_g = "Timing cuts: "  + std::to_string(n_cut4_gamma) + " remaining";
    std::string cut5_label_g = "E > 50 keV: " + std::to_string(n_cut5_gamma) + " remaining";
    std::string cut6_label_g = "ETot < 3.00 MeV: "  + std::to_string(n_cut6_gamma) + " remaining";
    leg2->AddEntry(h_gamma, full_label_g.c_str(), "l");
    leg2->AddEntry(h_gamma_cut1, cut1_label_g.c_str(), "l");
    leg2->AddEntry(h_gamma_cut2, cut2_label_g.c_str(), "l");
    leg2->AddEntry(h_gamma_cut3, cut3_label_g.c_str(), "l");
    leg2->AddEntry(h_gamma_cut4, cut4_label_g.c_str(), "l");
    leg2->AddEntry(h_gamma_cut5, cut5_label_g.c_str(), "l");
    leg2->AddEntry(h_gamma_cut6, cut6_label_g.c_str(), "l");
    leg2->Draw();

    c->SaveAs("/sps/nemo/scratch/elvin/graphics_simulation/BestCuts_two_energy_histos.png");
    //TFile *outFile = TFile::Open("/sps/nemo/scratch/elvin/graphics_data/BestCuts_two_energy_histos_V2.root", "RECREATE");
    std::cout << "Saved plot as graphics_simulation/BestCuts_two_energy_histos.png" << std::endl;
    //c->Write();
    c->Update();
    //outFile->Close();
}