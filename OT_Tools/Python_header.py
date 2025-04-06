import os

def update_includes(directory):
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(('.c', '.h')):
                file_path = os.path.join(root, file)
                with open(file_path, 'r') as f:
                    lines = f.readlines()
                
                updated_lines = []
                for line in lines:
                    if '#include <openssl/' in line or '# include <openssl/' or '#  include <openssl' in line:
                        # Extract the filename from the include statement
                        start_index = line.find('<openssl/')
                        end_index = line.find('>', start_index)
                        if start_index != -1 and end_index != -1:
                            # Replace with the new include path
                            include_file = line[start_index + 1:end_index]  # Strip <>
                            new_include = f'#include "TPM/TPMCmd/tpm/include/openssl/{include_file}"\n'
                            updated_lines.append(new_include)
                            continue
                    updated_lines.append(line)  # Keep other lines unchanged
                
                # Write back the updated file
                with open(file_path, 'w') as f:
                    f.writelines(updated_lines)

# Run the update
update_includes('/home/tcb2196/OpenTitan/opentitan/TPM')

