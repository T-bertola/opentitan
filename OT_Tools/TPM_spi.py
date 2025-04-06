import argparse
import serial
import time

#STANDARD COMMAND CODE AND TAGS 

tpm_CC_Create = bytearray([
    0x00, 0x00, 0x01, 0x53
])

tpm_ST_SESSIONS = bytearray([
    0x00, 0x80, 0x02,
])

#HANDLES FOR CREATED OBJECTS

tpm_aes_key_handle = bytearray([
    0x40, 0x00, 0x00, 0x01
])

TPM_AES_KEY_BLOCK = 0


# Example TPM command: Read TPM version (0x80, 0x01 is TPM_STARTUP)
tpm_command_hdr_read_vid_did = bytearray([
    0x83,  # TPM Command, a read of 3 bytes 
    0xD4, 0x0F, 0x00]) #Read the VID, DID register 

tpm_command_hdr_read_intr_status = bytearray([
    0x83,  # TPM Command, a read of 3 bytes 
    0xD4, 0x00, 0x10]) #Read the Interrupt Status register 


tpm_start = bytearray([
    0x00, 
    0x80, 0x01, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00,
    0x01, 0x44, 0x00, 0x00, 
    0x00, 0x00, 0x00
])
tpm_start_hdr_start = bytearray([
    0x0B,   
    0xD4, 0x00, 0x80
    ])


def send_tpm_command(serial_port, command):
    """
    Sends a TPM command over UART and receives the response.

    Args:
        serial_port: The pyserial Serial object.
        command: A bytearray or list of bytes representing the TPM command.

    Returns:
        A bytearray with the TPM's response.
    """
    if not isinstance(command, (list, bytearray)):
        raise ValueError("Command must be a list or bytearray of bytes.")

    # Send the command
    serial_port.write(command)

    # Wait for a response (adjust timeout as needed)
    time.sleep(0.5)

    # Read the response (use an appropriate buffer size)
    response = serial_port.read(1024)
    return response

def adjust_list(array):

    if ((len(array) % 4) == 0):
        mod_chk = 3
    elif ((len(array) % 4) == 3):
        mod_chk = 0
    elif ((len(array) % 4) == 1):
        mod_chk = 2
    elif ((len(array) % 4) == 2):
        mod_chk =1
    else:
        mod_chk = 4 - (len(array) % 4)
    array.insert(0, 0x00)
    zeros = bytearray(mod_chk)
    array.extend(zeros)
    return array


def generate_command_size(tag, command_code, payload):

    command_size = len(tag) + 4 + len(command_code) + len(payload)  

    return command_size.to_bytes(4, byteorder='big')

def send_to_tpm(serial_port, array, size):
    tpm_command_hdr_write = bytearray([
    0x00,  
    0xD4, 0x00, 0x80
    ])
    tpm_command_hdr_write[0] = 0x00 + (size -1)
    print(" ".join(f"{x:02x}" for x in tpm_command_hdr_write))
    print(" ".join(f"{x:02x}" for x in array))
    print("Sending command Header to TPM (4 bytes)...")
    response = send_tpm_command(serial_port, tpm_command_hdr_write)
    print("TPM Response:", response)

    print("Sending the TPM CMD ")
    response = send_tpm_command(serial_port, array)
    print("TPM Response: ",response )

def read_tpm(serial_port, size ):
    #Tells us how many additional 0x00 we need to send to make it divisible by 4
    mod_chk = 4 - (size % 4)
    print(size)
    if(size < 64):
        tpm_zeros = bytearray(size + mod_chk)
        tpm_command_hdr_read = bytearray([
        0x80,   #TPM Command, read a command of 12 bytes
        0xD4, 0x00, 0x80
        ])
        tpm_command_hdr_read[0] = 0x80 + (size -1)
        print("sending a read command", tpm_command_hdr_read)
        response = send_tpm_command(serial_port, tpm_command_hdr_read)
        print("CMD Response:")
        print(" ".join(f"{x:02x}" for x in response))
        #time.sleep(1)
        response = send_tpm_command(serial_port, tpm_zeros)
        print("TPM Response:")
        print(" ".join(f"{x:02x}" for x in response))
    else:
        amount_read = 0
        max_read = 64
        while(amount_read < size):
            amount_read = amount_read + 64
            if((size - amount_read) < 0):
                max_read = abs(size - amount_read)
            tpm_zeros = bytearray(64 + mod_chk)
            tpm_command_hdr_read = bytearray([
            0x80,   #TPM Command, read a command of 12 bytes
            0xD4, 0x00, 0x80
            ])
            tpm_command_hdr_read[0] = 0x80 + (max_read -1)
            print("sending a read command", tpm_command_hdr_read)
            response = send_tpm_command(serial_port, tpm_command_hdr_read)
            print("CMD Response:")
            print(" ".join(f"{x:02x}" for x in response))
            #time.sleep(1)
            response = send_tpm_command(serial_port, tpm_zeros)
            print("TPM Response:")
            print(" ".join(f"{x:02x}" for x in response))
            input("Press Enter to continue reading")



def start_tpm(serial_port):
    print("Starting up TPM...")

    print("Sending the START header")
    response = send_tpm_command(serial_port, tpm_start_hdr_start)
    print("TPM Response:", response)

    print("Sending the START Command")
    response = send_tpm_command(serial_port, tpm_start)
    print("TPM Response:", response)

    # input("Press Enter to read response from TPM...")

    # print("Reading TPM Response")

    # read_tpm(serial_port, 0x0a)

    input("Press Enter to continue once response is read...")

def pcr_extend(serial_port):
    pcr_extend_hdr_1 = bytearray([
        0x3f, 
        0xD4, 0x00, 0x80
    ])
    pcr_extend_1 = bytearray([
        0x00,
        0x80, 0x02, 0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x01, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x09, 0x40, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
        0x0b, 0x18, 0x5f, 0x8d, 0xb3, 0x22, 0x71, 0xfe, 0x25, 0xf5, 0x61, 0xa6, 0xfc, 0x93, 0x8b, 0x2e,
        0x26, 0x43, 0x06, 0xec, 0x30, 0x4e, 0xda, 0x51, 0x80, 0x07, 0xd1, 0x76, 0x48, 0x26, 0x38, 0x19,
        0x00, 0x00, 0x00
    ])

    pcr_extend_hdr_2 = bytearray([
        0x00, 
        0xD4, 0x00, 0x80        
    ])

    pcr_extend_2 = bytearray([
        0x00,
        0x69,
        0x00, 0x00
    ])
    print("Sending command Header to TPM (4 bytes)...")
    response = send_tpm_command(serial_port, pcr_extend_hdr_1)
    print("TPM Response:", response)

    print("Sending the TPM CMD ")
    response = send_tpm_command(serial_port, pcr_extend_1)
    print("TPM Response: ",response )   

    #For now, in simulation I need to feed it in blocks like this, when FPGA comes, i can probably add a second delay 
    input("Press Enter when TPM read in the first block...")

    print("Sending command Header to TPM (4 bytes)...")
    response = send_tpm_command(serial_port, pcr_extend_hdr_2)
    print("TPM Response:", response)

    print("Sending the TPM CMD ")
    response = send_tpm_command(serial_port, pcr_extend_2)
    print("TPM Response: ",response ) 

    #Put reading shit here 

    # input("Press Enter to read from TPM")

    # read_tpm(serial_port, 0x13)

    input("Press Enter to continue once response is read...")

def gen_rsa_key(serial_port):
    PRIM_RSA_GEN_HDR_1 = bytearray([
                            0x3F,   
                            0xD4, 0x00, 0x80
                            ])

    PRIM_KEY_GEN_1 = bytearray([
    0x00,
    0x80, 0x02, 0x00, 0x00, 0x00, 0x63, 0x00, 0x00, 0x01, 0x31, 0x40, 0x00, 0x00, 0x01, 0x00, 0x00, 
    0x00, 0x09, 0x40, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x3a, 0x00, 0x01, 0x00, 0x0b, 0x00, 0x03, 0x04, 0x72, 0x00, 0x00, 0x00, 0x06, 0x00,
    0x80, 0x00, 0x43, 0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xe3, 0xb0, 0xc4, 
    0x00, 0x00, 0x00
])
    
    PRIM_RSA_GEN_HDR_2 = bytearray([
        0x22,   
        0xD4, 0x00, 0x80
        ])

    PRIM_KEY_GEN_2 = bytearray([
    0x00, 
    0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24, 0x27, 0xae, 0x41, 
    0xe4, 0x64, 0x9b, 0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00
])

    print("Sending command Header to TPM (4 bytes)...")
    response = send_tpm_command(serial_port, PRIM_RSA_GEN_HDR_1)
    print("TPM Response:", response)

    print("Sending the TPM CMD ")
    response = send_tpm_command(serial_port, PRIM_KEY_GEN_1)
    print("TPM Response: ",response )   

    #For now, in simulation I need to feed it in blocks like this, when FPGA comes, i can probably add a second delay 
    input("Press Enter when TPM read in the first block...")

    print("Sending command Header to TPM (4 bytes)...")
    response = send_tpm_command(serial_port, PRIM_RSA_GEN_HDR_2)
    print("TPM Response:", response)

    print("Sending the TPM CMD ")
    response = send_tpm_command(serial_port, PRIM_KEY_GEN_2)
    print("TPM Response: ",response ) 

    # input("Press Enter to read from TPM")

    # read_tpm(serial_port, 0x1ee)

    input("Press Enter to continue once response is read...")

def gen_aes_key(serial_port):
    tpm_aes_key_gen_hdr_wr_1 = bytearray([
        0x3F,   
        0xD4, 0x00, 0x80
        ])
    
    tpm_aes_key_gen_1 = bytearray([
    0x00, 
    0x80, 0x02, 0x00, 0x00, 0x00, 0x5b, 0x00, 0x00, 0x01, 0x53, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x09, 0x40, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x32, 0x00, 0x25, 0x00, 0x0b, 0x00, 0x06, 0x00, 0x32, 0x00, 0x20, 0xe1, 0xb9, 0x6d, 
    0x2d, 0x29, 0xdd, 0xa5, 0x52, 0x87, 0x54, 0x14, 0x4d, 0x90, 0x3d, 0xc0, 0xa3, 0xfc, 0x79, 0xa5,
    0x00, 0x00, 0x00
])  

    tpm_aes_key_gen_2 = bytearray([
    0x00,
    0xea, 0x54, 0xf9, 0x8a, 0xda, 0xc3, 0xde, 0xa2, 0x0e, 0x0f, 0xdf, 0x4e, 0x2a, 0x00, 0x06, 0x00, 
    0x80, 0x00, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    
    ])

    tpm_aes_key_gen_hdr_wr_2 = bytearray([
        0x1A,   
        0xD4, 0x00, 0x80
        ])

    print("Sending command Header to TPM (4 bytes)...")
    response = send_tpm_command(serial_port, tpm_aes_key_gen_hdr_wr_1)
    print("TPM Response:", response)

    print("Sending the TPM CMD ")
    response = send_tpm_command(serial_port, tpm_aes_key_gen_1)
    print("TPM Response: ",response )   

    #For now, in simulation I need to feed it in blocks like this, when FPGA comes, i can probably add a second delay 
    input("Press Enter when TPM read in the first block...")

    print("Sending command Header to TPM (4 bytes)...")
    response = send_tpm_command(serial_port, tpm_aes_key_gen_hdr_wr_2)
    print("TPM Response:", response)

    print("Sending the TPM CMD ")
    response = send_tpm_command(serial_port, tpm_aes_key_gen_2)
    print("TPM Response: ",response ) 

    # input("Press Enter to read from TPM")

    # read_tpm(serial_port, 0x1c6)

    input("Press Enter to continue once response is read...")

def load_key(serial_port):
    #put a call to the function to read the private structure from the create response
    private = bytearray([
    0x8e, 
    0x00, 0x20, 0x54, 0xbf, 0x1a, 0x08, 0x7c, 0x3f, 0x81, 0x60, 0x56, 0xd3, 0x64, 0x88, 0xdb, 0x5e, 
    0xe8, 0xad, 0x00, 0xf4, 0x21, 0x42, 0x34, 0xa7, 0xbd, 0xce, 0x61, 0xdf, 0x5c, 0xe8, 0x1d, 0x7d, 
    0x8a, 0xb8, 0x00, 0x10, 0x4d, 0x98, 0xca, 0x70, 0xe8, 0x9a, 0x9a, 0xe8, 0x78, 0x43, 0xbb, 0x29, 
    0x19, 0x3f, 0xcb, 0xc2, 0x63, 0xa7, 0x10, 0x0b, 0xee, 0xee, 0xae, 0x45, 0xe4, 0x43, 0x2a, 0xd5, 
    0x41, 0x03, 0x9a, 0x1b, 0x20, 0x1f, 0xb3, 0x84, 0x90, 0xea, 0x7b, 0xdd, 0x51, 0x35, 0x4c, 0xc4, 
    0xc4, 0x9e, 0x4f, 0x45, 0x13, 0xbe, 0xab, 0x15, 0x16, 0x04, 0x4f, 0x00, 0xbf, 0x51, 0xea, 0x18,
    0x21, 0x55, 0xb1, 0x2a, 0xe7, 0xcb, 0xf3, 0xd8, 0x15, 0x49, 0x6a, 0xdf, 0x52, 0xae, 0xb8, 0x96,
    0x6d, 0x41, 0xe3, 0x15, 0xac, 0x25, 0x7b, 0xc5, 0x0f, 0x80, 0xad, 0xd1, 0xb2, 0x4e, 0x1d, 0xfa, 
    0xa2, 0x78, 0x2f, 0x29, 0xf5, 0xfd, 0x9d, 0x2a, 0x37, 0xb9, 0xac, 0x54, 0xc6, 0xcc
    ])
    public = bytearray([
    0x82, 0x38, 0x9d, 0x70, 0x21, 0x24, 0x46, 0x66, 0x33, 0xef, 0x47, 0x9d, 0xc1, 0x25,
    0x45, 0x8e, 0xfb, 0x67, 0xa5, 0xcd, 0x3c, 0x5f, 0xc7, 0xdb, 0x37, 0x07, 0x9d, 0x04, 0x45, 0xe9, 
    0xde, 0xe5
    ])
    #Construct the command 
    load_key_ary = bytearray([
    0x80, 0x02, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x01, 0x57, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x09, 0x40, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    ])
    load_key_ary_2 = bytearray([
    0x00, 0x52, 0x00, 0x25, 0x00, 
    0x0b, 0x00, 0x06, 0x00, 0x32, 0x00, 0x20, 0xe1, 0xb9, 0x6d, 0x2d, 0x29, 0xdd, 0xa5, 0x52, 0x87, 
    0x54, 0x14, 0x4d, 0x90, 0x3d, 0xc0, 0xa3, 0xfc, 0x79, 0xa5, 0xea, 0x54, 0xf9, 0x8a, 0xda, 0xc3, 
    0xde, 0xa2, 0x0e, 0x0f, 0xdf, 0x4e, 0x2a, 0x00, 0x06, 0x00, 0x80, 0x00, 0x43, 0x00, 0x20
    ])
    load_key_ary.extend(private)
    load_key_ary.extend(load_key_ary_2)
    load_key_ary.extend(public)
    #load_key_ary.extend(zeros)
    blocks_of_data = [load_key_ary[i:i + 64] for i in range(0, len(load_key_ary), 64)]
    for i, block in enumerate(blocks_of_data):
        len_array = len(block)
        new_array = adjust_list(block)
        print(" ".join(f"{x:02x}" for x in load_key_ary))
        print("SIZE", len_array)
        send_to_tpm(serial_port, new_array, len_array)
        input("Press Enter when the block is read in")
    input("Press Enter when LOAD is done")
    #  print(" ".join(f"{x:02x}" for x in load_key_ary[i:i+16]))

def start_auth(serial_port):
    auth_data = bytearray([
    0x80, 0x01, 0x00, 0x00, 0x00, 0x3B, 0x00, 0x00, 0x01, 0x76, 0x40, 0x00, 0x00, 0x07, 0x40, 0x00, 
    0x00, 0x07, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x0B
])

    send_to_tpm(serial_port, adjust_list(auth_data), len(auth_data) - 1 )
    input("Press Enter when Auth is done")

def policy_pcr(serial_port):
    policy_data = bytearray([
    0x80, 0x01, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x01, 0x7f, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x0b, 0x03, 0x0f, 0x00, 0x00    
    ])
    array_size= len(policy_data)
    new_array = adjust_list(policy_data)
    send_to_tpm(serial_port, new_array, array_size)
    input("Pres Enter when Policy PCR is done")

def encrypt_seal(serial_port):
    encrypt = bytearray([
    0x80, 0x02, 0x00, 0x00, 0x00, 0x37, 0x00, 0x00, 0x01, 0x64, 0x80, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x00, 0x09, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x10, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x05, 0x11, 0x22, 0x33, 0x44, 0x55
])
    array_size= len(encrypt)
    new_array = adjust_list(encrypt)
    send_to_tpm(serial_port, new_array, array_size)
    input("Pres Enter when ENCRYPT is done")   

def main():
    # Device path for the pseudo-terminal
    tpm_device = "/dev/pts/5"

    parser = argparse.ArgumentParser(description="Process two flags: -S and -R.")
    parser.add_argument("-S", "--send", type=str, required=False, help="TPM command to send (e.g., RNG)")
    parser.add_argument("-R", "--read", type=str, required=False, help="TPM command to read (e.g., RNG)")
    parser.add_argument("-F", action="store_true", help="Flag F (set if present).")

    args = parser.parse_args()

    try:
        # Open the serial port
        with serial.Serial(tpm_device, baudrate=115200, timeout=1) as serial_port:
            print(f"Opened {tpm_device} for TPM communication.")
            if (args.F):
                tpm_zeros = bytearray([0x00,0x00,0x00,0x00])
                print("Flushing output")
                response = send_tpm_command(serial_port, tpm_zeros)
                print("TPM Response: ", response)
            # Send the command to the TPM
            if (args.send):
                if (args.send.upper() == "RNG"):
                    
                    start_tpm(serial_port)

                    tpm_command_hdr_rng = bytearray([
                    0x0B,   
                    0xD4, 0x00, 0x80
                    ])
                    tpm_command_rng = bytearray([
                    0x00,
                    0x80, 0x01, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x01, 0x7B, 0x00, 0x04, 
                    0x00, 0x00, 0x00
                    ])
                    print("Sending command Header to TPM (4 bytes)...")
                    response = send_tpm_command(serial_port, tpm_command_hdr_rng)
                    print("TPM Response:", response)

                    print("Sending the Command Code")
                    response = send_tpm_command(serial_port, tpm_command_rng)
                    print("TPM Response:", response)

                    input("Press Enter to read TPM...")

                    read_tpm(serial_port, 0x10)

                elif(args.send.upper() == "AES_KEY_GEN_1"):
                    tpm_aes_key_gen_hdr_wr_1 = bytearray([
                        0x3F,   
                        0xD4, 0x00, 0x80
                        ])
                    
                    tpm_aes_key_gen_1 = bytearray([
                    0x00, 
                    0x80, 0x02, 0x00, 0x00, 0x00, 0x5b, 0x00, 0x00, 0x01, 0x31, 0x40, 0x00, 0x00, 0x01, 0x00, 0x00, 
                    0x00, 0x09, 0x40, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 
                    0x00, 0x00, 0x32, 0x00, 0x25, 0x00, 0x0b, 0x00, 0x03, 0x00, 0x72, 0x00, 0x00, 0x00, 0x06, 0x01, 
                    0x00, 0x00, 0x44, 0x00, 0x20, 0x97, 0xc7, 0x0e, 0x77, 0x66, 0x55, 0xe3, 0x8f, 0x62, 0x47, 0x6f,
                    0x00, 0x00, 0x00
                ])

                    
                    print("Sending command Header to TPM (4 bytes)...")
                    response = send_tpm_command(serial_port, tpm_aes_key_gen_hdr_wr_1)
                    print("TPM Response:", response)
                    time.sleep(1)

                    print("Sending the TPM CMD 1")
                    response = send_tpm_command(serial_port, tpm_aes_key_gen_1)
                    print("TPM Response: ",response )

                elif(args.send.upper() == "AES_KEY_GEN_2"):
                    tpm_aes_key_gen_2 = bytearray([
                    0x00,
                    0x06, 0x7a, 0xf3, 0x7f, 0x03, 0xaf, 0x28, 0x7e, 0x37, 0xf4, 0x3a, 0xb1, 0x83, 0x23, 0x51, 0xb5, 
                    0xcc, 0x25, 0xf4, 0x39, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00
                    ])

                    tpm_aes_key_gen_hdr_wr_2 = bytearray([
                        0x1A,   
                        0xD4, 0x00, 0x80
                        ])
        
                    print("Sending command Header to TPM (4 bytes)...")
                    response = send_tpm_command(serial_port, tpm_aes_key_gen_hdr_wr_2)
                    print("TPM Response:", response)
                    time.sleep(1)

                    print("Sending the TPM CMD 2")
                    response = send_tpm_command(serial_port, tpm_aes_key_gen_2)
                    print("TPM Response: ",response )
                    
                elif (args.send.upper() == "AUTH"):
                    tpm_start_auth_session = bytearray([
                        0x00,
                        0x80, 0x01, 0x00, 0x00, 0x00, 0x3B, 0x00, 0x00, 0x01, 0x76, 0x40, 0x00,
                        0x00, 0x07, 0x40, 0x00, 0x00, 0x07, 0x00, 0x20, 0xd4, 0xdf, 0x3f, 0x57, 0x05, 0x23, 0x1d, 0xc0,
                        0xd5, 0x2a, 0xe7, 0x86, 0x62, 0xbd, 0xd9, 0x17, 0xd6, 0x5b, 0xdb, 0x9d, 0x40, 0xbd, 0x73, 0xf0,
                        0xd1, 0xc7, 0x9a, 0x90, 0xfd, 0xa4, 0x6d, 0xf3, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x0b,
                    ])
                    tpm_start_auth_hdr = bytearray([
                        0x3A,   
                        0xD4, 0x00, 0x80
                        ])
                    print("Sending command Header to TPM (4 bytes)...")
                    response = send_tpm_command(serial_port, tpm_start_auth_hdr)
                    print("TPM Response:", response)
                    time.sleep(1)

                    print("Sending the TPM CMD ")
                    response = send_tpm_command(serial_port, tpm_start_auth_session)
                    print("TPM Response: ",response )

                elif (args.send.upper() == "HIERARCHY"):
                    tpm_en_hierarchy = bytearray([
                        0x00,
                        0x80, 0x01, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x01, 0x21, 0x40, 0x00, 0x00, 0x01, 0x40, 0x00, 
                        0x00, 0x01, 0x40,
                    ])
                    tpm_en_hier_hdr = bytearray([
                        0x12,   
                        0xD4, 0x00, 0x80
                        ])
                    print("Sending command Header to TPM (4 bytes)...")
                    response = send_tpm_command(serial_port, tpm_en_hier_hdr)
                    print("TPM Response:", response)
                    time.sleep(1)

                    print("Sending the TPM CMD ")
                    response = send_tpm_command(serial_port, tpm_en_hierarchy)
                    print("TPM Response: ",response ) 

                elif (args.send.upper() == "PRIM_RSA_KEY"):

                    start_tpm(serial_port)
                    gen_rsa_key(serial_port)

                elif (args.send.upper() == "SEAL"):

                    start_tpm(serial_port)
                    input("Enter when start is done")
                    pcr_extend(serial_port)
                    input("Enter when PCR is done")
                    gen_rsa_key(serial_port)
                    input("Enter when RSA key is done")
                    gen_aes_key(serial_port)
                    input("Enter when the key is created")
                    load_key(serial_port)
                    input("Enter when the key is loaded")
                    start_auth(serial_port)
                    input("Enter when the AuthSession is started")
                    policy_pcr(serial_port)
                    encrypt_seal(serial_port)

                    


                   

            elif(args.read):
                tpm_command_hdr_read = bytearray([
                0x8B,   #TPM Command, read a command of 12 bytes
                0xD4, 0x00, 0x80
                ])
                if (args.read.upper() == "REG"):
                    print("sending a read command for the RNG")
                    response = send_tpm_command(serial_port, tpm_command_hdr_read)
                    print("TPM Response:", response)

            else:
                print("Unregonized Argument")


    except serial.SerialException as e:
        print(f"Error opening serial port {tpm_device}: {e}")
    except Exception as e:
        print(f"Error communicating with TPM: {e}")

if __name__ == "__main__":
    main()

