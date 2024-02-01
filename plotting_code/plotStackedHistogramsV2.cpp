#include <TFile.h>
#include <TH1.h>
#include <THStack.h>
#include <TKey.h>
#include <TDirectory.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TText.h>
#include <TStyle.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>

using namespace std;

struct HistogramSetting {
    string typeOfHisto;
    string title;
    string xAxisTitle;
};

struct ProcessInfo{
    bool isSignal;
    int color; // ROOT color code
}

const string plotExtension = ".png"; // save file extension



void DrawStackedHistograms(const vector<TH1*>& histograms, const string& channelName, const HistogramSetting& setting) {

    if(setting.typeOfHisto == "cutflow"){

        THStack* stack = new THStack("stack", setting.title.c_str());
        TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);

        for (auto hist : histograms) {
            stack->Add(hist);
            legend->AddEntry(hist, hist->GetName(), "l");
        }

        TCanvas* canvas = new TCanvas("canvas", setting.title.c_str(), 800, 400);
        stack->Draw("HIST");
        legend->Draw();

        string plotExtension = ".png";
        //string saveName = channelName + "_" + setting.title + plotExtension;
	string saveName = channelName + "_" + histograms[0]->GetName() + plotExtension;
        canvas->SaveAs(saveName.c_str());
    
        delete stack;
        delete legend;
        delete canvas;
    }
    else {

        THStack* stack = new THStack("stack", setting.title.c_str());
        TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);

        for (auto hist : histograms) {
            stack->Add(hist);
            legend->AddEntry(hist, hist->GetName(), "l");
        }

        TCanvas* canvas = new TCanvas("canvas", setting.title.c_str(), 800, 600);
        stack->Draw("HIST");
        legend->Draw();

        string plotExtension = ".png";
        //string saveName = channelName + "_" + setting.title + plotExtension;
	string saveName = channelName + "_" + histograms[0]->GetName() + plotExtension;
        canvas->SaveAs(saveName.c_str());
    
        delete stack;
        delete legend;
        delete canvas;
    }


}


void DrawHistogram(TH1* hist, const string& channelName, const HistogramSetting& setting) {

    if (setting.typeOfHisto == "cutflow") {
        TCanvas* canvas = new TCanvas("canvas", setting.title.c_str(), 800, 400);
        hist->Draw("hist");
        //string saveName = channelName + "_" + setting.title + plotExtension;
	string saveName = channelName + "_" + hist->GetName() + plotExtension;
        canvas->SaveAs(saveName.c_str());
        delete canvas;
    } else {
        TCanvas* canvas = new TCanvas("canvas", setting.title.c_str(), 600, 600);
        hist->Draw("hist");
        //string saveName = channelName + "_" + setting.title + plotExtension;
        string saveName = channelName + "_" + hist->GetName() + plotExtension;
        canvas->SaveAs(saveName.c_str());
        delete canvas;
    }

}


void Iteration_Directories_And_Histograms(const vector<string>& processes, const unordered_map<string, vector<HistogramSetting>>& histogramSettings) {

    if (processes.empty()) {
        cout << "[Plotter Error]: There are no processes. Please check the processes list." << endl;
        return;
    }

    for (const auto& process : processes) {
    // Iteration for ttHH, ttZH, QCD, ..., etc

        TFile* file = TFile::Open(process.c_str(), "READ");

        for (const auto& settingPair : histogramSettings) {
            const string& channelName = settingPair.first;
            const vector<HistogramSetting>& settings = settingPair.second;

            TDirectory* dir = nullptr;
            file->GetObject(channelName.c_str(), dir);
            if (!dir) {
                cout << "[Plotter Log]: No " << channelName << " in " << process << "'s root file, Skipping this" << endl;
                continue;
            }
            cout << "[Plotter Log]: " << channelName << " exists at " << process << "'s root file." << endl;


            vector<TH1*> histograms;
            TIter nextHist(dir->GetListOfKeys());
            TKey* histKey;
            while ((histKey = (TKey*)nextHist())) {

                if (strcmp(histKey->GetClassName(), "TH1D") == 0) {
                    TH1* hist = (TH1*)histKey->ReadObj();
                    hist->SetDirectory(0); // For the memory management

                    histograms.push_back(hist); // For the stack histogram
                }
                // If there are addtional type of histogram.. Please fullfill below code
                // else if ( ... ){
                // ...
                // }

            }

            for (const auto& setting : settings) {

                if(!histograms.empty()){

                    for (auto hist : histograms) {
                        DrawHistogram(hist, channelName, setting);
                    }
                  
                    DrawStackedHistograms(histograms, channelName, setting);
              
                    for (auto hist : histograms) {
                        delete hist;
                    }
                }
            }

        }// End of the channel's interation

        file->Close();

    } // End of the processes' interation

} // End of the function


int main() {

    vector<string> processes = {"ttHH.root", "QCD.root"};

    unordered_map<string, vector<HistogramSetting>> histogramSettings = {

        {"presel", 
	    {
                {"cutflow", "cutflow of pre-selection"}
	    }
	},
        {"baselineForHTHisto", 
            {
                {"jetHTbeforeHTCut", "jet HT [before HT Cut]"},
                {"jetHTafterHTCut", "jet HT [before HT Cut]"},
                {"cutflow", "cutflow of baseline for HT histo"}
            }
	},
        {"baselineForWHisto", 
            {
                {"cutflow", "cutflow of baseline for W histo"}
            }
	},
        {"baselineHistos", 
            {
                {"histo_njets"    , "number of jets"},
                {"histo_nbjets"   , "number of b-jets"},
                {"histo_nljets"   , "number of light-jets"},
                {"hjet01pt"       , "1st jet P_{t}"},
                {"hjet02pt"       , "2nd jet P_{t}"},
                {"hjet03pt"       , "3rd jet P_{t}"},
                {"hjet04pt"       , "4th jet P_{t}"},
                {"hjet05pt"       , "5th jet P_{t}"},
                {"hjet06pt"       , "6th jet P_{t}"},
                {"hjet01eta"      , "1st jet #eta"},
                {"hjet02eta"      , "2nd jet #eta"},
                {"hjet03eta"      , "3rd jet #eta"},
                {"hjet04eta"      , "4th jet #eta"},
                {"hjet05eta"      , "5th jet #eta"},
                {"hjet06eta"      , "6th jet #eta"},
                {"jetHT"          , "jets HT"},
                {"jetBHT"         , "b-jets HT"},
                {"cutflow"        , "cutflow of baseline selection"}
            }
	},
        {"channelChi2", 
            {
                {"cutflow"        , "cutflow of baseline for W histo"},
                {"histo_njets"    , "number of jets"},
                {"histo_nbjets"   , "number of b-jets"},
                {"histo_nljets"   , "number of light-jets"},
                {"hjet01pt"       , "1st jet P_{t}"},
                {"hjet02pt"       , "2nd jet P_{t}"},
                {"hjet03pt"       , "3rd jet P_{t}"},
                {"hjet04pt"       , "4th jet P_{t}"},
                {"hjet05pt"       , "5th jet P_{t}"},
                {"hjet06pt"       , "6th jet P_{t}"},
                {"hjet01eta"      , "1st jet #eta"},
                {"hjet02eta"      , "2nd jet #eta"},
                {"hjet03eta"      , "3rd jet #eta"},
                {"hjet04eta"      , "4th jet #eta"},
                {"hjet05eta"      , "5th jet #eta"},
                {"hjet06eta"      , "6th jet #eta"},
                {"jetHT"          , "jets HT"},
                {"jetBHT"         , "b-jets HT"},
                {"hchi2HH"        , "Reconstructed Higgs pair #chi^{2}"},
                {"hmH1cnd"        , "mass of 1st higgs candidate"},
                {"hmH2cnd"        , "mass of 2nd higgs candidate"},
                {"cutflow"        , "cutflow with Higgs pair reconstruction"}
            }
	}

    };

    Iteration_Directories_And_Histograms(processes, histogramSettings);

    return 0;
}
