///clude <TFile.h>
#include <TH1.h>
#include <THStack.h>
#include <TKey.h>
#include <TDirectory.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TEfficiency.h>
#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <algorithm>

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
    string saveName;
};

const string plotExtension = ".png"; // save file extension
const string savePath = "plotOutput";

void setLegendSetting(TLegend* histoLegend){

    histoLegend->SetNColumns(4);
    histoLegend->SetFillStyle(0000);
    histoLegend->SetBorderSize(0);
    histoLegend->SetShadowColor(1);
    histoLegend->SetTextFont(62);

}

void setLatexSetting(TLatex histoLatex){
 
    // Adding CMS private work text
    histoLatex.SetNDC();
    histoLatex.SetTextSize(0.04);
    histoLatex.SetTextAlign(11);
    histoLatex.DrawLatex(0.1, 0.94, "CMS #scale[0.5]{#font[52]{Private Work}}");

    // Adding the year and luminosity
    histoLatex.SetTextSize(0.04);
    histoLatex.SetTextAlign(31); // Align to the right
    histoLatex.DrawLatex(0.9, 0.94, "41.5 fb^{-1} [13 TeV]");

}

TGraphAsymmErrors* CreateRatioPlot(TH1* signal, THStack* background){

    TGraphAsymmErrors* grRatio = new TGraphAsymmErrors();

    // Calculate sum of background THStack
    TH1D* sumHist = static_cast<TH1D*>(signal->Clone("sumHist"));
    sumHist->Reset();
    TList* histList = background->GetHists();
    TIter next(histList);
    TH1D* hist;
    while((hist = static_cast<TH1D*>(next()))){
	sumHist->Add(hist);
    }

    // ratio histogram and it's error
    TH1D* ratioHist = static_cast<TH1D*>(signal->Clone("ratioHIst"));
    ratioHist->Reset();
    for(int i = 1; i <= signal->GetNbinsX(); ++i){
	double S = signal->GetBinContent(i);
	double B = sumHist->GetBinContent(i);
	double sigma_S = signal->GetBinError(i);
	double sigma_B = sumHist->GetBinError(i);

	// the x value is the center value of the bin
	double x = signal->GetBinCenter(i);

	if(B > 0) {
	    // Calculate Ratio R & it's errors
	    double R = S / B;
	    double errorLow = R * sqrt(pow(sigma_S/S, 2) + pow(sigma_B/B, 2));
	    double errorHigh = errorLow;

	    int iPoint = grRatio->GetN();
	    grRatio->SetPoint(iPoint, x, R);
	    grRatio->SetPointError(iPoint, 0.0, 0.0, errorLow, errorHigh);
	}

    } // End of signal iteration

    return grRatio;

}



void DrawStackedHistograms(const vector<pair<TH1*, ProcessInfo>>& histograms, const HistogramSetting& setting, const string& histName) {
 
    double maxY = 0;
    THStack* stack = new THStack("stack", "");
    TLegend* legend = new TLegend(0.5, 0.7, 0.99, 0.99);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
// For background processes' histogram

    // Store only the background histograms in a separate vector
    vector<pair<TH1*, ProcessInfo>> bgHistograms;
    for (const auto& histPair : histograms) {
        if (!histPair.second.isSignal) {
            bgHistograms.push_back(histPair);
        }
    }

    // Sort the background histograms in ascending order based on the integral
    std::sort(bgHistograms.begin(), bgHistograms.end(),
        [](const pair<TH1*, ProcessInfo>& a, const pair<TH1*, ProcessInfo>& b) {
            return a.first->Integral() < b.first->Integral(); // Ascending sort, biggest one is on the top
            //return a.first->Integral() > b.first->Integral(); // Descending sort, biggest one is on the bottom

        }
    );

    for (const auto& histPair : bgHistograms) {
        TH1* hist = histPair.first;

        // Modify backgrounds cutflow histograms
	if(std::string(hist->GetName()) == "cutflow") {
            // Setting for background histograms
            maxY = max(maxY, hist->GetMaximum());
            hist->SetFillColor(histPair.second.color);
            hist->SetLineColor(histPair.second.color);
            stack->Add(hist);
            legend->AddEntry(hist, histPair.second.legendName.c_str(), "f");
        }
        else{
            maxY = max(maxY, hist->GetMaximum());
            hist->SetFillColor(histPair.second.color);
            hist->SetLineColor(histPair.second.color);
            stack->Add(hist);
            legend->AddEntry(hist, histPair.second.legendName.c_str(), "f");
        }

    }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////    


////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
// For signal process's histogram

    TH1* signalHist = nullptr;
    for (const auto& histPair : histograms) {

        if (histPair.second.isSignal) {

            signalHist = histPair.first;
    
            // Modify signal cutflow histograms
            if(std::string(signalHist->GetName()) == "cutflow") {
                // Setting for background histograms
                maxY = max(maxY, signalHist->GetMaximum());
                signalHist->SetLineWidth(3);
                signalHist->SetFillStyle(0);
                signalHist->SetFillColor(histPair.second.color);
                legend->AddEntry(signalHist, histPair.second.legendName.c_str(), "f"); 
            }
            else{
                maxY = max(maxY, signalHist->GetMaximum());
	        signalHist->SetLineWidth(3);
                signalHist->SetFillStyle(0);
                signalHist->SetFillColor(histPair.second.color);
                legend->AddEntry(signalHist, histPair.second.legendName.c_str(), "f");
            }

            break;
        }
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////    

    if (signalHist) {
        maxY = max(maxY, signalHist->GetMaximum());
    }

    TCanvas* canvas = nullptr;
    if (setting.typeOfHisto == "cutflow") {
        canvas = new TCanvas("canvas", setting.title.c_str(), 2000, 1400);

    }
    else{
        canvas = new TCanvas("canvas", setting.title.c_str(), 1300, 1400);
    }

    // Making upper pad
    TPad* upperPad = new TPad("upperPad", "Upper Pad", 0.0, 0.3, 1.0, 1.0);
    TPad* lowerPad = new TPad("lowerPad", "Lower Pad", 0.0, 0.0, 1.0, 0.3);

    // Set the canvas and pad's margin
    canvas->SetRightMargin(0.05);
    canvas->SetLeftMargin(0.12);
    upperPad->SetTopMargin(0.07);
    upperPad->SetBottomMargin(0.02);
    lowerPad->SetTopMargin(0);
    lowerPad->SetBottomMargin(0.1);

    // Draw pads in the canvas
    upperPad->Draw();
    lowerPad->Draw();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
// Upper pad draw setting

    upperPad->cd();
    stack->Draw("HIST");
    //stack->Draw("nostackb");
    stack->GetXaxis()->SetTitle(setting.xAxisTitle.c_str());
    stack->GetYaxis()->SetTitle("Events / bin");
    stack->SetMaximum(maxY * 1.2);
    stack->SetMinimum(1e-4); 
    canvas->Update();

    signalHist->Draw("HISTsame");
    signalHist->SetTitle(setting.title.c_str());
    signalHist->GetYaxis()->SetRangeUser(1e-2, maxY * 2.0); 
    //signalHist->SetMaximum(maxY * 1.2); 
    upperPad->SetLogy(1);
    upperPad->Update();

    //canvas->SetTopMargin(0.07);
    //canvas->SetBottomMargin(0.12);

    setLegendSetting(legend);
    legend->Draw("same");
    TLatex latex;
    setLatexSetting(latex);
    upperPad->Update();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
// Lower pad draw setting

    lowerPad->cd();

    auto* grRatio = CreateRatioPlot(signalHist, stack);
    grRatio->Draw("AP");
    lowerPad->Update();
    //gPad->Update();
    grRatio->SetLineWidth(2);
    grRatio->SetLineColor(kBlue+1);
    grRatio->SetMarkerStyle(21);
    grRatio->SetMarkerColor(kBlue);
    //Eff->GetXaxis()->SetTitle("X-axis title");
    //Eff->GetYaxis()->SetTitle("Efficiency");

    //Eff->SetMinimum(0); // 예를 들어, 최소값을 0으로 설정
    //Eff->SetMaximum(1); // 최대값을 1로 설정 (효율은 0과 1 사이의 값)
    grRatio->Draw("AP");

    lowerPad->Update();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////    


    string saveName = savePath + "/" + histName + "_" + setting.saveName + plotExtension;
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
                dir->GetObject(setting.typeOfHisto.c_str(), hist);
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


        {"QCD", {"QCD.root", false, kRed, "QCD"}},
	{"ttHH", {"ttHH.root", true, 0, "ttHH"}},

	{"ttZZto4b", {"ttZZto4b.root", false, 46, "ttZZto4b"}},
	{"ttZHto4b", {"ttZHto4b.root", false, 30, "ttZHto4b"}},
	{"tt4b", {"tt4b.root", false, 28, "tt4b"}},
	{"ttHtobb", {"ttHtobb.root", false, 11, "ttHtobb"}},
	{"ttZtobb", {"ttZtobb.root", false, 9, "ttZtobb"}},

        {"ttWW", {"ttWW.root", false, 8, "ttWW"}},
        {"ttWH", {"ttWH.root", false, 7, "ttWH"}},
        {"ttWZ", {"ttWZ.root", false, 6, "ttWZ"}},
        {"tttW", {"tttW.root", false, 5, "tttW"}},
        {"tttt", {"tttt.root", false, 4, "tttt"}},

        {"ttTohadronic", {"ttTohadronic.root", false, 3, "tt to had"}}


    };

    unordered_map<string, vector<HistogramSetting>> histogramSettings = {

        {"presel", 
	    {
                {"cutflow", "cutflow of pre-selection", "", "cutflow"}
	    }
	},
        {"baselineHistos", 
            {
                {"histo_njets"    , "number of jets"               , "# of jets"       , "nOfJets"},
                {"histo_nbjets"   , "number of b-jets"             , "# of b-jets"     , "nOfBjets"},
                {"histo_nljets"   , "number of light-jets"         , "# of light-jets" , "nOfLjets"},
                {"hjet01pt"       , "1st jet pT"                   , "pT [GeV]"        , "Jet01pT"},
                {"hjet02pt"       , "2nd jet pT"                   , "pT [GeV]"        , "Jet02pT"},
                {"hjet03pt"       , "3rd jet pT"                   , "pT [GeV]"        , "Jet03pT"},
                {"hjet04pt"       , "4th jet pT"                   , "pT [GeV]"        , "Jet04pT"},
                {"hjet05pt"       , "5th jet pT"                   , "pT [GeV]"        , "Jet05pT"},
                {"hjet06pt"       , "6th jet pT"                   , "pT [GeV]"        , "Jet06pT"},
                {"hjet01eta"      , "1st jet #eta"                 , "#eta"            , "Jet01eta"},
                {"hjet02eta"      , "2nd jet #eta"                 , "#eta"            , "Jet02eta"},
                {"hjet03eta"      , "3rd jet #eta"                 , "#eta"            , "Jet03eta"},
                {"hjet04eta"      , "4th jet #eta"                 , "#eta"            , "Jet04eta"},
                {"hjet05eta"      , "5th jet #eta"                 , "#eta"            , "Jet05eta"},
                {"hjet06eta"      , "6th jet #eta"                 , "#eta"            , "Jet06eta"},
                {"jetHT"          , "jets HT"                      , "HT [GeV]"        , "JetHT"},
                {"jetBHT"         , "b-jets HT"                    , "HT [GeV]"        , "BJetHT"},
                {"lightjetHT"     , "light jets HT"                , "HT [GeV]"        , "LJetHT"},
                {"cutflow"        , "cutflow of baseline selection", ""                , "cutflow"}
            }
	},
        {"channelChi2HH", 
            {
                {"hchi2HH"        , "Reconstructed Higgs pair #chi^{2}", "#chi^{2}"            , "ChiSqrHH"},
                {"hmH1cnd"        , "mass of 1st higgs candidate"      , "invariant mass [GeV]", "H01Mass"},
                {"hmH2cnd"        , "mass of 2nd higgs candidate"      , "invariant mass [GeV]", "H02Mass"},
            }
	},
        {"channelChi2ZZ", 
            {
                {"hchi2ZZ"        , "Reconstructed Z pair #chi^{2}", "#chi^{2}"                , "ChiSqrZZ"},
                {"hmZ1cnd"        , "mass of 1st Z candidate", "invariant mass [GeV]"          , "Z01Mass"},
                {"hmZ2cnd"        , "mass of 2nd Z candidate", "invariant mass [GeV]"          , "Z02Mass"},

            }
	},
        {"channelChi2ZH", 
            {
                {"hchi2ZH"       , "Reconstructed Z pair #chi^{2}", "#chi^{2}"                 , "ChiSqrZH"},
                {"hmHcnd"        , "mass of H candidate", "invariant mass [GeV]"               , "HMass"},
                {"hmZcnd"        , "mass of Z candidate", "invariant mass [GeV]"               , "ZMass"},

            }
	}
    };

    Iteration_Directories_And_Histograms(processes, histogramSettings);

    return 0;
}
