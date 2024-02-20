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

#include <TRatioPlot.h>
#include <TGaxis.h>

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

void setLatexSetting(TLatex histoLatex, string histo_title){
 
    // Adding CMS private work text
    histoLatex.SetNDC();
    histoLatex.SetTextSize(0.05);
    histoLatex.SetTextAlign(11);
    histoLatex.DrawLatex(0.1, 0.94, "CMS #scale[0.85]{#font[52]{Private Work}}");

    // Adding the year and luminosity
    histoLatex.SetTextSize(0.04);
    histoLatex.SetTextAlign(31); // Align to the right
    histoLatex.DrawLatex(0.9, 0.945, "2017 year, 41.5 fb^{-1} [13 TeV]");

    histoLatex.SetTextSize(0.04);
    histoLatex.SetTextAlign(11);
    string title = "[ " + histo_title + " ]";
    histoLatex.DrawLatex(0.13, 0.89, title.c_str());

}

TGraphAsymmErrors* CreateRatioPlot(TH1* signal, THStack* background, const HistogramSetting& setting){

    auto First_Stacked_histo = (TH1*)background->GetHists()->First();
    double xAxisRange_low  = First_Stacked_histo->GetXaxis()->GetXmin();
    double xAxisRange_high = First_Stacked_histo->GetXaxis()->GetXmax();

    double maxVal = -std::numeric_limits<double>::max();
    double minVal = std::numeric_limits<double>::max();

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

	if(B > 0 && S > 0) {
	    // Calculate Ratio R & it's errors
	    double R = S / B;
	    double errorLow = R * sqrt(pow(sigma_S/S, 2) + pow(sigma_B/B, 2));
	    double errorHigh = errorLow;

	    int iPoint = grRatio->GetN();
            grRatio->SetPoint(iPoint, x, R);
            grRatio->SetPointError(iPoint, 0.0, 0.0, errorLow, errorHigh);
            maxVal = std::max(maxVal, R + errorHigh);
	    minVal = std::min(minVal, R - errorLow);
	}

    } // End of signal iteration


    if(setting.typeOfHisto == "cutflow"){
    //Set of Cutflow
	const int nBins = First_Stacked_histo->GetNbinsX();
	grRatio->GetXaxis()->Set(nBins, xAxisRange_low, xAxisRange_high);
        for(int i = 1; i<= nBins; ++i) {
            const char* label = First_Stacked_histo->GetXaxis()->GetBinLabel(i);
            grRatio->GetXaxis()->SetBinLabel(i, label);
	}

        grRatio->GetXaxis()->LabelsOption("u"); // draw labels up (end of label right adjusted)
        //grRatio->GetXaxis()->LabelsOption("d"); // draw labels down (start of label left adjusted)
        //grRatio->GetXaxis()->LabelsOption("h"); // draw horizontal axis labels
        //grRatio->GetXaxis()->LabelsOption("v"); // draw vertical axis labels

        grRatio->GetXaxis()->SetLabelSize( 0.12 );
        grRatio->GetXaxis()->SetLabelOffset( 0.02 );

	grRatio->GetYaxis()->SetTitle( "#bf{Ratio of [ #it{Sig / Bkg} ]}" );
        grRatio->GetYaxis()->SetTitleOffset( 0.5 );
    }
    else{
    // Set of other histograms
        grRatio->GetXaxis()->SetLimits( xAxisRange_low, xAxisRange_high );
        grRatio->GetXaxis()->SetLabelSize( 0.08 );

	grRatio->GetYaxis()->SetTitle( "#bf{Ratio of [ #it{Sig / Bkg} ]}" );
        grRatio->GetYaxis()->SetTitleOffset( 0.7 );
    }
	
    grRatio->GetXaxis()->SetTitle( (setting.xAxisTitle).c_str() );
    grRatio->GetXaxis()->SetTitleSize( 0.085 );

    double padding = 0.1 * (maxVal - minVal);
    grRatio->GetYaxis()->SetRangeUser( minVal - padding, maxVal + padding);
    //grRatio->GetYaxis()->SetRangeUser( 0.0, 0.1 * 1e-3);
    grRatio->GetYaxis()->SetLabelSize( 0.08 );
    grRatio->GetYaxis()->SetTitleSize( 0.07 );
    grRatio->GetYaxis()->SetNdivisions(505);
    grRatio->GetYaxis()->SetMaxDigits(2);

    grRatio->SetLineWidth(4);
    grRatio->SetLineColor(kBlue+2);
    grRatio->SetMarkerStyle(20);
    grRatio->SetMarkerSize(1);
    grRatio->SetMarkerColor(kBlue);
    grRatio->SetLineWidth(2);

    return grRatio;

}



void DrawStackedHistograms(const vector<pair<TH1*, ProcessInfo>>& histograms, const HistogramSetting& setting, const string& histName) {
 
    double maxY = 0;
    THStack* stack = new THStack("stack", "");
    TLegend* legend = new TLegend(0.5, 0.7, 0.905, 0.93);

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
                signalHist->SetLineWidth(4);
                signalHist->SetFillStyle(0);
                signalHist->SetFillColor(histPair.second.color);
                legend->AddEntry(signalHist, histPair.second.legendName.c_str(), "f"); 
            }
            else{
                maxY = max(maxY, signalHist->GetMaximum());
	        signalHist->SetLineWidth(4);
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
    TPad* upperPad = new TPad("upperPad", "Upper Pad", 0.0, 0.30, 1.0, 1.0);
    TPad* lowerPad = new TPad("lowerPad", "Lower Pad", 0.0, 0.0, 1.0, 0.30);

    // Set the canvas and pad's margin
    canvas->SetRightMargin(0.02);
    canvas->SetLeftMargin(0.12);
    upperPad->SetTopMargin(0.065);
    upperPad->SetBottomMargin(0.02);
    lowerPad->SetTopMargin(0.080);
    lowerPad->SetBottomMargin(0.35);

    // Draw pads in the canvas
    upperPad->Draw();
    lowerPad->Draw();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
// Upper pad draw setting

    upperPad->cd();
    stack->Draw("HIST");
    //stack->Draw("nostackb");
    stack->GetXaxis()->SetTitle(setting.xAxisTitle.c_str());
    stack->GetYaxis()->SetTitle("#bf{# of Events / bin}");
    stack->SetMaximum(maxY * 2.0);
    stack->SetMinimum(1e-4); 
    stack->GetXaxis()->SetLabelSize(0);
    canvas->Update();

    signalHist->Draw("HISTsame");
    //signalHist->GetYaxis()->SetRangeUser(1e-2, maxY * 2.0); 
    //signalHist->SetMaximum(maxY * 1.2); 
    upperPad->SetLogy(1);
    upperPad->Update();

    //canvas->SetTopMargin(0.07);
    //canvas->SetBottomMargin(0.12);

    setLegendSetting(legend);
    legend->Draw("same");
    TLatex latex;
    setLatexSetting(latex, setting.title.c_str());
    upperPad->Update();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
// Lower pad draw setting

    lowerPad->cd();
    lowerPad->SetGrid(1,1);
    lowerPad->SetTickx(1);

    auto* grRatio = CreateRatioPlot(signalHist, stack, setting);
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
	{"ttHH", {"ttHH.root", true, kBlack, "ttHH"}},

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
                {"cutflow", "Cut-flow of pre-selection", "", "cutflow"}
	    }
	},
        {"baselineHistos", 
            {
                {"histo_njets"    , "# of jets"                     , "# of jets"       , "nOfJets"},
                {"histo_nbjets"   , "# of b-jets"                   , "# of b-jets"     , "nOfBjets"},
                {"histo_nljets"   , "# of light-jets"               , "# of light-jets" , "nOfLjets"},
                {"hjet01pt"       , "1#scale[0.8]{st} jet pT"       , "pT [GeV]"        , "Jet01pT"},
                {"hjet02pt"       , "2#scale[0.8]{nd} jet pT"       , "pT [GeV]"        , "Jet02pT"},
                {"hjet03pt"       , "3#scale[0.8]{rd} jet pT"       , "pT [GeV]"        , "Jet03pT"},
                {"hjet04pt"       , "4#scale[0.8]{th} jet pT"       , "pT [GeV]"        , "Jet04pT"},
                {"hjet05pt"       , "5#scale[0.8]{th} jet pT"       , "pT [GeV]"        , "Jet05pT"},
                {"hjet06pt"       , "6#scale[0.8]{th} jet pT"       , "pT [GeV]"        , "Jet06pT"},
                {"hjet01eta"      , "1#scale[0.8]{st} jet #eta"     , "#eta"            , "Jet01eta"},
                {"hjet02eta"      , "2#scale[0.8]{nd} jet #eta"     , "#eta"            , "Jet02eta"},
                {"hjet03eta"      , "3#scale[0.8]{rd} jet #eta"     , "#eta"            , "Jet03eta"},
                {"hjet04eta"      , "4#scale[0.8]{th} jet #eta"     , "#eta"            , "Jet04eta"},
                {"hjet05eta"      , "5#scale[0.8]{th} jet #eta"     , "#eta"            , "Jet05eta"},
                {"hjet06eta"      , "6#scale[0.8]{th} jet #eta"     , "#eta"            , "Jet06eta"},
                {"jetHT"          , "Jets HT"                       , "HT [GeV]"        , "JetHT"},
                {"jetBHT"         , "B-jets HT"                     , "HT [GeV]"        , "BJetHT"},
                {"lightjetHT"     , "Light jets HT"                 , "HT [GeV]"        , "LJetHT"},
                {"cutflow"        , "Cut-flow of baseline selection" , ""                , "cutflow"}
            }
	},
        {"channelChi2HH", 
            {
                {"hchi2HH"        , "Reco Higgs pair #chi^{2}"             , "#chi^{2}"             , "ChiSqrHH"},
                {"hmH1cnd"        , "Mass of 1#scale[0.8]{st} higgs cand"  , "invariant mass [GeV]" , "H01Mass"},
                {"hmH2cnd"        , "Mass of 2#scale[0.8]{nd} higgs cand"  , "invariant mass [GeV]" , "H02Mass"},
            }
	},
        {"channelChi2ZZ", 
            {
                {"hchi2ZZ"        , "Reco Z pair #chi^{2}"             , "#chi^{2}"             , "ChiSqrZZ"},
                {"hmZ1cnd"        , "Mass of 1#scale[0.8]{st} Z cand"  , "invariant mass [GeV]" , "Z01Mass"},
                {"hmZ2cnd"        , "Mass of 2#scale[0.8]{nd} Z cand"  , "invariant mass [GeV]" , "Z02Mass"},

            }
	},
        {"channelChi2ZH", 
            {
                {"hchi2ZH"       , "Reco ZH #chi^{2}", "#chi^{2}"             , "ChiSqrZH"},
                {"hmHcnd"        , "Mass of H cand"  , "invariant mass [GeV]" , "HMass"},
                {"hmZcnd"        , "Mass of Z cand"  , "invariant mass [GeV]" , "ZMass"},

            }
	}
    };

    Iteration_Directories_And_Histograms(processes, histogramSettings);

    return 0;
}
