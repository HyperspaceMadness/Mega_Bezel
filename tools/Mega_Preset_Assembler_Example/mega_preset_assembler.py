import os 

# Path that this file is in
dir_path = os.path.dirname(os.path.realpath(__file__))
print(dir_path)

from os import listdir
import os.path
template_paths = [f for f in listdir(dir_path) if os.path.isfile(os.path.join(dir_path, f)) ]

output_path = os.path.join(dir_path, 'Output')

errors = []
successes = []

preset_keys = [ 'shader', 
                'filter_linear', 
                'frame_count_mod', 
                'srgb_framebuffer', 
                'float_framebuffer', 
                'mipmap_input', 
                'alias', 
                'scale', 
                'scale_x', 
                'scale_y', 
                'scale_type', 
                'scale_type_x', 
                'scale_type_y']

# Go through all template files
for template_path in [p for p in template_paths if os.path.splitext(p)[1] == '.protoslangp'] :
    # print('\n' + os.path.split(template_path)[1])
    template_path = os.path.join(dir_path, template_path)
    template_contents = open(template_path, "r").read()
    next_pass_index = 0
    # Step through each line of the template file
    out_preset_contents = ''
    template_lines = template_contents.splitlines()
    template_errors = []
    if template_lines:
        # print('    Component Presets:')
        for line in template_lines:
            if line.startswith('#reference '):
                line_split = line.split('"')
                # print('        ' + os.path.split(line_split[1])[1])
                component_preset_path = os.path.join(dir_path, line_split[1])
                
                if os.path.exists(component_preset_path):
                    component_lines = open(component_preset_path, "r").read().splitlines()

                    if len(component_lines) > 1 and component_lines[0].startswith('shaders = '):
                        num_shaders = int(component_lines[0].split(" = ")[1])
                        shader_indexes = range(0, num_shaders)
                        new_shader_indexes = range(next_pass_index, next_pass_index + num_shaders)
                        processed_line_indexes = []
                        
                        # Remove the shaders = line
                        component_lines = component_lines[1:]
                        # Step through each line of the component to adjust the pass indexes
                        zipped_indexes = list(zip(shader_indexes, new_shader_indexes))
                        zipped_indexes.reverse()
                        for old_index, new_index in zipped_indexes:
                            for i in range(len(component_lines)):
                                if i not in processed_line_indexes:
                                    for key in preset_keys:
                                        if key + str(old_index) + " =" in component_lines[i]:
                                            processed_line_indexes.append(i)
                                            component_lines[i] = component_lines[i].replace(str(old_index) + " =", str(new_index) + " =")
                        # Add all the component lines to the output preset
                        for line in component_lines:
                            out_preset_contents += line + '\n'

                        next_pass_index += num_shaders
                    else:
                        # print("            Can't find Shaders line at first line of the preset, adding all lines without processing")

                        for line in component_lines:
                            out_preset_contents += line + '\n'
                else:
                    # print("        Component Preset file not found:" + component_preset_path)
                    template_errors.append( '      Component Preset file not found:' + component_preset_path)
                    continue

        out_preset_contents = "shaders = " + str(next_pass_index) + '\n' + out_preset_contents

        if not template_errors:
            output_file_path = os.path.join(output_path, os.path.split(template_path)[1].replace('protoslangp', 'slangp'))
            f = open(output_file_path, "w")
            f.write(out_preset_contents)
            f.close()
            successes.append(os.path.split(output_file_path)[1])
        else:
            errors.append(os.path.split(template_path)[1])
            errors.extend(template_errors)
            errors.append( '\n' )
    else:
        errors.append( os.path.split(template_path)[1] + ':' )
        errors.append( '    is Empty\n' )

if successes:
    print('\nSuccessfully Assembled:')
    for success in successes:
        print('    ' + success)

if errors:
    print('\nErrors:')
    for error in errors:
        print('    ' + error)
else:
    print('Completed without Errors!!!') 