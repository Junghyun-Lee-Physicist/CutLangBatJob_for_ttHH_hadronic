#!/usr/bin/env python3

import os
import subprocess
import shutil

###################################################################################
# User define configuration

# For using [ configure.txt values ]
import sys
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
parser_dir = os.path.join(parent_dir, 'configuration_Parser')
sys.path.append(parser_dir)
from parser_for_configuration import ConfigParser

config_file = os.path.join(current_dir, "configure_forBatchJob.txt")
config_parser = ConfigParser( config_file )

JobID = config_parser.batch_job_config.get("JobID")
CutLang_path = config_parser.batch_job_config.get("CutLang_path")
SampleType = config_parser.batch_job_config.get("SampleType")
CutLangExeScriptName = config_parser.batch_job_config.get("CutLangExeScriptName")
ADLScriptName = config_parser.batch_job_config.get("ADLScriptName")
ChannelNames  = config_parser.batch_job_config.get("ChannelNames")
NumberOfEvent = config_parser.batch_job_config.get("NumberOfEvent")
EOS_Storage_Path = config_parser.batch_job_config.get("EOS_Storage_Path")
JobFlavour = config_parser.batch_job_config.get("JobFlavour")

print(f'''

#####################################################################

* Let's check the configuration for batch jobs

  - Current job id : {JobID}
  - Setted CutLang path : {CutLang_path}
  - Inputted sample type : {SampleType}
  - CutLang execution shell script : {CutLangExeScriptName}
  - ADL analysis script : {ADLScriptName}
  - Number of Event which you want to run : {NumberOfEvent}
  - EOS Storage path for save CutLang output : {EOS_Storage_Path}
  - Job Flavour for batch job : {JobFlavour}

#####################################################################
''')

###################################################################################


runScriptPath = os.path.join(parent_dir, 'script_forRunCL')
samplelistPath = os.path.join(parent_dir, 'samplelist')
proxyPath = os.path.join(current_dir, 'proxy.cert')
CutLangExeScriptPath = os.path.join(CutLang_path, 'runs', CutLangExeScriptName + '.sh')
ADLScriptPath = os.path.join(CutLang_path, 'runs', ADLScriptName + '.adl')


def copy_adlScript_to_ScriptDir_atCutLang():
     
    try:
        subprocess.run(["ls", CutLang_path + "/runs/"], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print(f"There is [ {CutLang_path}/runs/ ] directory")
    except subprocess.CalledProcessError as error:
        print(f"[ERROR] There is no /runs/ directory at {CutLang_path}. Please check the CutLang installation")

    print(f"Now copy the [ {CutLangExeScriptName}.sh ] and [ {ADLScriptName}.adl ] to the [ CutLang/runs ] directory")
    shutil.copy( runScriptPath + "/" + CutLangExeScriptName + ".sh", CutLang_path + "/runs/" )
    shutil.copy( runScriptPath + "/" + ADLScriptName + ".adl", CutLang_path + "/runs/" )


def create_WorkDir():
    nameOfWorkDir = "WorkDir_for_batchJob_" + JobID
    try:
        subprocess.run(["ls", nameOfWorkDir], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print(f"work directory for condor job & jobID: [ {JobID} ] already exists at PWD")
        print(f"skip making directory...")
    except:
        subprocess.run(["mkdir", nameOfWorkDir], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print(f"Created jobID [ {JobID} ] work directory at : {nameOfWorkDir}")

    return nameOfWorkDir


def create_logDir_at_WorkDir(nameOfWorkDir):

    nameOfLogDir = "logForJob_" + JobID
    try:
        subprocess.run(["ls", nameOfLogDir], cwd = nameOfWorkDir, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print(f"log directory for job {JobID} already exists at WorkDir")
        print(f"skip making directory...")
    except:
        subprocess.run(["mkdir", nameOfLogDir], cwd = nameOfWorkDir, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print(f"Created job [ {JobID} ]'s log directory: {nameOfLogDir}")

    for directory in [ "error", "output", "log" ]:
        try:
            subprocess.run(["ls", directory], cwd = nameOfWorkDir + "/" + nameOfLogDir, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            print(f"{directory} log for job {JobID} already exists at {nameOfLogDir}")
            print(f"skip making directory...")
        except:
            subprocess.run(["mkdir", directory], cwd = nameOfWorkDir + "/" + nameOfLogDir, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            print(f"Created directory: {nameOfLogDir}/{directory} for job {JobID}")

    return nameOfLogDir


def create_outputDir_at_EOSstorage_forEachChannel(base_storage, channel_names):
    for channel in channel_names:
        job_directory = f"{base_storage}/{JobID}"
        directory = f"{base_storage}/{JobID}/{channel}"
     
        try:
            # Check if directory exists
            subprocess.run(["ls", job_directory], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            print(f"job output dir for channel [ {channel} ] already exists at: {job_directory}")
            print(f"skip making directory...")
        except:
            # Directory does not exist, so create it
            subprocess.run(["mkdir", job_directory], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            print(f"Created directory: {job_directory}")

        try:
            # Check if directory exists
            subprocess.run(["ls", directory], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            print(f"job output dir for channel [ {channel} ] already exists at: {directory}")
            print(f"skip making directory...")
        except:
            # Directory does not exist, so create it
            subprocess.run(["mkdir", directory], check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            print(f"Created directory: {directory}")


def create_script(nevt, channel, outputDir_XRootD_Protocol_path, nameOfWorkDir):

    CutLang_cmd  = f"\"{CutLangExeScriptPath}\" \"$SamplePath\" {SampleType} -i \"{ADLScriptPath}\""
    if(nevt.isdigit()):
        CutLang_cmd += f" -e {nevt}"

    script_content = f'''#!/bin/bash
SamplePath=$1
ClusterId=$2
ProcId=$3
mkdir "workDir" && cd "workDir"
{CutLang_cmd}
NewOutputName=\"histoOut-{channel}_$ClusterId_$ProcId.root\"
mv histoOut-{ADLScriptName}.root $NewOutputName
xrdcp -r -f $NewOutputName \"{outputDir_XRootD_Protocol_path}\"
cd ..
rm -rf "workDir"
'''

    script_name = f"run_{channel}.sh"
    script_path = f"{nameOfWorkDir}/{script_name}"
    with open(script_path, "w") as file:
        file.write(script_content)
    os.system("chmod +x " + script_path)
    return script_name


def main():

    copy_adlScript_to_ScriptDir_atCutLang()
    nameOfWorkDir = create_WorkDir()
    nameOfLogDir = create_logDir_at_WorkDir(nameOfWorkDir)
    create_outputDir_at_EOSstorage_forEachChannel(EOS_Storage_Path, ChannelNames)

    for Channel in ChannelNames:
        channel = Channel
        samplelistfile = f"{samplelistPath}/{channel}.txt"
        nevt = NumberOfEvent
    
        outputDir_XRootD_Protocol_path = "root://eosuser.cern.ch/" + EOS_Storage_Path + "/" + JobID + "/" + channel
        script_name = create_script(nevt, channel, outputDir_XRootD_Protocol_path, nameOfWorkDir)
        condor_content = f'''x509userproxy         = {proxyPath}
executable            = {script_name}
getenv                = True
arguments             = $(samplelist) $(ClusterId) $(ProcId)
output                = {nameOfLogDir}/output/{channel}_$(ClusterId).$(ProcId).out
error                 = {nameOfLogDir}/error/{channel}_$(ClusterId).$(ProcId).err
log                   = {nameOfLogDir}/log/{channel}_$(ClusterId).log
should_transfer_files = IF_NEEDED
+JobFlavour = \"{JobFlavour}\"
queue samplelist from {samplelistfile}\n'''
    
    
        with open(f"{nameOfWorkDir}/condor_submit_{channel}.sub", "w") as file:
            file.write(condor_content)
    
        try:
            # Execute the condor_submit command
            result = subprocess.run(f"condor_submit condor_submit_{channel}.sub", cwd = nameOfWorkDir, shell=True, check=True)
            print(f"Condor job for channel [ {channel} ] successfully submitted.")
        except subprocess.CalledProcessError as error:
            # Handle errors during command execution
            print(f"[ERROR] Error occurred while submitting Condor job for channel [ {channel} ]: {error}")
    

if __name__ == "__main__":
    main()
