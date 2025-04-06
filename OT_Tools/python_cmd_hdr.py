import os

def update_fp_includes(directory):
    # Check if the starting directory exists
    if not os.path.exists(directory):
        print(f"Error: The directory '{directory}' does not exist.")
        return

    # Walk through the directory recursively
    for root, _, files in os.walk(directory):
        for file in files:
            # Process only .c and .h files
            if file.endswith(('.c', '.h')):
                file_path = os.path.join(root, file)
                print(f"Processing file: {file_path}")

                # Read the file's contents
                with open(file_path, 'r') as f:
                    lines = f.readlines()

                updated_lines = []
                for line in lines:
                    # Check for local includes ending with '_fp.h'
                    if '#include' in line and '_fp.h' in line:
                        if '"' in line:  # Only process local includes
                            # Extract the file name from the include
                            start_index = line.find('"') + 1
                            end_index = line.find('"', start_index)
                            if start_index != -1 and end_index != -1:
                                include_file = line[start_index:end_index]  # Filename inside quotes
                                if include_file.endswith('_fp.h'):
                                    # Modify the include path
                                    new_include = f'#include "TPM/TPMCmd/tpm/include/private/prototypes/{include_file}"\n'
                                    updated_lines.append(new_include)
                                    continue
                    updated_lines.append(line)  # Keep other lines unchanged
                
                # Write the updated lines back to the file
                with open(file_path, 'w') as f:
                    f.writelines(updated_lines)

# Starting directory
update_fp_includes('/home/tcb2196/OpenTitan/opentitan/TPM/TPMCmd/tpm/src/')

