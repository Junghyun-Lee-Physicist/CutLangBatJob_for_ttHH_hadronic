#!/usr/bin/env python3

import re
import sys

class ConfigParser:

    def __init__(self, file_name):
        self.batch_job_config = {}
        self.output_merger_config = {}
        self.weights = {}
        self.read_config(file_name)

    def read_config(self, file_name):
        with open(file_name, 'r') as file:
            content = file.read()

        self.read_batch_job_config(content)
        self.read_outputMerger_config(content)
        ########self.read_sample_info_config(content)

    def parse_list_value(self, value):
        # If configure has the [ ], then parser will make the list
        value = value.strip("[] \t\n\r")
        return [item.strip().strip('"').strip("'") for item in value.split(',')]

    def read_batch_job_config(self, content):
        batch_job_config = re.search(r'configure_for_batchJob\{(.*?)\}', content, re.DOTALL)
        # [ re.serach ] module will get the all content within configure_for_batchJob{ ... }

        if not batch_job_config:
            raise ValueError("configure_for_batchJob section not found in config file.")
        
        config_content = re.sub(r'\\\s*\n', '', batch_job_config.group(1))
        # [ re.sub ] module will delete \ + \n
        # That means that
                  # some setting value \
                  # another line \
        # will become
                  # some setting value another line

        for line in config_content.split('\n'):
            line = line.strip()

            if line.startswith('#') or not line:
                continue  # Ignore comment-out or empty line
            # Delete comment-out at the end of the line
            line = line.split('#')[0].strip()

            if ':' in line:
                key, value = [x.strip() for x in line.split(':', 1)]
                if '[' in value and ']' in value:
                    value = self.parse_list_value(value)
                self.batch_job_config[key] = value

    def read_outputMerger_config(self, content):
        output_merger_config = re.search(r'configure_for_batchJobOutputMerger\{(.*?)\}', content, re.DOTALL)
        if not output_merger_config:
            raise ValueError("configure_for_batchJobOutputMerger section not found in config file.")

        config_content = re.sub(r'\\\s*\n', '', output_merger_config.group(1))
        # [ re.sub ] module will delete \ + \n
        # That means that
                  # some setting value \
                  # another line \
        # will become
                  # some setting value another line

        for line in config_content.split('\n'):
            line = line.strip()

            if line.startswith('#') or not line:
                continue  # Ignore comment-out or empty line
            # Delete comment-out at the end of the line
            line = line.split('#')[0].strip()

            if ':' in line:
                key, value = [x.strip() for x in line.split(':', 1)]
                if '[' in value and ']' in value:
                    value = self.parse_list_value(value)
                self.output_merger_config[key] = value


    def read_sample_info_config(self, content):
        sample_info_config = re.search(r'configure_for_sample_information\{(.*?)\}', content, re.DOTALL)
        if not sample_info_config:
            raise ValueError("configure_for_sample_information section not found in config file.")
        for line in sample_info_config.group(1).split('\n'):
            if ':' in line and not line.strip().startswith('#'):
                parts = line.split(',')
                if len(parts) != 3:
                    raise ValueError(f"Invalid sample information line: {line}\nPlease check the number of row")
                process, file_name, weight = [x.strip() for x in parts]
                self.weights[process] = (file_name, float(weight))



####if __name__ == "__main__":
####
####    try:
####        config_parser = ConfigParser("configure.txt")
####    
####    except ValueError as e:
####        raise Exception(f"[ConfigParser] ERROR : Fail to set configuration value: {e}")
