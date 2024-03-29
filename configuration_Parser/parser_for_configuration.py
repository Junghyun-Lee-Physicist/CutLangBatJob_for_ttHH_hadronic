#!/usr/bin/env python3

import re
import sys
import ast

class ConfigParser:

    def __init__(self, file_name):
        self.batch_job_config = {}
        self.output_merger_config = {}
        self.batch_job_output_scaler_config = {}

        self.read_config(file_name) # excecute all config parser method

    def read_config(self, file_name):
        with open(file_name, 'r') as file:
            content = file.read()

        self.read_batch_job_config(content)
        self.read_outputMerger_config(content)
        self.read_batchJobOutputScaler_config(content)

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


    def read_batchJobOutputScaler_config(self, content):
        scaler_config = re.search(r'configure_for_batchJobOutputScaler\{(.*?)\}', content, re.DOTALL)
        if not scaler_config:
            raise ValueError("configure_for_batchJobOutputScaler section not found in config file.")

        config_content = re.sub(r'\\\s*\n', '', scaler_config.group(1))
        for line in config_content.split('\n'):
            line = line.strip()
            if line.startswith('#') or not line:
                continue
            line = line.split('#')[0].strip()
            if ':' in line:
                key, value = [x.strip() for x in line.split(':', 1)]

                if key == 'ChannelInfo':
                    value = self.parse_channel_info(value)
                elif '[' in value and ']' in value:
                    value = self.parse_list_value(value)
                self.batch_job_output_scaler_config[key] = value

    def parse_channel_info(self, channel_info):
        channel_info_list = []
        # Replace newline characters with spaces, remove all spaces, and then concatenate.
        combined_info = ''.join(channel_info).replace('\n', '').replace('\r', '').replace(' ', '')
        parts = re.findall(r'\((.*?)\)', combined_info)
        
        for part in parts:
            elements = part.split(',')
            process = elements[0]
            xs = self.calculate_expression(elements[1])
            br = self.calculate_expression(elements[2])
            num_of_evt = int(elements[3])
            int_lumi = float(elements[4])
        
            channel_info_list.append((process, xs, br, num_of_evt, int_lumi))
        
        return channel_info_list

    def calculate_expression(self, expr):

        expr = expr.replace(' ', '')
        if '*' in expr:
            factors = expr.split('*')
            result = 1.0
            for factor in factors:
                result *= float(factor)
            return result
        elif '/' in expr:
            numer, denom = expr.split('/')
            return float(numer) / float(denom)
        else:
            return float(expr)

####if __name__ == "__main__":
####
####    try:
####        config_parser = ConfigParser("configure.txt")
####    
####    except ValueError as e:
####        raise Exception(f"[ConfigParser] ERROR : Fail to set configuration value: {e}")
