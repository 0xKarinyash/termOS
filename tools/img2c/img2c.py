import sys
from PIL import Image

TRANSPARENCY_KEY = 0xFF00FF 

def main():
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} <input_image> <output_header_name>")
        sys.exit(1)

    input_path = sys.argv[1]
    output_name = sys.argv[2]
    
    try:
        img = Image.open(input_path).convert("RGBA")
    except Exception as e:
        print(f"Error opening image: {e}")
        sys.exit(1)

    width, height = img.size
    pixels = list(img.getdata())

    c_code = []
    c_code.append(f"#pragma once")
    c_code.append(f'#include "shitgui.h"')
    c_code.append(f"")
    c_code.append(f"// Transparency Key (Magic Number for transparent pixels)")
    c_code.append(f"#define {output_name.upper()}_TRANSPARENCY_KEY 0x{TRANSPARENCY_KEY:06X}")
    c_code.append(f"")
    c_code.append(f"// Image: {width}x{height}")
    c_code.append(f"static const u32 {output_name}_data[] = {{")

    line = "    "
    for i, p in enumerate(pixels):
        r, g, b, a = p
        
        if a < 128:
            val = TRANSPARENCY_KEY
        else:
            val = (r << 16) | (g << 8) | b
        
        line += f"0x{val:06X}, "
        
        if (i + 1) % 8 == 0:
            c_code.append(line.strip())
            line = "    "

    if line.strip():
        c_code.append(line.strip())

    c_code.append(f"}};")
    c_code.append(f"")
    c_code.append(f"static SG_Image {output_name}_img = {{")
    c_code.append(f"    .width = {width},")
    c_code.append(f"    .height = {height},")
    c_code.append(f"    .buffer = (u32*){output_name}_data")
    c_code.append(f"}};")

    # Сохраняем в файл
    output_filename = f"{output_name}.h"
    with open(output_filename, "w") as f:
        f.write("\n".join(c_code))

    print(f"Done! Saved '{output_filename}' with transparency key 0x{TRANSPARENCY_KEY:06X}")

if __name__ == "__main__":
    main()