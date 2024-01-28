#!/bin/bash
SamplePath=$1
ClusterId=$2
ProcId=$3
mkdir "workDir" && cd "workDir"
"/afs/cern.ch/user/j/junghyun/ttHH_analysis/CMSSW_12_6_5/src/CutLang/runs/CLA1.sh" "$SamplePath" CMSNANO -i "/afs/cern.ch/user/j/junghyun/ttHH_analysis/CMSSW_12_6_5/src/CutLang/runs/ttHH_had_v2.adl" -e 1000
NewOutputName="histoOut-QCD_HT700to1000_$ClusterId_$ProcId.root"
mv histoOut-ttHH_had_v2.root $NewOutputName
xrdcp -r -f $NewOutputName "root://eosuser.cern.ch//eos/user/j/junghyun/ttHH/ADL_result/240122_TEST/QCD_HT700to1000"
cd ..
rm -rf "workDir"
