import os 

# Path that this file is in
dir_path = os.path.dirname(os.path.realpath(__file__))

from os import listdir
import os.path
preset_paths = [f for f in listdir(dir_path) if os.path.isfile(os.path.join(dir_path, f)) ]

output_path = os.path.join(dir_path, 'Output')

errors = []
successes = []

# Go through all preset files
for preset_path in [p for p in preset_paths if os.path.splitext(p)[1] == '.slangp'] :
    # print('\n' + os.path.split(preset_path)[1])
    preset_path = os.path.join(dir_path, preset_path)
    template_contents = open(preset_path, "r").read()
    next_pass_index = 0
    # Step through each line of the preset file
    out_preset_contents = ''
    template_lines = template_contents.splitlines()
    template_errors = []
    if template_lines:
        for line in template_lines:
            line = line.replace(':/shaders/shaders_slang/bezel/Mega_Bezel/shaders/HyperspaceMadness/', '../../shaders/HyperspaceMadness/')
            line = line.replace(':/shaders/shaders_slang/', '../../../../')
            out_preset_contents += line + '\n'

    f = open(preset_path, "w")
    f.write(out_preset_contents)
    f.close()

            
            
