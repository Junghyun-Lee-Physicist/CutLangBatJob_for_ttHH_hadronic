#!/usr/bin/env python3

import os, sys, time, random
from string import *

import ROOT
from ROOT import gStyle, TFile, TH1, TH1D, TH2D, TCanvas, TLegend, TColor, TLatex, THStack


# Basic sets for ROOT styling parameters:
gStyle.SetOptStat(0)
gStyle.SetPalette(1)

gStyle.SetTextFont(42)

gStyle.SetTitleStyle(0000)
gStyle.SetTitleBorderSize(0)
gStyle.SetTitleFont(42)
gStyle.SetTitleFontSize(0.055)

gStyle.SetTitleFont(42, "xyz")
gStyle.SetTitleSize(0.5, "xyz")
gStyle.SetLabelFont(42, "xyz")
gStyle.SetLabelSize(0.45, "xyz")


# Put the samples in a list:
fileSamples = open("Config_forPlotting_v1.txt").readlines()

# Set the Sample's path
StoragePath = "/eos/user/j/junghyun/ttHH/ADL_result/231127/merged"

# Dictionary container
# { key : value }
samples = {
    # '<Sample type> : <Color code number>
    'ttHH' : 1,  # kBlack
    'ttZHto4b' : 400+2,  # kYellow+2
    'ttZZto4b' : 600+1,  # kBlue+1
    'tt4b' : 432+2, # kCyan+2
    'ttHtobb' :  860+10, # KAzure+10
    'QCD' : 632, # kRed
     ##    'EWK' : 616+1, # kMagenta+1
}

bgs = ['ttZHto4b', 'ttZZto4b', 'tt4b', 'ttHtobb', 'QCD']
sgs = ['ttHH',]


for sampleType, colorCode in samples.items():
    print("--------------------------------------------------------------------")
    print("Color-code [ %d ] will be used at sample type [ %s ]" % (colorCode, sampleType))
    sampleinfos = []
    for line in fileSamples:
        # Delete unnecessary line of sample text file
        if "#" in line or len(line) < 2:
            continue

        # [ strip ] will remove white-space characters
        # from both sides of text line, including [ \n ]
        cleanLine = line.strip()
        # Then, [ split ] will divide clean-line
        # based on whitespace characters (spaces, tabs, newline characters, etc)
        line = cleanLine.split()
        # e.g) if [ line        = " TTZH ZHto4b 0.0112 \n" ]
        #         [ strip(line) = "TTZH ZHto4b 0.0112" ]
        #         [ split(strip(line) = "[TTZH, ZHto4b, 0.0112]" ]

        if line[0] == sampleType:
            line[1] = line[1] + '.root'  # ZHto4b -> ZHto4b.root
            SampleAddress = StoragePath + "/" + line[1]
            line.append(TFile(SampleAddress))  # "[TTZH, ZHto4b.root, 0.0112, TFile(<path-to-sample>/ZHto4b.root]"
            line[2] = float(line[2]) # convert weight str to float

            sampleinfos.append(line) # put the above line to the sampleinfos
            samples[sampleType] = colorCode, sampleinfos
            # samples[sampleType] = (<colorCode number>, [[TTZH, ZHto4b.root, 0.0112, TFile(<path-to-sample>/ZHto4b.root)])
            print(f"Current sampleinfos: ")
            print(sampleinfos)

print("")
print("-------------------------------------------------------------------------")
print("These are the samples color code")
print(samples)
print("-------------------------------------------------------------------------")


#############################################################################################################


# Open files
# Now let's draw some histograms.
# We will compare distributions for different variables.
# You can try this with different histograms in different regions.
# Provide the region and histo names in the following list:

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

print("")
print("-------------------------------------------------------------------------")
print("Now stack histogram to each histo\n")

# Loop over the histograms and prepare the plots:
for hinfo in histoinfos:

    # In which region would you like to draw? You can change the region name.
    region = hinfo[0]
    # Which histogram would you like to plot?
    hname = hinfo[1]

    print("-------------------------------------------------------------------------")
    print(f"Current region & name : [ {region} & {hname} ]\n")

    # Make a plot legend
    # Change the entry names to reflect processes you are plotting!
    leg = TLegend(0.52, 0.70, 0.85, 0.82)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0000)
    leg.SetNColumns(2)

    # histogram set for background
    hbg = THStack("hbg", "")
    print("    Let's handle the background info\n")
    for bg in bgs:
        col, smplist = samples[bg]
        print(f"        current bg type : {bg}")
        print(f"        it's info : {smplist}")
        hbgi = smplist[0][3].Get(region + "/" + hname)
        hbgi.Scale(smplist[0][2]) # Set scale using calculated weight
        print(f"        For scale using following weight value : {smplist[0][2]}\n") 

        # set for cutflow plot
        if hname == "cutflow":
            nbins = 0
            for i in range(1, hbgi.GetXaxis().GetNbins() + 1):
                binLabel = hbgi.GetXaxis().GetBinLabel(i)
                ##if not ("Histo" in binLabel or "ALL" in binLabel or "bjets_forZZreco" in binLabel or "bjets_forZHreco" in binLabel or "chi2HH" in binLabel or "chi2ZZ" in binLabel or "chi2ZH" in binLabel):
                if not ("Histo" in binLabel or "ALL" in binLabel or "bjets_forZZreco" in binLabel or "bjets_forZHreco" in binLabel or "chi2ZZ" in binLabel or "chi2ZH" in binLabel):
                    nbins = nbins + 1
            hbginew = TH1D(hbgi.GetName() + str(random.random()), hbgi.GetTitle(), nbins, 0, nbins)
            ibin = 0
            for i in range(1, hbgi.GetXaxis().GetNbins() + 1):
                label = hbgi.GetXaxis().GetBinLabel(i)
                binLabel = label
                ##if not ("Histo" in label or "ALL" in label):
                ##if not ("Histo" in binLabel or "ALL" in binLabel or "bjets_forZZreco" in binLabel or "bjets_forZHreco" in binLabel or "chi2HH" in binLabel or "chi2ZZ" in binLabel or "chi2ZH" in binLabel):
                if not ("Histo" in binLabel or "ALL" in binLabel or "bjets_forZZreco" in binLabel or "bjets_forZHreco" in binLabel or "chi2ZZ" in binLabel or "chi2ZH" in binLabel):
                    ibin = ibin + 1
                    hbginew.GetXaxis().SetBinLabel(ibin, label)
                    hbginew.SetBinContent(ibin, hbgi.GetBinContent(i))
            hbgi = hbginew

        if hname == "histo_njets" or "histo_nbjets" or "histo_nljets":
            xaxis = hbgi.GetXaxis()
            xaxis.SetTitle("number of jets")


        # set for stack plot
        hbgi.SetLineColor(col)
        hbgi.SetFillColor(col)
        hbg.Add(hbgi)

        leg.AddEntry(hbgi, bg, "f")

    # Manage the signal histograms
    hsgs = []
    print("    Let's handle the signal info\n")
    for sg in sgs:
        col, smplist = samples[sg]
        print(f"        current bg type : {sg}")
        print(f"        it's info : {smplist}")
        hsg = smplist[0][3].Get(region + "/" + hname)
        hsg.Scale(smplist[0][2])
        print(f"        For scale using following weight value : {smplist[0][2]}\n")

        if hname == "cutflow":
            nbins = 0
            for i in range(1, hsg.GetXaxis().GetNbins() + 1):

                binLabel = hsg.GetXaxis().GetBinLabel(i)
                ##if not ("Histo" in binLabel or "ALL" in binLabel):
                ##if not ("Histo" in binLabel or "ALL" in binLabel or "bjets_forZZreco" in binLabel or "bjets_forZHreco" in binLabel or "chi2HH" in binLabel or "chi2ZZ" in binLabel or "chi2ZH" in binLabel):
                if not ("Histo" in binLabel or "ALL" in binLabel or "bjets_forZZreco" in binLabel or "bjets_forZHreco" in binLabel or "chi2ZZ" in binLabel or "chi2ZH" in binLabel):
                    nbins = nbins + 1
            hsgnew = TH1D(hsg.GetName() + str(random.random()), hsg.GetTitle(), nbins, 0, nbins)
            ibin = 0
            for i in range(1, hsg.GetXaxis().GetNbins() + 1):
                label = hsg.GetXaxis().GetBinLabel(i)
                binLabel = label
                ##if not ("Histo" in label or "ALL" in label):
                ##if not ("Histo" in binLabel or "ALL" in binLabel or "bjets_forZZreco" in binLabel or "bjets_forZHreco" in binLabel or "chi2HH" in binLabel or "chi2ZZ" in binLabel or "chi2ZH" in binLabel):
                if not ("Histo" in binLabel or "ALL" in binLabel or "bjets_forZZreco" in binLabel or "bjets_forZHreco" in binLabel or "chi2ZZ" in binLabel or "chi2ZH" in binLabel):
                    ibin = ibin + 1
                    hsgnew.GetXaxis().SetBinLabel(ibin, label)
                    hsgnew.SetBinContent(ibin, hsg.GetBinContent(i))
            hsg = hsgnew
        hsg.SetLineColor(col)
        hsg.SetLineWidth(3)
        leg.AddEntry(hsg, sg, "l")
        hsgs.append(hsg)



    # Find the maximum
    mxm = max(hbg.GetMaximum(), hsgs[0].GetMaximum()) *1.2 ####
    ####mim = min(hbg.GetMinimum(), hsgs[0].GetMinimum()) *0.9 

    # Draw the histograms
    hsgs[0].SetTitle("")
    hsgs[0].SetMaximum(mxm) ####
    ####hsgs[0].SetMinimum(mim) ####
  
    ####hsgs[0].GetXaxis().SetTitle(hbg.GetTitle())
    ##if hname == "histo_njets" or "histo_nbjets" or "histo_nljets":
    ##   hsgs[0].GetXaxis().SetTitle("number of jets")
    ##if hname == "hjet01pt" or "hjet02pt" or "hjet03pt" or "hjet04pt" or "hjet05pt" or "hjet06pt":
    ##   hsgs[0].GetXaxis().SetTitle("jet #pT")
    ##if hname == "hjet01eta" or "hjet02eta" or "hjet03eta" or "hjet04eta" or "hjet05eta" or "hjet06eta":
    ##   hsgs[0].GetXaxis().SetTitle("jet #eta")
    if hname == "jetHT":
       hsgs[0].GetXaxis().SetTitle("HT")
    if hname == "jetBHT":
       hsgs[0].GetXaxis().SetTitle("b-jet HT")
   ## if hname == "hchi2HH":
   ##    hsgs[0].GetXaxis().SetTitle("#chi^{2} of Higgs pair")
    if hname == "hmH1cnd":
       hsgs[0].GetXaxis().SetTitle("1st Higgs candidate mass")
    if hname == "hmH2cnd":
       hsgs[0].GetXaxis().SetTitle("2nd Higgs candidate mass")
    ##else:
    ##hsgs[0].GetXaxis().SetTitle(hbg.GetTitle())
    ##hsgs[0].GetXaxis().SetTitle("#chi^{2} of Higgs pair")


    ##hsgs[0].GetXaxis().SetTitleOffset(1.25)
    hsgs[0].GetXaxis().SetTitleOffset(0.8)
    hsgs[0].GetXaxis().SetTitleSize(0.05)
    hsgs[0].GetXaxis().SetLabelSize(0.045)
    hsgs[0].GetXaxis().SetNdivisions(8, 5, 0)
    hsgs[0].GetYaxis().SetTitle("Number of events")
    ##hsgs[0].GetYaxis().SetTitleOffset(1.4)
    hsgs[0].GetYaxis().SetTitleOffset(1.1)
    hsgs[0].GetYaxis().SetTitleSize(0.05)
    hsgs[0].GetYaxis().SetLabelSize(0.045)

    # Write the region name on the histogram
    t = TLatex(0.60, 0.85, region)
    t.SetTextSize(0.041)
    t.SetTextFont(42)
    t.SetNDC()

    cxwidth = 650
    if hname == "cutflow":
        cxwidth = 1000
    # Now we make a canvas and draw our histograms
    # Canvas for stack plot with ratio of signal and backgrounds
    c = TCanvas("c_" + region + "_" + hname, "c_" + region + "_" + hname, cxwidth, 700)
    ####c.SetLeftMargin(0.15)
    ####c.SetRightMargin(0.15)

    logy = True
    if logy == True:
        hsgs[0].SetMaximum(hsgs[0].GetMaximum() * 3)
        hsgs[0].SetMinimum(1e-2)
        ####hsgs[0].GetYaxis().SetRangeUser( hsgs[0].GetMinimum(), hsgs[0].GetMaximum() )
        c.SetLogy(logy)
    hsgs[0].Draw("hist")
    hbg.Draw("hist same")
    for hsg in hsgs:
        hsg.Draw("hist same")
        hsg.Draw("axis same")
    c.Draw()
    leg.Draw("same")
    t.Draw("same")
    c.Update()   

    c.Print('.png')
    print("-------------------------------------------------------------------------")

