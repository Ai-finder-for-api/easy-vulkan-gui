#!/usr/bin/env python3
"""Dev tool: write a small HDR sky EXR for testing import_lighting. Needs an EXR writer (uses OpenEXR's Python
binding if present; otherwise the C++ test writes one with the same OpenEXR). Not part of the library."""
import sys, math, os
try:
    import OpenEXR, Imath, array
except ImportError:
    print("OpenEXR python bindings not installed; skipping (the C++ test uses the raw API).")
    sys.exit(0)
W, H = 256, 128
r = array.array('f'); g = array.array('f'); b = array.array('f')
for y in range(H):
    th = (y + 0.5) / H * math.pi
    for x in range(W):
        ph = (x + 0.5) / W * 2 * math.pi
        d = (math.sin(th) * math.cos(ph), math.cos(th), math.sin(th) * math.sin(ph))
        sun = max(0.0, d[0] * 0.95 + d[1] * 0.3)
        sky = 0.2 + 0.4 * max(d[1], 0)
        r.append(sky + 30 * sun**16); g.append(sky * 0.9 + 22 * sun**16); b.append(sky * 1.2 + 12 * sun**16)
hdr = OpenEXR.Header(W, H)
hdr['channels'] = {c: Imath.Channel(Imath.PixelType(Imath.PixelType.FLOAT)) for c in 'RGB'}
out = os.path.join(os.path.dirname(__file__), '..', 'tests', 'assets', 'sky.exr')
f = OpenEXR.OutputFile(out, hdr)
f.writePixels({'R': r.tobytes(), 'G': g.tobytes(), 'B': b.tobytes()})
print('wrote', os.path.normpath(out))
