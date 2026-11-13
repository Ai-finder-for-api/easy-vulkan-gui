#!/usr/bin/env python3
"""Dev-time tool (not part of the library): writes the GLB test assets in tests/assets/."""
import json, math, struct, os, sys

OUT = os.path.join(os.path.dirname(__file__), "..", "tests", "assets")

class Glb:
    def __init__(self):
        self.bin = bytearray(); self.views = []; self.accs = []
    def _view(self, data):
        while len(self.bin) % 4: self.bin.append(0)
        self.views.append({"buffer": 0, "byteOffset": len(self.bin), "byteLength": len(data)})
        self.bin += data
        return len(self.views) - 1
    def acc(self, data, ctype, count, typ, minmax=None, normalized=False):
        a = {"bufferView": self._view(data), "componentType": ctype, "count": count, "type": typ}
        if normalized: a["normalized"] = True
        if minmax: a["min"], a["max"] = minmax
        self.accs.append(a); return len(self.accs) - 1
    def vec3(self, pts):
        data = b"".join(struct.pack("<3f", *p) for p in pts)
        mn = [min(p[i] for p in pts) for i in range(3)]; mx = [max(p[i] for p in pts) for i in range(3)]
        return self.acc(data, 5126, len(pts), "VEC3", (mn, mx))
    def indices(self, idx, kind):
        fmt, ct = {"u8": ("B", 5121), "u16": ("H", 5123), "u32": ("I", 5125)}[kind]
        return self.acc(struct.pack("<%d%s" % (len(idx), fmt), *idx), ct, len(idx), "SCALAR")
    def write(self, path, gltf):
        gltf["asset"] = {"version": "2.0"}
        gltf["accessors"] = self.accs; gltf["bufferViews"] = self.views
        gltf["buffers"] = [{"byteLength": len(self.bin)}]
        j = json.dumps(gltf, separators=(",", ":")).encode()
        j += b" " * ((4 - len(j) % 4) % 4)
        b = bytes(self.bin) + b"\0" * ((4 - len(self.bin) % 4) % 4)
        total = 12 + 8 + len(j) + 8 + len(b)
        with open(path, "wb") as f:
            f.write(struct.pack("<III", 0x46546C67, 2, total))
            f.write(struct.pack("<II", len(j), 0x4E4F534A) + j)
            f.write(struct.pack("<II", len(b), 0x004E4942) + b)
        print("wrote", os.path.normpath(path), total, "bytes")

def sub(a, b): return (a[0]-b[0], a[1]-b[1], a[2]-b[2])
def cross(a, b): return (a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0])
def dot(a, b): return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]

def fix_winding(pos, nrm, idx):
    """Make every triangle CCW with respect to its vertex normals (outward)."""
    out = []
    for i in range(0, len(idx), 3):
        a, b, c = idx[i:i+3]
        n = cross(sub(pos[b], pos[a]), sub(pos[c], pos[a]))
        s = tuple(nrm[a][k] + nrm[b][k] + nrm[c][k] for k in range(3))
        out += [a, b, c] if dot(n, s) >= 0 else [a, c, b]
    return out

def box():
    pos, nrm, idx = [], [], []
    for axis in range(3):
        for sign in (1, -1):
            n = [0, 0, 0]; n[axis] = sign
            u = [0, 0, 0]; v = [0, 0, 0]; u[(axis+1) % 3] = 1; v[(axis+2) % 3] = 1
            base = len(pos)
            for su, sv in ((-1, -1), (1, -1), (1, 1), (-1, 1)):
                pos.append(tuple(0.5*(n[k] + su*u[k] + sv*v[k]) for k in range(3))); nrm.append(tuple(n))
            idx += [base, base+1, base+2, base, base+2, base+3]
    return pos, nrm, fix_winding(pos, nrm, idx)

def sphere(r=0.5, stacks=24, slices=32):
    pos, nrm, idx = [], [], []
    for i in range(stacks + 1):
        th = math.pi * i / stacks
        for j in range(slices + 1):
            ph = 2 * math.pi * j / slices
            n = (math.sin(th)*math.cos(ph), math.cos(th), math.sin(th)*math.sin(ph))
            pos.append(tuple(r*c for c in n)); nrm.append(n)
    for i in range(stacks):
        for j in range(slices):
            a = i*(slices+1)+j; b = a+slices+1
            idx += [a, b, a+1, a+1, b, b+1]
    return pos, nrm, fix_winding(pos, nrm, idx)

def torus(R=0.5, r=0.2, nu=40, nv=20):
    pos, nrm, idx = [], [], []
    for i in range(nu + 1):
        u = 2*math.pi*i/nu
        for j in range(nv + 1):
            v = 2*math.pi*j/nv
            n = (math.cos(v)*math.cos(u), math.sin(v), math.cos(v)*math.sin(u))
            c = (R*math.cos(u), 0, R*math.sin(u))
            pos.append(tuple(c[k] + r*n[k] for k in range(3))); nrm.append(n)
    for i in range(nu):
        for j in range(nv):
            a = i*(nv+1)+j; b = a+nv+1
            idx += [a, b, a+1, a+1, b, b+1]
    return pos, nrm, fix_winding(pos, nrm, idx)

def quat_axis(axis, deg):
    h = math.radians(deg)/2; s = math.sin(h)
    return [axis[0]*s, axis[1]*s, axis[2]*s, math.cos(h)]

def scene():
    g = Glb(); meshes = []
    def add_mesh(pos, nrm, idx, kind, mat, color=None):
        attrs = {"POSITION": g.vec3(pos)}
        if nrm: attrs["NORMAL"] = g.vec3(nrm)
        if color:
            attrs["COLOR_0"] = g.acc(b"".join(struct.pack("<4B", *c) for c in color), 5121, len(color), "VEC4", normalized=True)
        meshes.append({"primitives": [{"attributes": attrs, "indices": g.indices(idx, kind), "material": mat}]})
    p, n, i = box();    add_mesh(p, n, i, "u16", 3)                       # 0 ground (plain box)
    p, n, i = box()                                                        # 1 colored cube, per-face colors via COLOR_0
    faces = [(230,60,60),(60,200,90),(70,110,240),(240,210,70),(200,80,220),(70,220,220)]
    col = [(*faces[k//4], 255) for k in range(24)]
    add_mesh(p, n, i, "u16", 5, col)
    p, n, i = sphere(); add_mesh(p, n, i, "u32", 1)                       # 2 metallic green sphere (u32 indices)
    p, n, i = torus();  add_mesh(p, n, i, "u16", 4)                       # 3 gold torus
    pyr = [(-.5,-.5,-.5),(.5,-.5,-.5),(.5,-.5,.5),(-.5,-.5,.5),(0,.6,0)]   # 4 pyramid: no normals, u8 indices -> flat shading
    pi = [0,1,4, 1,2,4, 2,3,4, 3,0,4, 0,2,1, 0,3,2]
    add_mesh(pyr, None, pi, "u8", 0)
    mats = [
        {"pbrMetallicRoughness": {"baseColorFactor": [0.85, 0.15, 0.12, 1], "metallicFactor": 0.0, "roughnessFactor": 0.6}},
        {"pbrMetallicRoughness": {"baseColorFactor": [0.15, 0.75, 0.30, 1], "metallicFactor": 0.9, "roughnessFactor": 0.3}},
        {"pbrMetallicRoughness": {"baseColorFactor": [0.1, 0.2, 0.9, 1]}, "emissiveFactor": [0.0, 0.0, 0.25]},
        {"pbrMetallicRoughness": {"baseColorFactor": [0.55, 0.55, 0.58, 1], "metallicFactor": 0.0, "roughnessFactor": 0.9}, "doubleSided": True},
        {"pbrMetallicRoughness": {"baseColorFactor": [1.0, 0.76, 0.25, 1], "metallicFactor": 1.0, "roughnessFactor": 0.2}},
        {"pbrMetallicRoughness": {"baseColorFactor": [1, 1, 1, 1], "metallicFactor": 0.0, "roughnessFactor": 0.7}},
    ]
    s = 0.8   # matrix node: translate(0,0,-1.8) * uniform scale 0.8 (column-major)
    nodes = [
        {"name": "root", "rotation": quat_axis((0, 1, 0), 17), "children": [1, 2, 3, 4, 5, 6]},
        {"name": "ground", "mesh": 0, "translation": [0, -0.62, 0], "scale": [7, 0.1, 7]},
        {"name": "cube", "mesh": 1, "translation": [-1.7, 0, 0], "rotation": quat_axis((0, 1, 0), 35)},
        {"name": "sphere", "mesh": 2, "translation": [0, 0, 0], "children": [7]},
        {"name": "torus", "mesh": 3, "translation": [1.8, 0, 0], "rotation": quat_axis((1, 0, 0), 70)},
        {"name": "pyramid", "mesh": 4, "matrix": [s,0,0,0, 0,s,0,0, 0,0,s,0, 0,0,-1.8,1]},
        {"name": "empty-group"},
        {"name": "moon", "mesh": 2, "translation": [0, 0.85, 0], "scale": [0.3, 0.3, 0.3]},
    ]
    g.write(os.path.join(OUT, "scene.glb"), {"scene": 0, "scenes": [{"nodes": [0]}], "nodes": nodes, "meshes": meshes, "materials": mats})

def quad(g, z, half, mat, ccw=True):
    pos = [(-half,-half,z),(half,-half,z),(half,half,z),(-half,half,z)]
    nrm = [(0,0,1)]*4
    idx = [0,1,2,0,2,3] if ccw else [0,2,1,0,3,2]
    return {"primitives": [{"attributes": {"POSITION": g.vec3(pos), "NORMAL": g.vec3(nrm)}, "indices": g.indices(idx, "u16"), "material": mat}]}

def depth():
    """Near red quad listed BEFORE a far blue quad (painter's order would be wrong); a back-facing green quad in between."""
    g = Glb()
    meshes = [quad(g, 0.0, 0.6, 0), quad(g, -3.0, 4.0, 1), quad(g, -1.5, 4.0, 2, ccw=False)]
    flat = lambda c: {"pbrMetallicRoughness": {"baseColorFactor": c, "metallicFactor": 0.0, "roughnessFactor": 1.0}}
    mats = [flat([1, 0, 0, 1]), flat([0, 0, 1, 1]), flat([0, 1, 0, 1])]
    nodes = [{"mesh": 0}, {"mesh": 1}, {"mesh": 2}]
    g.write(os.path.join(OUT, "depth.glb"), {"scene": 0, "scenes": [{"nodes": [0, 1, 2]}], "nodes": nodes, "meshes": meshes, "materials": mats})

def nearclip():
    """A huge floor quad passing under and behind the camera: exercises near-plane clipping."""
    g = Glb()
    pos = [(-50,-1,50),(50,-1,50),(50,-1,-50),(-50,-1,-50)]
    nrm = [(0,1,0)]*4
    meshes = [{"primitives": [{"attributes": {"POSITION": g.vec3(pos), "NORMAL": g.vec3(nrm)}, "indices": g.indices([0,1,2,0,2,3], "u16"), "material": 0}]}]
    mats = [{"pbrMetallicRoughness": {"baseColorFactor": [0.8, 0.8, 0.8, 1], "metallicFactor": 0.0, "roughnessFactor": 1.0}}]
    g.write(os.path.join(OUT, "floor.glb"), {"scene": 0, "scenes": [{"nodes": [0]}], "nodes": [{"mesh": 0}], "meshes": meshes, "materials": mats})

if __name__ == "__main__":
    os.makedirs(OUT, exist_ok=True)
    scene(); depth(); nearclip()
