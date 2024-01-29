import ROOT

# Function to read weights from a configuration file
def read_weights(config_file):
    weights = {}
    with open(config_file, 'r') as file:
        for line in file:
            if line.strip() and not line.startswith('#'):
                sample, weight = line.split()
                weights[sample] = float(weight)
    return weights

# Function to scale histograms by the given weight
def scale_histograms(root_file_path, weight, output_file_path):
    # Open the ROOT file
    root_file = ROOT.TFile(root_file_path, 'UPDATE')
    
    # Iterate over all keys in the ROOT file
    for key in root_file.GetListOfKeys():
        obj = key.ReadObj()
        # Check if the object is a histogram
        if obj.InheritsFrom('TH1'):
            # Scale the histogram
            obj.Scale(weight)
            # Write the scaled histogram back
            obj.Write()
    
    # Save the changes to the ROOT file
    root_file.Write()
    root_file.Close()

# Assume 'config.txt' is your configuration file with sample names and weights
weights = read_weights('config.txt')

# Loop through your samples and apply the weights
for sample, weight in weights.items():
    # Construct the ROOT file name for each sample
    root_file_name = f'{sample}.root'
    output_file_name = f'scaled_{sample}.root'
    scale_histograms(root_file_name, weight, output_file_name)
