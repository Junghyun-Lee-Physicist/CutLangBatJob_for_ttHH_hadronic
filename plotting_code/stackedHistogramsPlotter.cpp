
#include <TFile.h>
#include <THStack.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TH1.h>
#include <TKey.h>
#include <TDirectory.h>
#include <TString.h>

#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;

void Iteration_Directories_And_Histograms(const TString sourcePath, const std::vector<TString>& processes, const std::vector<TString>& channels) {

    if (processes.empty()) {
        std::cout << "[ERROR]: There are no processes. Please check the processes list." << std::endl;
        return;
    }

    for (const auto& process : processes) {

        TFile *file = TFile::Open(sourcePath + "/" + process + ".root");

	// Iteration for all TDirectories in the root file
        TIter nextDir(file->GetListOfKeys());
        TKey *dirKey;
        while ((dirKey = (TKey*)nextDir())) {
            if (dirKey->IsFolder()) {
                TDirectory *dir = (TDirectory*)dirKey->ReadObj();
                std::string dirName = dir->GetName();

                if (std::find(channels.begin(), channels.end(), dirName) == channels.end()) {
	            cout<<"[Plotter Log]: No "<<dirName<<" in "<<process<<"'s root file, Skipping this"<<endl;
                    continue;
                }
                cout<<"[Plotter Log]: "<<dirName<<" exist!! at"<<process<<"'s root file."<<endl;


                ////THStack *stack = new THStack("stack", ("Stacked Histogram for " + dirName).c_str());
                ////TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9); // Legend 위치 설정

                // Iteration for histograms in the TDirectory
                TIter nextHist(dir->GetListOfKeys());
                TKey *histKey;
                while ((histKey = (TKey*)nextHist())) {

                    if (strcmp(histKey->GetClassName(), "TH1D") == 0) {

                        TH1 *hist = (TH1*)histKey->ReadObj();
                        ////stack->Add(hist);
                        ////legend->AddEntry(hist, hist->GetName(), "f");
			TCanvas *canvas = new TCanvas();
			hist->Draw("hist");
			TString saveName = process + "_" + dirName + "_" + hist->GetName() + ".png";
			canvas->SaveAs(saveName);

			delete canvas;

                    }//If statement of histKey's class name
                  //All histograms are TH1D. if not, please fullfill below cases
	          //else if{
	          //    ....
		  //}

                }// End of nextHist interation


                ////TCanvas *canvas = new TCanvas();
                ////stack->Draw("nostack"); // "nostack" option will show each histograms
                ////legend->Draw();

                ////std::string outFileName = dirName + "_" + fileName + ".png";
                ////canvas->SaveAs(outFileName.c_str());

                ////delete canvas;
                ////delete stack;
                ////delete legend;

            }// End of if statement of IsFolder()

        }// End of the nextDir iteration

        file->Close();
    }
}

int main(){
 
    TString sourceSamplePath = "/eos/user/j/junghyun/ttHH/ADL_result/240130/scaled";

    // Define processes and channels which you want to apply plotter
    std::vector<TString> processes = {"ttHH", "QCD"};
    std::vector<TString> channels = {
        "presel",
        "baselineForHTHisto",
        "baselineForWHisto",
        "baseline",
        "baselineHistos",
        "channelChi2"
    };

    Iteration_Directories_And_Histograms(sourceSamplePath, processes, channels);

    return 0;
}

