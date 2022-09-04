import os 

# Path that this file is in
dir_path = os.path.dirname(os.path.realpath(__file__))

from os import listdir
import os.path
import shutil
preset_paths = [f for f in listdir(dir_path) if os.path.isfile(os.path.join(dir_path, f)) ]

output_path = os.path.join(dir_path, 'Output')

errors = []
successes = []

# Go through all preset files
for file_name in [p for p in preset_paths if os.path.splitext(p)[1] == '.slangp'] :
    # print('\n' + os.path.split(file_name)[1])
    file_name = os.path.join(dir_path, file_name)
    template_contents = open(file_name, "r").read()
    next_pass_index = 0
    # Step through each line of the preset file
    out_preset_contents = ''
    template_lines = template_contents.splitlines()
    template_errors = []
    if template_lines:
        for line in template_lines:
            line = line.replace(':/shaders/shaders_slang/bezel/Mega_Bezel/shaders/', '../../shaders/')
            line = line.replace(':/shaders/shaders_slang/', '../../../../')
            out_preset_contents += line + '\n'

    f = open(file_name, "w")
    f.write(out_preset_contents)
    f.close()

print('Absolute Paths Replace Complete!!!') 


# Go through all preset files
for file_name in [p for p in preset_paths if os.path.splitext(p)[1] == '.slangp'] :
    # print('\n' + os.path.split(file_name)[1])
    old_preset_path = os.path.join(dir_path, file_name)

    presets_base_path = dir_path
    for i in range(3):
        presets_base_path = os.path.split(presets_base_path)[0]

    test_presets_base_path = os.path.join(presets_base_path, 'WIP/_Test_Presets')
    presets_base_path = os.path.join(presets_base_path, 'Presets')

    if '_TEST' in file_name:
        new_preset_path = os.path.join(test_presets_base_path, file_name)
    else:
        if 'DREZ' in file_name:
            new_preset_path = os.path.join(presets_base_path, "Base_CRT_Presets_DREZ", file_name)
        else:
            new_preset_path = os.path.join(presets_base_path, "Base_CRT_Presets", file_name)

    if os.path.exists(new_preset_path):
        os.replace(old_preset_path, new_preset_path)
    else:
        os.rename(old_preset_path, new_preset_path)

print('Files Move and Replace Complete!!!') 
