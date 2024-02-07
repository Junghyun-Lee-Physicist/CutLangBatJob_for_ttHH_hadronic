#!/bin/bash
SamplePath=$1
ClusterId=$2
ProcId=$3
mkdir "workDir" && cd "workDir"
"/afs/cern.ch/user/j/junghyun/ttHH_analysis/test/CMSSW_12_6_5/src/CutLang/runs/CLA1.sh" "$SamplePath" CMSNANO -i "/afs/cern.ch/user/j/junghyun/ttHH_analysis/test/CMSSW_12_6_5/src/CutLang/runs/ttHH_had_v2_6.adl"

NewOutputName="histoOut-ttZtobb_$ClusterId_$ProcId.root"
mv histoOut-ttHH_had_v2_6.root $NewOutputName
xrdcp -r -f $NewOutputName "root://eosuser.cern.ch//eos/user/j/junghyun/ttHH/ADL_result/240208/ttZtobb"

NewCsvName="ttZtobb_table_$ClusterId_$ProcId.csv"
mv ttHHcsv.csv $NewCsvName
xrdcp -r -f $NewCsvName "root://eosuser.cern.ch//eos/user/j/junghyun/ttHH/ADL_result/240208/ttZtobb"

cd ..
rm -rf "workDir"
