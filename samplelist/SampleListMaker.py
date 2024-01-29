#!/usr/bin/env python3

import subprocess
import os

def get_dataset_file_list(dataset_name, output_file):

    print("[NOTICE] This is the code for makeing samplelist using das query with data-set")
    print("[NOTICE] Before run this code, Please do [ voms-proxy-init ] first to use das query")

    try:
        # DAS query command
        command = f"dasgoclient -query='file dataset={dataset_name}'"

        # Execute the DAS query
        process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = process.communicate()

        # Check for errors in DAS query
        if process.returncode != 0:
            print(f"Error executing DAS query: {err.decode().strip()}")
            return

        # Convert byte string to string and split lines
        file_list = out.decode('utf-8').split('\n')

        # Filter out empty strings
        file_list = [f for f in file_list if f]

        # Add the xrootd prefix
        xrootd_prefix = "root://xrootd-cms.infn.it/"
        file_list = [xrootd_prefix + f for f in file_list]

        # Write to output file
        with open(output_file, 'w') as file:
            for file_path in file_list:
                file.write(file_path + '\n')

        print(f"File list successfully written to {output_file}")

    except Exception as e:
        print(f"An error occurred: {e}")


# Replace with your dataset name
dataset_name = "/TTTT_TuneCP5_13TeV-amcatnlo-pythia8/RunIISummer20UL17NanoAODv9-106X_mc2017_realistic_v9-v2/NANOAODSIM"

# Replace with your desired output file name
output_file = "tttt.txt"

# Call function
get_dataset_file_list(dataset_name, output_file)
