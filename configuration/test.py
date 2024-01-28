
from parser_for_configuration import ConfigParser


config_parser = ConfigParser("configure.txt")

batch_job_config = config_parser.batch_job_config

jobid = batch_job_config.get("JobID")
sampletype = batch_job_config.get("SampleType")
CutLangExeScriptName = batch_job_config.get("CutLangExeScriptName")
ADLScriptName = batch_job_config.get("ADLScriptName")
ChannelNames =  batch_job_config.get("ChannelNames")
NumberOfEvent =  batch_job_config.get("NumberOfEvent")
EOS_Storage_Path = batch_job_config.get("EOS_Storage_Path")
JobFlavour = batch_job_config.get("JobFlavour")


print(f'''
job id = {jobid}
type = {sampletype}
scriptname = {CutLangExeScriptName}
ADLScriptName = {ADLScriptName}
ChannelNames = {ChannelNames}
NumberOfEvent = {NumberOfEvent}
EOS_Storage_Path = {EOS_Storage_Path}
JobFlavour = {JobFlavour}
''')
