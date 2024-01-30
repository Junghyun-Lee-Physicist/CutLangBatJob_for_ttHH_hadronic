import ROOT

# For using [ configure.txt values ]
import os
import sys
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
parser_dir = os.path.join(parent_dir, 'configuration_Parser')
sys.path.append(parser_dir)
from parser_for_configuration import ConfigParser

def main():
    config_file = os.path.join(current_dir, "configure_forBatchJob.txt")
    config_parser = ConfigParser( config_file )
    scale_batch_job_outputs(config_parser)

def scale_batch_job_outputs(config):
    scaler_config = config.batch_job_output_scaler_config
    mergerObj = scaler_config.get("mergerObj")
    scaleObj = scaler_config.get("scaleObj")
    channel_info = scaler_config.get("ChannelInfo")

    print(f"MergerObj: {mergerObj}")
    print(f"ScaleObj: {scaleObj}")

    # Check the merger object directory
    if not os.path.isdir(mergerObj):
        print(f"Error: '{mergerObj}' directory does not exist. Exiting.")
        return

    # Check and create scaleObj directory
    if not os.path.isdir(scaleObj):
        print(f"'{scaleObj}' directory does not exist. Creating it.")
        os.makedirs(scaleObj)
    else:
        print(f"Directory '{scaleObj}' already exists.")

    print(f"mergerObj: {mergerObj}")
    print(f"ScaleObj: {scaleObj}")
    print(f"channel_info : {channel_info}")
    print(f"scaler config: {scaler_config}")
    for channel in channel_info:
        process, xs, br, num_of_evt, int_lumi = channel
        weight = (xs * br * int_lumi) / num_of_evt

        input_file_path = f"{mergerObj}/{process}.root"
        output_file_path = f"{scaleObj}/{process}.root"
        print(f"Processing {input_file_path}")

        # Open the ROOT file
        input_file = ROOT.TFile.Open(input_file_path, "READ")
        output_file = ROOT.TFile.Open(output_file_path, "RECREATE")

        # Iterate over all directories in the ROOT file
        input_file.cd()
        for key in ROOT.TIter(input_file.GetListOfKeys()):
            if key.IsFolder():
                dir_name = key.GetName()
                print(f"Processing directory: {dir_name}")
                input_dir = input_file.GetDirectory(dir_name)
                output_dir = output_file.mkdir(dir_name)
                output_dir.cd()
                
                # Iterate over all histograms in the directory
                for hist_key in ROOT.TIter(input_dir.GetListOfKeys()):
                    obj = hist_key.ReadObj()
                    if obj.InheritsFrom("TH1"):
                        print(f"Scaling histogram: {obj.GetName()}")
                        obj.Sumw2()
                        obj.Scale(weight)
                        obj.Write()

        input_file.Close()
        output_file.Close()
        print(f"Output saved to {output_file_path}")

if __name__ == "__main__":
    main()
