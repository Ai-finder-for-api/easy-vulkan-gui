// glTF accessors: validate a bufferView/accessor pair and decode its elements (float, normalized ints, indices).
#include "glb_internal.hpp"
#include "mem.hpp"

namespace evgx { namespace glbi {

uint32_t rd32(const uint8_t* p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }
uint32_t rd16(const uint8_t* p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8); }
float rdf(const uint8_t* p) { union { uint32_t u; float f; } x; x.u = rd32(p); return x.f; }

int J(Ctx& c, int obj, const char* key) { return json::get(c.d, obj, key); }
int I(Ctx& c, int tok, int def) { double v = json::number(c.d, tok, def); return (v > -2e9 && v < 2e9) ? (int)v : def; }
size_t Z(Ctx& c, int tok, double maxv) {
    double v = json::number(c.d, tok, 0);
    if (!(v > 0)) return 0;
    if (v > maxv) v = maxv;
    return (size_t)v;
}
float F(Ctx& c, int tok, double def) {
    double v = json::number(c.d, tok, def);
    if (!(v > -1e30)) v = v != v ? def : -1e30;
    if (v > 1e30) v = 1e30;
    return (float)v;
}

// resolves accessor `ai` to its bytes in the binary chunk, rejecting anything out of bounds or unsupported
bool locate(Ctx& c, int ai, Acc& a) {
    int acc = json::item(c.d, c.accessors, ai);
    if (acc < 0) { c.err = "bad accessor index"; return false; }
    a.ctype = I(c, J(c, acc, "componentType"));
    a.norm = json::boolean(c.d, J(c, acc, "normalized"));
    a.count = (uint32_t)Z(c, J(c, acc, "count"), 2147483648.0);
    int tt = J(c, acc, "type");
    a.ncomp = json::streq(c.d, tt, "SCALAR") ? 1 : json::streq(c.d, tt, "VEC2") ? 2 : json::streq(c.d, tt, "VEC3") ? 3
            : json::streq(c.d, tt, "VEC4") ? 4 : json::streq(c.d, tt, "MAT4") ? 16 : 0;
    if (!a.ncomp) { c.err = "unsupported accessor type"; return false; }
    int bvi = J(c, acc, "bufferView");
    if (bvi < 0) { c.err = "accessor without bufferView (sparse/zero-filled) is not supported"; return false; }
    int bv = json::item(c.d, c.bufferViews, I(c, bvi));
    if (bv < 0) { c.err = "bad bufferView index"; return false; }
    if (I(c, J(c, bv, "buffer")) != 0) { c.err = "external buffers are not supported (use .glb)"; return false; }
    int cs = (a.ctype == 5120 || a.ctype == 5121) ? 1 : (a.ctype == 5122 || a.ctype == 5123) ? 2 : (a.ctype == 5125 || a.ctype == 5126) ? 4 : 0;
    if (!cs) { c.err = "bad componentType"; return false; }
    size_t elem = (size_t)a.ncomp * cs;
    a.stride = Z(c, J(c, bv, "byteStride"), 2147483648.0);
    if (!a.stride) a.stride = elem;
    a.off = Z(c, J(c, bv, "byteOffset")) + Z(c, J(c, acc, "byteOffset"));
    if (a.count && a.off + (size_t)(a.count - 1) * a.stride + elem > c.binlen) { c.err = "accessor out of bounds"; return false; }
    return true;
}

// one component, converted to float (normalized integers map to [-1,1] or [0,1])
float comp(const Acc& a, const uint8_t* p) {
    switch (a.ctype) {
        case 5126: return rdf(p);
        case 5121: return a.norm ? p[0] / 255.f : (float)p[0];
        case 5120: { int v = (int8_t)p[0]; return a.norm ? (v / 127.f < -1.f ? -1.f : v / 127.f) : (float)v; }
        case 5123: { uint32_t v = rd16(p); return a.norm ? v / 65535.f : (float)v; }
        case 5122: { int v = (int16_t)rd16(p); return a.norm ? (v / 32767.f < -1.f ? -1.f : v / 32767.f) : (float)v; }
        default:   return (float)rd32(p);
    }
}

// a VEC3 (or VEC4, dropping w) accessor as an array of vec3
vec3<float>* read_vec3(Ctx& c, int ai, uint32_t* count) {
    Acc a;
    if (!locate(c, ai, a)) return nullptr;
    if (a.ncomp < 3) { c.err = "expected VEC3/VEC4 accessor"; return nullptr; }
    int cs = (a.ctype == 5120 || a.ctype == 5121) ? 1 : (a.ctype == 5122 || a.ctype == 5123) ? 2 : 4;
    vec3<float>* out = alloc_arr<vec3<float>>(a.count ? a.count : 1);
    if (!out) { c.err = "out of memory"; return nullptr; }
    for (uint32_t i = 0; i < a.count; i++) {
        const uint8_t* p = c.bin + a.off + (size_t)i * a.stride;
        out[i] = vec3<float>(comp(a, p), comp(a, p + cs), comp(a, p + 2 * cs));
    }
    *count = a.count;
    return out;
}

// a SCALAR index accessor (u8, u16 or u32) widened to u32
uint32_t* read_indices(Ctx& c, int ai, uint32_t* count) {
    Acc a;
    if (!locate(c, ai, a)) return nullptr;
    if (a.ncomp != 1 || (a.ctype != 5121 && a.ctype != 5123 && a.ctype != 5125)) { c.err = "bad index accessor"; return nullptr; }
    uint32_t* out = alloc_arr<uint32_t>(a.count ? a.count : 1);
    if (!out) { c.err = "out of memory"; return nullptr; }
    for (uint32_t i = 0; i < a.count; i++) {
        const uint8_t* p = c.bin + a.off + (size_t)i * a.stride;
        out[i] = a.ctype == 5121 ? p[0] : a.ctype == 5123 ? rd16(p) : rd32(p);
    }
    *count = a.count;
    return out;
}

}}  // namespace evgx::glbi
