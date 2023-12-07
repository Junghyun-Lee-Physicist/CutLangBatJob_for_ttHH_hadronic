#!/usr/bin/env python3

import ROOT

##############################################################################
# Please check the below path

QCD_Storage_Path = "/eos/user/j/junghyun/ttHH/ADL_result/231127/QCD" # Source
Save_Path_for_output = "/eos/user/j/junghyun/ttHH/ADL_result/231127/merged" # Output destination

##############################################################################


def WeightCalculator(nOfEvt, xs):
    # This function will calculate weight
    # formula like below:
    # [ weight = xs / nOfEvt ]
    # of course the [ integrated luminosity have to be multiplied but that thing will be applied at plotting code

    weight = (49810.0 * xs) / nOfEvt # Integrated Luminosity of 2017
    ##weight = (41529.0 * xs) / nOfEvt # Integrated Luminosity of 2017

    return weight


samples = {

    # "sample name"    : (<sample path>, weight value)

    "QCD_HT50to100"    : (QCD_Storage_Path + "/" + "QCD_HT50to100.root"     , WeightCalculator(26243010.0, 187700000.0)),
    "QCD_HT100to200"   : (QCD_Storage_Path + "/" + "QCD_HT100to200.root"    , WeightCalculator(54381393.0, 23640000.0)),
    "QCD_HT200to300"   : (QCD_Storage_Path + "/" + "QCD_HT200to300.root"    , WeightCalculator(42714435.0, 1546000.0)),
    "QCD_HT300to500"   : (QCD_Storage_Path + "/" + "QCD_HT300to500.root"    , WeightCalculator(43429979.0, 321600.0)),
    "QCD_HT500to700"   : (QCD_Storage_Path + "/" + "QCD_HT500to700.root"    , WeightCalculator(36194860.0, 30980.0)),
    "QCD_HT700to1000"  : (QCD_Storage_Path + "/" + "QCD_HT700to1000.root"   , WeightCalculator(32934816.0, 6364.0)),
    "QCD_HT1000to1500" : (QCD_Storage_Path + "/" + "QCD_HT1000to1500.root"  , WeightCalculator(10186734.0, 1117.0)),
    "QCD_HT1500to2000" : (QCD_Storage_Path + "/" + "QCD_HT1500to2000.root"  , WeightCalculator(7701876.0, 108.4)),
    "QCD_HT2000toInf"  : (QCD_Storage_Path + "/" + "QCD_HT2000toInf.root"   , WeightCalculator(4112573.0, 22.36)),
}

# Histogram information
# NOTE: Plese refer Plotting code's [ histoinfos ]

histoinfos = [
    # ["<regionname>", "<histoname>"]
    ["presel", "cutflow"],
    ######################################
    ["baselineForHTHisto", "jetHTbeforeHTCut"],
    ["baselineForHTHisto", "jetHTafterHTCut"],
    ["baselineForHTHisto", "cutflow"],
    ["baselineForWHisto" , "hmWcand"],
    ["baselineForWHisto" , "cutflow"],
    ["baseline", "cutflow"],
    ######################################
    ["baselineHistos", "histo_njets"],
    ["baselineHistos", "histo_nbjets"],
    ["baselineHistos", "histo_nljets"],
    ["baselineHistos", "hjet01pt"],
    ["baselineHistos", "hjet02pt"],
    ["baselineHistos", "hjet03pt"],
    ["baselineHistos", "hjet04pt"],
    ["baselineHistos", "hjet05pt"],
    ["baselineHistos", "hjet06pt"],
    ["baselineHistos", "hjet01eta"],
    ["baselineHistos", "hjet02eta"],
    ["baselineHistos", "hjet03eta"],
    ["baselineHistos", "hjet04eta"],
    ["baselineHistos", "hjet05eta"],
    ["baselineHistos", "hjet06eta"],
    ######################################
    ["baselineHistos", "jetHT"],
    ["baselineHistos", "jetBHT"],
    ["baselineHistos", "cutflow"],
    ######################################
    ["channelChi2", "histo_njets"],
    ["channelChi2", "histo_nbjets"],
    ["channelChi2", "histo_nljets"],
    ["channelChi2", "hjet01pt"],
    ["channelChi2", "hjet02pt"],
    ["channelChi2", "hjet03pt"],
    ["channelChi2", "hjet04pt"],
    ["channelChi2", "hjet05pt"],
    ["channelChi2", "hjet06pt"],
    ["channelChi2", "hjet01eta"],
    ["channelChi2", "hjet02eta"],
    ["channelChi2", "hjet03eta"],
    ["channelChi2", "hjet04eta"],
    ["channelChi2", "hjet05eta"],
    ["channelChi2", "hjet06eta"],
    ["channelChi2", "jetHT"],
    ["channelChi2", "jetBHT"],
    ["channelChi2", "hchi2HH"],
    ["channelChi2", "hmH1cnd"],
    ["channelChi2", "hmH2cnd"],
    ["channelChi2", "hchi2ZZ"],
    ["channelChi2", "hmZ1cnd"],
    ["channelChi2", "hmZ2cnd"],
    ["channelChi2", "hchi2ZH"],
    ["channelChi2", "hmHcnd"],
    ["channelChi2", "hmZcnd"],
    ["channelChi2", "cutflow"],

]

# dictionary for storing merged histogram 
merged_histograms = {}

# interation for each sample
for sample, (file_path, weight) in samples.items():
    file = ROOT.TFile(file_path)

    # interation for each histogram at sample
    for region, histo_name in histoinfos:
        full_histo_name = f"{region}/{histo_name}"
        histogram = file.Get(full_histo_name)

        if histogram:
            # histogram copy using first file
            if full_histo_name not in merged_histograms:
                merged_histograms[full_histo_name] = histogram.Clone(full_histo_name)
                merged_histograms[full_histo_name].SetDirectory(0)
            else:
                # apply the weight and histogram merging
                temp_histo = histogram.Clone()
                temp_histo.Scale(weight)
                merged_histograms[full_histo_name].Add(temp_histo)

    file.Close()

# Save the output file
output_file = ROOT.TFile(Save_Path_for_output + "/QCD.root", "RECREATE")
for full_histo_name, histogram in merged_histograms.items():
    directory = "/".join(full_histo_name.split("/")[:-1])
    histo_name = full_histo_name.split("/")[-1]

    if not output_file.GetDirectory(directory):
        output_file.mkdir(directory)

    output_file.cd(directory)
    histogram.Write(histo_name)


output_file.Close()
