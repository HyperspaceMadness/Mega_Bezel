import os 

# Path that this file is in
root_dir : str = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
build_output_dir : str = os.path.join(root_dir, 'Output')

from os import listdir
import os.path
import shutil
preset_filenames = [f for f in listdir(build_output_dir) if os.path.isfile(os.path.join(build_output_dir, f)) ]

errors = []
successes = []

# Go through all preset files
for file_name in [p for p in preset_filenames if os.path.splitext(p)[1] == '.slangp'] :
    # print('\n' + os.path.split(file_name)[1])
    file_name = os.path.join(build_output_dir, file_name)
    template_contents = open(file_name, "r").read()
    next_pass_index = 0
    # Step through each line of the preset file
    out_preset_contents = ''
    template_lines = template_contents.splitlines()
    template_errors = []
    if template_lines:
        for line in template_lines:
            line = line.replace(':/shaders/shaders_slang/bezel/Mega_Bezel/shaders/', '../../../shaders/')
            line = line.replace(':/shaders/shaders_slang/', '../../../../../')
            out_preset_contents += line + '\n'

    f = open(file_name, "w")
    f.write(out_preset_contents)
    f.close()

print('')
print('Absolute Paths Replace Complete!!!') 


# Go through all preset files
for file_name in [p for p in preset_filenames if os.path.splitext(p)[1] == '.slangp'] :

    out_simple_preset_contents = ''
    template_lines = ['#reference "Root_Presets/' + file_name + '"',
                      '#reference "../../resource/param_values/base/auto-settings.params"']
    if template_lines:
        for line in template_lines:
            out_simple_preset_contents += line + '\n'

    # print('\n' + os.path.split(file_name)[1])
    old_preset_path = os.path.join(build_output_dir, file_name)

    presets_base_path = build_output_dir
    for i in range(3):
        presets_base_path = os.path.split(presets_base_path)[0]

    test_presets_base_path = os.path.join(presets_base_path, 'WIP/_Test_Presets')
    presets_base_path = os.path.join(presets_base_path, 'Presets')

    if '_TEST' in file_name:
        new_preset_path = os.path.join(test_presets_base_path, file_name)
    else:
        if 'DREZ' in file_name:
            new_preset_path = os.path.join(presets_base_path, "Base_CRT_Presets_DREZ/Root_Presets", file_name)
            simple_preset_path = os.path.join(presets_base_path, "Base_CRT_Presets_DREZ", file_name)
        else:
            new_preset_path = os.path.join(presets_base_path, "Base_CRT_Presets/Root_Presets", file_name)
            simple_preset_path = os.path.join(presets_base_path, "Base_CRT_Presets", file_name)

    if os.path.exists(new_preset_path):
        os.replace(old_preset_path, new_preset_path)
    else:
        os.rename(old_preset_path, new_preset_path)

    if os.path.exists(simple_preset_path):
        os.remove(simple_preset_path)
    f = open(simple_preset_path, "w")
    f.write(out_simple_preset_contents)
    f.close()

print('SUCCESS!   Files Move and Replace Complete!!!') 
print('---------------------------------------------------------------------------')
print('')