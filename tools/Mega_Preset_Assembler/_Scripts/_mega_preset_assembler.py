import os 
import os.path
from os import listdir

# Path that this file is in
root_dir : str = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
template_dir : str = os.path.join(root_dir, 'Template')
output_dir = os.path.join(root_dir, 'Output')

template_filenames = [f for f in listdir(template_dir) if os.path.isfile(os.path.join(template_dir, f)) ]


errors = []
successes = []

preset_keys = [ 'shader', 
                'filter_linear', 
                'frame_count_mod', 
                'srgb_framebuffer', 
                'float_framebuffer', 
                'mipmap_input', 
                'wrap_mode', 
                'alias', 
                'scale', 
                'scale_x', 
                'scale_y', 
                'scale_type', 
                'scale_type_x', 
                'scale_type_y']

num_presets = 0

def GetNumShaders(in_path : str ) -> int:
    file_lines = open(in_path, "r").read().splitlines()
    num_shaders : int = 0
    if len(component_lines) > 1 and file_lines[0].startswith('shaders = '):
        num_shaders = int(file_lines[0].split(" = ")[1])
    return num_shaders

# return list of lines and index of last pass
def GetPresetLines(in_path : str, output_start_pass_index : int) -> tuple[list[str], int]:
    """

    """
    if not os.path.exists(in_path):
        print('        Path does not exist: ' + in_path)
        return (None, None)
    else:
        num_shaders = 0
        component_lines = open(in_path, "r").read().splitlines()
        out_component_lines : list[str] = []
        next_pass_index : int = output_start_pass_index

        if component_lines[0].startswith('shaders = '):
            num_shaders = int(component_lines[0].split(" = ")[1])
            shader_indexes = range(0, num_shaders)
            new_shader_indexes = range(next_pass_index, next_pass_index + num_shaders)
            processed_line_indexes = []
            
            # Remove the shaders = line
            out_component_lines = component_lines[1:]
            
            # Step through each line of the component to adjust the pass indexes
            zipped_indexes = list(zip(shader_indexes, new_shader_indexes))
            zipped_indexes.reverse()
            for old_index, new_index in zipped_indexes:
                for i in range(len(out_component_lines)):
                    if i not in processed_line_indexes:
                        for key in preset_keys:
                            if key + str(old_index) + " =" in out_component_lines[i]:
                                processed_line_indexes.append(i)
                                out_component_lines[i] = out_component_lines[i].replace(str(old_index) + " =", str(new_index) + " =")
            next_pass_index = output_start_pass_index + num_shaders
        else:
            for component_line in component_lines:
                if component_line.startswith('#reference'):
                    dir_path : str = os.path.split(in_path)[0]
                    reference_path : str = os.path.join(dir_path, component_line.split('"')[1])
                    read_success : bool = False
                    if os.path.exists(reference_path):
                        new_lines, next_pass_index = GetPresetLines(reference_path, next_pass_index)
                        read_success = new_lines != None
                    else:
                        print('        Error while processing: ' + in_path)
                        print('        File does not exist:    ' + reference_path)
                        return None
                    if not read_success :
                        print('        Error while processing: ' + in_path)
                        return None
                    out_component_lines.extend(new_lines)
                else:
                    out_component_lines.append(component_line)
                    
        return (out_component_lines, next_pass_index)

# Go through all template files
for template_filename in [p for p in template_filenames if os.path.splitext(p)[1] == '.protoslangp'] :
    num_presets += 1
    # print('\n' + os.path.split(template_filename)[1])
    template_filename = os.path.join(template_dir, template_filename)
    template_contents = open(template_filename, "r").read()
    next_pass_index = 0
    # Step through each line of the template file
    out_preset_contents = ''
    template_lines = template_contents.splitlines()
    template_errors = []
    if template_lines:
        # print('    Component Presets:')
        for line in template_lines:
            if line.startswith('#reference '):
                # print('        ' + os.path.split(line_split[1])[1])
                component_preset_path = os.path.join(template_dir, line.split('"')[1])
                
                if os.path.exists(component_preset_path):

                    component_lines, next_pass_index = GetPresetLines(component_preset_path, next_pass_index)

                        # Add all the component lines to the output preset
                    for line in component_lines:
                        out_preset_contents += line + '\n'
                else:
                    # print("        Component Preset file not found:" + component_preset_path)
                    template_errors.append( '      Component Preset file not found:' + component_preset_path)
                    continue

        out_preset_contents = "shaders = " + str(next_pass_index) + '\n' + out_preset_contents

        if not template_errors:
            output_file_path = os.path.join(output_dir, os.path.split(template_filename)[1].replace('protoslangp', 'slangp'))
            f = open(output_file_path, "w")
            f.write(out_preset_contents)
            f.close()
            successes.append(os.path.split(output_file_path)[1])
        else:
            errors.append(os.path.split(template_filename)[1])
            errors.extend(template_errors)
            errors.append( '\n' )
    else:
        errors.append( os.path.split(template_filename)[1] + ':' )
        errors.append( '    is Empty\n' )

if successes:
    print('\n    Successfully Assembled:')
    for success in successes:
        print('    ' + success)

if errors:
    print('\nErrors:')
    for error in errors:
        print('    ' + error)
else:
    print('')
    print('SUCCESS!   All (' + str(num_presets) + ') Presets Completed')
    print('---------------------------------------------------------------------------')
    print('')
