#
# Das Script speichert das JPG-Bild `frank_128x64_clean.jpg` als C-Array.
# 

import os

# --- Konfiguration ---
INPUT_FILENAME = "/users/frank/Source/PlatformIO/biodom-mini/sketches/JPGtoXBM/JPGtoXBM_PoC_2/frank_128x64_clean.jpg"
OUTPUT_FILENAME = "/users/frank/Source/PlatformIO/biodom-mini/sketches/JPGtoXBM/JPGtoXBM_PoC_2/frank_128x64_clean_jpg.h" 
ARRAY_NAME = "image_jpg"

try:
    os.makedirs(os.path.dirname(OUTPUT_FILENAME), exist_ok=True)
    with open(INPUT_FILENAME, 'rb') as f_in, open(OUTPUT_FILENAME, 'w') as f_out:
        f_out.write("#pragma once\n\n")
        f_out.write(f"static const unsigned char {ARRAY_NAME}[] = {{\n  ")
        byte_count = 0
        byte = f_in.read(1)
        while byte:
            f_out.write(f"0x{byte.hex().upper()}, ")
            byte_count += 1
            if byte_count % 16 == 0:
                f_out.write("\n  ")
            byte = f_in.read(1)
        f_out.write("\n};\n")
        print(f"Erfolgreich! '{INPUT_FILENAME}' wurde als C-Array in '{OUTPUT_FILENAME}' gespeichert.")
        print(f"Array-Größe: {byte_count} Bytes.")

except FileNotFoundError:
    print(f"FEHLER: Die Datei '{INPUT_FILENAME}' wurde nicht gefunden. Bitte stelle sicher, dass sie im selben Verzeichnis wie das Skript liegt.")
