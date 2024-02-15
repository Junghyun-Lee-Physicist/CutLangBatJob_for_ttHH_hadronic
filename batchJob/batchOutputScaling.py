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


def trim_cutflow_Xlables(oldCutflow):
    # Extract bin labels, contents, and errors from the original histogram into a list.

    want_to_replace_label_inCutflowbins = {
        'size(jets) >= 6' : '# of jets >= 6',
        'size(bjets) >= 4' : '# of b-jets >= 4',
        'pT(jets[5]) > 40' : '6th jets pT > 40',
        'size(electrons) + size(muons) == 0' : 'leptons veto',
        'size(lightjets) >= 2' : '# of light jets >= 2',
        'm(Wcand) [] 30 250' : '30 < reco W mass < 250'
    }

    bins_data = []
    for bin in range(1, oldCutflow.GetNbinsX() + 1):
        label = oldCutflow.GetXaxis().GetBinLabel(bin)
        # Check the replace label if it matches any [ want_to_replace_label_inCutflowbins ]
        for old_label, new_label in want_to_replace_label_inCutflowbins.items():
            if old_label in label:
                label = label.replace(old_label, new_label)
                break
        bins_data.append({
            'label' : label,
            'content' : oldCutflow.GetBinContent(bin),
            'error' : oldCutflow.GetBinError(bin)
        })

    # Filter out the bins that contain labels to be removed.
    unwanted_phrase_inCutflowbins = ["ALL", "Histo", "Save", "~"]
    filtered_bins_data = [
            bin_data for bin_data in bins_data
            if not any(phrase in bin_data['label'] for phrase in unwanted_phrase_inCutflowbins)
            ]

    # Create a new histogram.
    trimmed_hist = ROOT.TH1D(oldCutflow.GetName(), oldCutflow.GetTitle(),
            len(filtered_bins_data),0,len(filtered_bins_data))

    # Fill the new histogram with the filtered data.
    for i, bin_data in enumerate(filtered_bins_data, start=1):
        trimmed_hist.GetXaxis().SetBinLabel(i, bin_data['label'])
        trimmed_hist.SetBinContent(i, bin_data['content'])
        trimmed_hist.SetBinError(i, bin_data['error'])

    return trimmed_hist


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
                        if "cutflow" in obj.GetName():
                            print(f"Processing and trimming cutflow histogram: {obj.GetName()}")
                            trimmed_hist = trim_cutflow_Xlables(obj)
                            trimmed_hist.Sumw2()
                            trimmed_hist.Scale(weight)
                            trimmed_hist.Write()
                        else:
                            print(f"Scaling histogram: {obj.GetName()}")
                            obj.Sumw2()
                            obj.Scale(weight)
                            obj.Write()

        input_file.Close()
        output_file.Close()
        print(f"Output saved to {output_file_path}")

if __name__ == "__main__":
    main()
