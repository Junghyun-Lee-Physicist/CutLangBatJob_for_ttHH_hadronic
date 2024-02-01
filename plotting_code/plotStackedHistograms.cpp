#include <TFile.h>
#include <TH1.h>
#include <TKey.h>
#include <TDirectory.h>
#include <TCanvas.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

struct HistogramSetting {
    string typeOfHisto;
    string title;
};

void DrawHistogram(TH1* hist, const HistogramSetting& setting) {

    if (setting.typeOfHisto == "cutflow") {
        TCanvas* canvas = new TCanvas("canvas", setting.title.c_str(), 800, 400); 
        hist->Draw("hist");
        string saveName = setting.title + ".png";
        canvas->SaveAs(saveName.c_str());
        delete canvas;
    } else {
        TCanvas* canvas = new TCanvas("canvas", setting.title.c_str(), 600, 600);
        hist->Draw("hist");
        string saveName = setting.title + ".png";
        canvas->SaveAs(saveName.c_str());
        delete canvas;
    }
}

void Iteration_Directories_And_Histograms(const vector<string>& processes, const unordered_map<string, HistogramSetting>& histogramSettings) {

    if (processes.empty()) {
        cout << "[Plotter Error]: There are no processes. Please check the processes list." << endl;
        return;
    }

    for (const auto& process : processes) {
        TFile* file = TFile::Open(process.c_str(), "READ");

        TIter nextDir(file->GetListOfKeys());
        TKey* dirKey;
        while ((dirKey = (TKey*)nextDir())) {
            if (dirKey->IsFolder()) {
                TDirectory* dir = (TDirectory*)dirKey->ReadObj();
                string dirName = dir->GetName();

                auto it = histogramSettings.find(dirName);
                if (it == histogramSettings.end()) {
                    cout << "[Plotter Log]: No " << dirName << " in " << process << "'s root file, Skipping this" << endl;
                    continue;
                }
                cout << "[Plotter Log]: " << dirName << " exists at " << process << "'s root file." << endl;

                TIter nextHist(dir->GetListOfKeys());
                TKey* histKey;
                while ((histKey = (TKey*)nextHist())) {

                    if (strcmp(histKey->GetClassName(), "TH1D") == 0) {
                        TH1* hist = (TH1*)histKey->ReadObj();
                        hist->SetDirectory(0); // For the memory

                        DrawHistogram(hist, it->second);

                    }

                }// End of the interation for histograms
            }
        }// End of the interation for TDirectories

        file->Close();
    }// End of the iteration for processes
}

int main() {
    vector<string> processes = {"ttHH.root", "QCD.root"};
    unordered_map<string, HistogramSetting> histogramSettings = {
        {"presel", {"cutflow", "cutflow of preselection"}},
        {"baselineForHTHisto", {"cutflow", "cutflow of baselineForHT"}}
    };

    Iteration_Directories_And_Histograms(processes, histogramSettings);

    return 0;
}
