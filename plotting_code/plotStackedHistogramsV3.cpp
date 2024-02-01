#include <TFile.h>
#include <TH1.h>
#include <THStack.h>
#include <TKey.h>
#include <TDirectory.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

struct ProcessInfo {
    string path;
    bool isSignal;
    int color; // stacked histogram color
    string legendName;
};

struct HistogramSetting {
    string typeOfHisto;
    string title;
    string xAxisTitle;
};

const string plotExtension = ".png"; // save file extension


void DrawStackedHistograms(const vector<pair<TH1*, ProcessInfo>>& histograms, const HistogramSetting& setting, const string& histName) {
 
   double maxY = 0;
    THStack* stack = new THStack("stack", "");
    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);

    // For background processes' histogram
    for (const auto& histPair : histograms) {
        if (!histPair.second.isSignal) {
            TH1* hist = histPair.first;
            maxY = max(maxY, hist->GetMaximum());
            hist->SetFillColor(histPair.second.color);
            stack->Add(hist);
            legend->AddEntry(hist, histPair.second.legendName.c_str(), "l");
        }
    }

    // For signal process's histogram
    TH1* signalHist = nullptr;
    for (const auto& histPair : histograms) {
        if (histPair.second.isSignal) {
            signalHist = histPair.first;
	    signalHist->SetLineWidth(3);
            signalHist->SetFillColor(histPair.second.color);
            legend->AddEntry(signalHist, histPair.second.legendName.c_str(), "l");
            break;
        }
    }

    if (signalHist) {
        maxY = max(maxY, signalHist->GetMaximum());
    }

    TCanvas* canvas = nullptr;
    if (setting.typeOfHisto == "cutflow") {
        canvas = new TCanvas("canvas", setting.title.c_str(), 1000, 700);
    }
    else{
        canvas = new TCanvas("canvas", setting.title.c_str(), 650, 700);
    }

    stack->Draw("HIST");
    stack->SetMinimum(1e-2);
    stack->SetMaximum(maxY * 1.2); 
    stack->SetTitle(setting.title.c_str());

    signalHist->Draw("HISTsame");

    canvas->SetLogy(1);

    legend->Draw("same");
    canvas->Update();


    string saveName = histName + "_" + setting.title + plotExtension;
    canvas->SaveAs(saveName.c_str());

    delete stack;
    delete legend;
    delete canvas;
}

void Iteration_Directories_And_Histograms(const unordered_map<string, ProcessInfo>& processes, const unordered_map<string, vector<HistogramSetting>>& histogramSettings) {

    for (const auto& histSettingPair : histogramSettings) {

        const string& channelName = histSettingPair.first;
        const vector<HistogramSetting>& settings = histSettingPair.second;
        // ex)         
        // ....
        // {"baselineForHTHisto",
        //     {
        //         {"jetHTbeforeHTCut", "jet HT [before HT Cut]", "[GeV]"},
        //         {"jetHTafterHTCut", "jet HT [before HT Cut]", "[GeV]"},
        //         {"cutflow", "cutflow of baseline for HT histo", "cuts"}
        //     }
        // },
        // ....
        //
        // --> channelName = baselineForHTHisto
        // --> setting = {"jetHTbeforeHTCut", "jet HT [before HT Cut]", "[GeV]"},
        //               {"jetHTafterHTCut", "jet HT [before HT Cut]", "[GeV]"},
        //               {"cutflow", "cutflow of baseline for HT histo", "cuts"}
                         

        for(const auto& setting : settings) {
    
            vector<pair<TH1*, ProcessInfo>> histogramsForStacking;
    
            for (const auto& processPair : processes) {
                const string& processName = processPair.first;
                const ProcessInfo& processInfo = processPair.second;
    
                TFile* file = TFile::Open(processInfo.path.c_str(), "READ");
                if (!file || file->IsZombie()) {
                    cerr << "Cannot open file: " << processInfo.path << endl;
                    continue;
                }
    
                TDirectory* dir = nullptr;
                file->GetObject(channelName.c_str(), dir);
                if (!dir) {
                    cerr << "Directory not found: " << channelName << " in " << processInfo.path << endl;
                    file->Close();
                    continue;
                }
    
                TH1* hist = nullptr;
                dir->GetObject(setting.first.c_str(), hist);
                if (hist) {

                   TKey* key = dir->GetKey(setting.typeOfHisto.c_str());
                    if (key && strcmp(key->GetClassName(), "TH1D") == 0) {
                        TH1* hist = static_cast<TH1*>(key->ReadObj());
                        hist->SetDirectory(0); // Detach the histogram from the file
                        histogramsForStacking.push_back(make_pair(hist, processInfo));
		    }
                }
    
                file->Close();
            }
    
            DrawStackedHistograms(histogramsForStacking, setting, channelName);
    
            // Clean up
            for (auto& histPair : histogramsForStacking) {
                delete histPair.first;
            }

        } // End of histograms iteration
    } // End of hisogramSettings interation (= interation of channels

} // End of the function


int main() {

    unordered_map<string, ProcessInfo> processes = {
        {"ttHH", {"ttHH.root", true, kRed, "ttHH"}},
        {"QCD", {"QCD.root", false, kBlue, "QCD"}}
    };

    unordered_map<string, vector<HistogramSetting>> histogramSettings = {

        {"presel", 
	    {
                {"cutflow", "cutflow of pre-selection", "cuts"}
	    }
	},
        {"baselineForHTHisto", 
            {
                {"jetHTbeforeHTCut", "jet HT [before HT Cut]", "[GeV]"},
                {"jetHTafterHTCut", "jet HT [before HT Cut]", "[GeV]"},
                {"cutflow", "cutflow of baseline for HT histo", "cuts"}
            }
	},
        {"baselineForWHisto", 
            {
                {"cutflow", "cutflow of baseline for W histo", "cuts"}
            }
	},
        {"baselineHistos", 
            {
                {"histo_njets"    , "number of jets" , ""},
                {"histo_nbjets"   , "number of b-jets", ""},
                {"histo_nljets"   , "number of light-jets", ""},
                {"hjet01pt"       , "1st jet P_{t}", "[GeV]"},
                {"hjet02pt"       , "2nd jet P_{t}", "[GeV]"},
                {"hjet03pt"       , "3rd jet P_{t}", "[GeV]"},
                {"hjet04pt"       , "4th jet P_{t}", "[GeV]"},
                {"hjet05pt"       , "5th jet P_{t}", "[GeV]"},
                {"hjet06pt"       , "6th jet P_{t}", "[GeV]"},
                {"hjet01eta"      , "1st jet #eta", "[#eta]"},
                {"hjet02eta"      , "2nd jet #eta", "[#eta]"},
                {"hjet03eta"      , "3rd jet #eta", "[#eta]"},
                {"hjet04eta"      , "4th jet #eta", "[#eta]"},
                {"hjet05eta"      , "5th jet #eta", "[#eta]"},
                {"hjet06eta"      , "6th jet #eta", "[#eta]"},
                {"jetHT"          , "jets HT", "[GeV]"},
                {"jetBHT"         , "b-jets HT", "[GeV]"},
                {"cutflow"        , "cutflow of baseline selection", "cuts"}
            }
	},
        {"channelChi2", 
            {
                {"histo_njets"    , "number of jets" , ""},
                {"histo_nbjets"   , "number of b-jets", ""},
                {"histo_nljets"   , "number of light-jets", ""},
                {"hjet01pt"       , "1st jet P_{t}", "[GeV]"},
                {"hjet02pt"       , "2nd jet P_{t}", "[GeV]"},
                {"hjet03pt"       , "3rd jet P_{t}", "[GeV]"},
                {"hjet04pt"       , "4th jet P_{t}", "[GeV]"},
                {"hjet05pt"       , "5th jet P_{t}", "[GeV]"},
                {"hjet06pt"       , "6th jet P_{t}", "[GeV]"},
                {"hjet01eta"      , "1st jet #eta", "[#eta]"},
                {"hjet02eta"      , "2nd jet #eta", "[#eta]"},
                {"hjet03eta"      , "3rd jet #eta", "[#eta]"},
                {"hjet04eta"      , "4th jet #eta", "[#eta]"},
                {"hjet05eta"      , "5th jet #eta", "[#eta]"},
                {"hjet06eta"      , "6th jet #eta", "[#eta]"},
                {"jetHT"          , "jets HT", "[GeV]"},
                {"jetBHT"         , "b-jets HT", "[GeV]"},
                {"hchi2HH"        , "Reconstructed Higgs pair #chi^{2}", "#chi^{2}"},
                {"hmH1cnd"        , "mass of 1st higgs candidate", "[GeV]"},
                {"hmH2cnd"        , "mass of 2nd higgs candidate", "[GeV]"},
                {"cutflow"        , "cutflow with Higgs pair reconstruction", "cuts"}
            }
	}

    };

    Iteration_Directories_And_Histograms(processes, histogramSettings);

    return 0;
}
