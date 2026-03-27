"""
Prevod policie.c (LVGL uint8_t RGB565, 426x334)
-> police_img.h (uint16_t RGB565, 408x320, zadny orez)

Spust z adresare images/:  python convert_policie.py
"""

import re, sys

SRC   = "policie.c"
DST   = "police_img.h"
W_SRC, H_SRC = 426, 334
W_DST, H_DST = 240, 188

scale  = min(W_DST / W_SRC, H_DST / H_SRC)
W_OUT  = round(W_SRC * scale)
H_OUT  = round(H_SRC * scale)
print(f"Source {W_SRC}x{H_SRC} -> output {W_OUT}x{H_OUT}  (scale {scale:.4f})")

print("Reading policie.c ...")
with open(SRC, "r", encoding="utf-8") as f:
    raw = f.read()

start = raw.index("{") + 1
end   = raw.rindex("}")
chunk = raw[start:end]

nums = [int(x, 16) for x in re.findall(r"0x[0-9A-Fa-f]+", chunk)]
expected = W_SRC * H_SRC * 2
print(f"Read {len(nums)} bytes, expected {expected}")
if len(nums) != expected:
    sys.exit(f"ERROR: byte count mismatch ({len(nums)} != {expected})")

# Reconstruct uint16_t pixels (LVGL little-endian: [lo, hi])
pixels = []
for i in range(0, len(nums), 2):
    pixels.append(nums[i] | (nums[i+1] << 8))

assert len(pixels) == W_SRC * H_SRC

def get_pixel(x, y):
    x = max(0, min(W_SRC - 1, x))
    y = max(0, min(H_SRC - 1, y))
    return pixels[y * W_SRC + x]

def rgb_to_rgb565(r, g, b):
    return ((int(r) & 0x1F) << 11) | ((int(g) & 0x3F) << 5) | (int(b) & 0x1F)

print("Downscaling (bilinear) ...")
out_pixels = []
for dy in range(H_OUT):
    sy = dy / scale
    sy0 = int(sy); sy1 = min(sy0 + 1, H_SRC - 1)
    fy  = sy - sy0
    for dx in range(W_OUT):
        sx = dx / scale
        sx0 = int(sx); sx1 = min(sx0 + 1, W_SRC - 1)
        fx  = sx - sx0
        p00 = get_pixel(sx0, sy0); p10 = get_pixel(sx1, sy0)
        p01 = get_pixel(sx0, sy1); p11 = get_pixel(sx1, sy1)
        r = ((1-fx)*(1-fy)*((p00>>11)&0x1F) + fx*(1-fy)*((p10>>11)&0x1F) +
             (1-fx)*    fy *((p01>>11)&0x1F) + fx*    fy *((p11>>11)&0x1F))
        g = ((1-fx)*(1-fy)*((p00>> 5)&0x3F) + fx*(1-fy)*((p10>> 5)&0x3F) +
             (1-fx)*    fy *((p01>> 5)&0x3F) + fx*    fy *((p11>> 5)&0x3F))
        b = ((1-fx)*(1-fy)*( p00     &0x1F) + fx*(1-fy)*( p10     &0x1F) +
             (1-fx)*    fy *( p01    &0x1F) + fx*    fy *( p11     &0x1F))
        out_pixels.append(rgb_to_rgb565(r, g, b))

print(f"Output pixels: {len(out_pixels)}  ({W_OUT}x{H_OUT})")

print(f"Writing {DST} ...")
with open(DST, "w", encoding="utf-8") as f:
    f.write("#pragma once\n")
    f.write("#include <Arduino.h>\n\n")
    f.write(f"constexpr uint16_t POLICE_IMG_W = {W_OUT};\n")
    f.write(f"constexpr uint16_t POLICE_IMG_H = {H_OUT};\n")
    f.write(f"const uint16_t police_img[{W_OUT * H_OUT}] PROGMEM = {{\n")
    per_row = 16
    for i in range(0, len(out_pixels), per_row):
        row = out_pixels[i:i+per_row]
        f.write("    " + ", ".join(f"0x{p:04X}" for p in row))
        if i + per_row < len(out_pixels):
            f.write(",")
        f.write("\n")
    f.write("};\n")

print("Done.")
