#!/usr/bin/python3

import re
import math

source = open("juliette.ansi", "r")
code_out = open("../../src/console/ansi_art.h", "w")


def appendPixelTable(color, char, repeat):

    if repeat == 0:
        return

    if repeat > 1:
        bitmapTable.append(0x00)
        bitmapTable.append(repeat)

    # Write unicode character (U+0080 to U+FFFF)
    if ord(char) >= 0x0080 and ord(char) <= 0xFFFF:
        bitmapTable.append(0x01)
        bitmapTable.append(ord(char) >> 8)
        bitmapTable.append(ord(char) & 0xFF)

    # Write standard ASCII character (U+0020 to U+007F)
    elif ord(char) >= 0x0020 and ord(char) <= 0x007F:
        bitmapTable.append(ord(char))

    # Unsupported character, replace by a space character (U+0020)
    else:
        bitmapTable.append(0x20)

    # Write RGB565 color value   
    bitmapTable.append(color >> 8)
    bitmapTable.append(color & 0xFF)


escape = False
bitmapTable = []
currentLinePixels = 0
pixelRepeat=0
prevColorIndex=None
prevChar=None
currentColor = -1



while True:

    char = source.read(1)

    if not char:
        break


    if char == '\x1b':
        esc_seq = ""
        escape = True
        continue

    if escape is True:

        esc_seq += char

        if char == 'm':
            escape = False

            # Parse color escape sequence from source
            res = re.search("\[48;2;(\d+);(\d+);(\d+)m", esc_seq)
            if not res:
                continue

            (r,g,b) = res.groups()

            # Convert the 24-bit RGB value to 16-bit RGB565
            r = math.floor(int(r) * 31 / 255)
            g = math.floor(int(g) * 63 / 255)
            b = math.floor(int(b) * 31 / 255)
            currentColor = (r << 11) + (g << 5) + b 

        continue

    if char == '\x0a' or char == '\x0b':

        if currentLinePixels == 0:
            continue

        currentLinePixels = 0
        

        appendPixelTable(currentColor, prevChar, pixelRepeat)
        bitmapTable.append(0x0A)
        
        pixelRepeat = 0
        prevChar = None
        prevColorIndex = None

        continue

    currentLinePixels += 1

    if prevChar is None:
        prevChar = char

    if prevColorIndex is None:
        prevColorIndex = currentColor

    if prevChar == char and prevColorIndex == currentColor:
        pixelRepeat += 1
        continue

    appendPixelTable(prevColorIndex, prevChar, pixelRepeat)

    prevChar = char
    prevColorIndex = currentColor
    pixelRepeat = 1


# Add the last pixel
if currentLinePixels > 0:
    appendPixelTable(currentColor, prevChar, pixelRepeat)


# Generate the include file header
code_out.write("//******************************************************************************\r\n")
code_out.write("//\r\n")
code_out.write("// Project : Alarm Clock V3\r\n")
code_out.write("// File    : src/console/ansi_art.h\r\n")
code_out.write("// Author  : Benoit Frigon <www.bfrigon.com>\r\n")
code_out.write("//\r\n")
code_out.write("// For Juliette...\r\n")
code_out.write("//\r\n")
code_out.write("// -----------------------------------------------------------------------------\r\n")
code_out.write("//\r\n")
code_out.write("// This work is licensed under the Creative Commons Attribution-ShareAlike 4.0\r\n")
code_out.write("// International License. To view a copy of this license, visit\r\n")
code_out.write("//\r\n")
code_out.write("// http://creativecommons.org/licenses/by-sa/4.0/\r\n")
code_out.write("//\r\n")
code_out.write("// or send a letter to Creative Commons,\r\n")
code_out.write("// PO Box 1866, Mountain View, CA 94042, USA.\r\n")
code_out.write("//\r\n")
code_out.write("//******************************************************************************\r\n")
code_out.write("#ifndef ANSI_ART_H\r\n")
code_out.write("#define ANSI_ART_H\r\n")
code_out.write("\r\n")
code_out.write("#include <avr/pgmspace.h>\r\n")
code_out.write("\r\n")
code_out.write("\r\n")
code_out.write("\r\n")


# Generate the bitmap table 
code_out.write("const uint8_t PROGMEM juliette_bitmap[] = {\r\n")
code_out.write("    ")

i = 0
for pixel in bitmapTable:
    i += 1
    code_out.write("0x{0:02X}, ".format(pixel))
    if i > 20:
        code_out.write("\r\n    ")
        i = 0

code_out.write("};\r\n\r\n")

code_out.write("\r\n#endif /* ANSI_ART_H */ ")