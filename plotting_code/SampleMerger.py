import os
import subprocess

class SampleMerger:
    def __init__(self, config_file):
        self.directories = self.read_config(config_file)

    def read_config(self, file_name):
        directories = []
        with open(file_name, 'r') as file:
            for line in file:
                parts = line.strip().split()
                
