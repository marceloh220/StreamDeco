#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Docstring para iconCreate

Created on Wed Jun 12 14:23:45 2024

convert .png images to .ico format for application icons.

"""

import os
from PIL import Image

def blackwhite(input_file, output_file, trashold=16):
    """Convert image to binary (black and white) format."""
    if not os.path.isfile(input_file):
        raise FileNotFoundError(f"Input file {input_file} does not exist")
    if not input_file or not output_file:
        raise ValueError("Input and output file paths must be provided")
    if not input_file.lower().endswith('.png'):
        raise ValueError("Input file must be a .png image")
    img = Image.open(input_file).convert("RGBA")
    img = img.resize((64, 64), resample=Image.LANCZOS)
    img = img.convert("L").point(lambda x: 255 if x < trashold else 0, '1')
    img.save(output_file)
    
if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: python iconCreate.py <input_png_path> <output_ico_path>")
    else:
        input_png = sys.argv[1]
        output_png = sys.argv[2]
        blackwhite(input_png, output_png)
        print(f"Image created at {output_png}")
