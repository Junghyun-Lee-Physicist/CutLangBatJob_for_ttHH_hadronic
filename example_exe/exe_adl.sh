
#!/bin/bash

SamplePath="root://xrootd-cms.infn.it///store/mc/RunIISummer20UL17NanoAODv9/TTHHTo4b_TuneCP5_13TeV-madgraph-pythia8/NANOAODSIM/106X_mc2017_realistic_v9-v2/2560000/19E28B9D-F846-8148-9241-370BC79388B5.root"
## TTHH single sample file

CutLangPath="/afs/cern.ch/user/j/junghyun/ttHH_analysis/test/CMSSW_12_6_5/src/CutLang"
runsPath="${CutLangPath}/runs"

cp ../script_forRunCL/ttHH_had_v2_4.adl ../script_forRunCL/CLA1.sh ${runsPath}

"${runsPath}/CLA1.sh" "$SamplePath" CMSNANO -i "${runsPath}/ttHH_had_v2_4.adl" -e 2000
