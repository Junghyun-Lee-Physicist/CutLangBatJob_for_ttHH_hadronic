
#!/bin/bash

## TTHH single sample file
SamplePath="root://xrootd-cms.infn.it///store/mc/RunIISummer20UL17NanoAODv9/TTHHTo4b_TuneCP5_13TeV-madgraph-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v2/2560000/19E28B9D-F846-8148-9241-370BC79388B5.root"

## QCD HT 300 to 500 sample file
##SamplePath="root://xrootd-cms.infn.it//store/mc/RunIISummer20UL17NanoAODv9/QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v1/2820000/2B81DADF-73E5-5A46-956B-DECDF1E443FD.root"


CutLangPath="/afs/cern.ch/user/j/junghyun/ttHH_analysis/test/CMSSW_12_6_5/src/CutLang"
runsPath="${CutLangPath}/runs"

scriptName="ttHH_had_v2_6.adl"
cp ../script_forRunCL/"${scriptName}" ../script_forRunCL/CLA1.sh ${runsPath}

"${runsPath}/CLA1.sh" "$SamplePath" CMSNANO -i "${runsPath}/${scriptName}" -e 1000
