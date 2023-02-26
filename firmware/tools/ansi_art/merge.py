#!/usr/bin/python3

import sys

fg = open(sys.argv[1] if sys.argv[1:] else "fg.ansi", "r")
bg = open(sys.argv[2] if sys.argv[2:] else "bg.ansi", "r")
out = open(sys.argv[3] if sys.argv[3:] else "juliette.ansi", "w")

escape = False

while True:

    char = bg.read(1)
    if not char:
        break

    if char == '\x1b':
        out.write(char)
        escape = True
        continue

    if escape is True:
        out.write(char)

        if char == 'm':
            escape = False

        continue
    
    char_fg = fg.read(1)

    if char_fg != '\x20':
        out.write(char_fg)

    else:
        out.write(char)
