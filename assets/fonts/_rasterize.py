import numpy as np
from PIL import Image
from freetype import *
from ctypes import *

font = input("font >>> ")
# font = "calibri.ttf"
font_name = font[:font.index('.')]
face = Face(font)
# face.set_char_size(int(input("size ex: (1500 (small) - 10000 (large)) >>> ")))
face.set_char_size(5000)
# string = input("string >>> ")
string = ''.join([chr(i) for i in range(32, 127)])  # standard characters
spacing = 4

# create ctype int array to hold the x position of each char
offsets = (c_uint16 * (len(string)))()
chars = []
x = 0
bottom = 0
height = 0

# get dimensions of final texture and gather data for each character
for i, curr in enumerate(string):
    face.load_char(curr)
    char_bottom = face.glyph.bitmap.rows - face.glyph.bitmap_top
    bottom = max(bottom, char_bottom)
    height = max(height, face.glyph.bitmap.rows + max(0, char_bottom))
    bitmap = face.glyph.bitmap
    chars.append((x, face.glyph.bitmap_top, bitmap.width, bitmap.rows,
                  np.array(bitmap.buffer, dtype='ubyte').reshape(bitmap.rows,
                                                                 bitmap.width)))
    offsets[i] = x
    x += max(face.glyph.advance.x >> 6, face.glyph.bitmap.width)
    #x += face.glyph.bitmap.width + spacing

buffer = np.zeros((height, x, 4), dtype=np.ubyte)
for x, ctop, cw, ch, cbuffer in chars:
    y = height - bottom - ctop
    buffer[y:y + ch, x:x + cw, 0] += cbuffer

buffer[:,:,1] = buffer[:,:,0]
buffer[:,:,2] = buffer[:,:,0]
buffer[:,:,3] = buffer[:,:,0]
    
with open(f"{font_name}.offs", "wb") as f:
    f.write(c_uint16(len(chars)))
    f.write(offsets)

image = Image.fromarray(buffer, "RGBA")
#image = image.convert("RGBA")
image.save(f"{font_name}.png")
image.close()
