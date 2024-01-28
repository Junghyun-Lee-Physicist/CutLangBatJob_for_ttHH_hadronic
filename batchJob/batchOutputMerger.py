import os
import subprocess

# For using [ configure.txt values ]
import sys
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.dirname(current_dir)
config_dir = os.path.join(parent_dir, 'configuration')
sys.path.append(config_dir)
from parser_for_configuration import ConfigParser


def main():
    config_file = os.path.join(config_dir, "configure.txt")
    config_parser = ConfigParser( config_file )

    output_path = config_parser.output_merger_config.get("output_path")
    channel_names = config_parser.output_merger_config.get("ChannelNames")
    merger_obj_path = config_parser.output_merger_config.get("mergerObj")

    # check the merger object direcotry's existence
    # and make mergerObj directory
    if not os.path.exists(merger_obj_path):
        os.makedirs(merger_obj_path)
    else:
        print(f"Merger directory {merger_obj_path} already exists.")

    # merger batch job output ROOT file every channel
    print("===================================================")
    print(f"All channels are like below: ")
    print(channel_names)
    print("===================================================")
    print("\nNow merger start")
    for channel in channel_names:
        channel_dir = os.path.join(output_path, channel)
        merged_file_path = os.path.join(merger_obj_path, f"{channel}.root")

        # ROOT merge using hadd command
        hadd_command = f"hadd -f {merged_file_path} {channel_dir}/*.root"
        subprocess.run(hadd_command, shell=True, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print(f"Merged {channel} files into {merged_file_path}")

if __name__ == "__main__":
    main()
