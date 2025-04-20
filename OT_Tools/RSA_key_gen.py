from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives import serialization

# Generate private key
private_key = rsa.generate_private_key(
    public_exponent=65537,
    key_size=2048,
)

# Extract key components
private_numbers = private_key.private_numbers()
n = private_numbers.public_numbers.n  # Modulus
e = private_numbers.public_numbers.e  # Public exponent
d = private_numbers.d  # Private exponent

# Helper function to convert integers to uint32_t arrays
def int_to_uint32_array(value):
    byte_data = value.to_bytes((value.bit_length() + 7) // 8, byteorder='big')
    return [int.from_bytes(byte_data[i:i+4], byteorder='big') for i in range(0, len(byte_data), 4)]

# Convert modulus, exponent, and private exponent to uint32_t arrays
modulus_array = int_to_uint32_array(n)    # 256 bytes = 64 uint32_t
exponent_array = int_to_uint32_array(e)   # Usually 1 uint32_t
private_exponent_array = int_to_uint32_array(d)  # 256 bytes = 64 uint32_t

# Function to format uint32_t arrays with 8 columns
def format_uint32_array(array):
    formatted = []
    for i in range(0, len(array), 8):
        row = ", ".join(f"0x{val:08x}" for val in array[i:i+8])
        formatted.append("    " + row)
    return formatted

# Save raw public key (modulus and exponent) to file
with open("raw_public_key.txt", "w") as public_file:
    public_file.write("uint32_t modulus[] = {\n")
    public_file.write(",\n".join(format_uint32_array(modulus_array)))
    public_file.write("\n};\n\n")
    public_file.write("uint32_t exponent[] = {\n")
    public_file.write(",\n".join(format_uint32_array(exponent_array)))
    public_file.write("\n};\n")

# Save raw private key (private exponent) to file
with open("raw_private_key.txt", "w") as private_file:
    private_file.write("uint32_t private_exponent[] = {\n")
    private_file.write(",\n".join(format_uint32_array(private_exponent_array)))
    private_file.write("\n};\n")

# Print key components
print("RSA Key Components:")
print(f"Modulus (n): {hex(n)}")
print(f"Public Exponent (e): {hex(e)}")
print(f"Private Exponent (d): {hex(d)}")

print("Keys have been saved to 'raw_public_key.txt' and 'raw_private_key.txt'")

