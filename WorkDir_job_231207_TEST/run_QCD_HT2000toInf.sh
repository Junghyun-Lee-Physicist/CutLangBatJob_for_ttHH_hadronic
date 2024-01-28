#!/bin/bash
SamplePath=$1
ClusterId=$2
ProcId=$3
mkdir "workDir" && cd "workDir"
"/afs/cern.ch/user/j/junghyun/ttHH_analysis/CMSSW_12_6_5/src/CutLang/runs/CLA1.sh" "$SamplePath" CMSNANO -i "/afs/cern.ch/user/j/junghyun/ttHH_analysis/CMSSW_12_6_5/src/CutLang/runs/ttHH_had_v1.adl" -e 2000
NewOutputName="histoOut-QCD_HT2000toInf_$ClusterId_$ProcId.root"
mv histoOut-ttHH_had_v1.root $NewOutputName
xrdcp -r -f $NewOutputName "root://eosuser.cern.ch//eos/user/j/junghyun/ttHH/ADL_result/231207_TEST/QCD_HT2000toInf"
cd ..
rm -rf "workDir"
