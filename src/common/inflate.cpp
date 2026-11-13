// DEFLATE decoder: bit reader, canonical Huffman tables built from code lengths, and the three block types.
// Written from RFC 1951 / RFC 1950. All memory comes from evg's allocator.
#include <evg/common/inflate.hpp>
#include <evg/common/hash.hpp>
#include "mem.hpp"

namespace evgx { namespace inflate {

namespace {

// growing output buffer; capacity doubles, capped at 2 GiB
struct Out {
    uint8_t* p = nullptr;
    size_t n = 0, cap = 0;

    bool reserve(size_t extra) {
        if (n + extra <= cap) return true;
        size_t nc = cap ? cap : 4096;
        while (nc < n + extra) {
            if (nc > ((size_t)1 << 30)) return false;
            nc *= 2;
        }
        uint8_t* q = (uint8_t*)mem_alloc(nc);
        if (!q) return false;
        for (size_t i = 0; i < n; i++) q[i] = p[i];
        mem_free(p);
        p = q;
        cap = nc;
        return true;
    }
};

// reads bits least-significant first from a byte buffer
struct Bits {
    const uint8_t* in;
    size_t n;
    size_t pos = 0;
    uint32_t buf = 0;
    int cnt = 0;

    // reads k bits (k <= 16); false when the input ends first
    bool get(int k, uint32_t& v) {
        while (cnt < k) {
            if (pos >= n) return false;
            buf |= (uint32_t)in[pos++] << cnt;
            cnt += 8;
        }
        v = buf & ((1u << k) - 1);
        buf >>= k;
        cnt -= k;
        return true;
    }

    // drops the bits left in the current byte; the next read starts on a byte boundary
    void align() { buf = 0; cnt = 0; }
};

// canonical Huffman table: count[len] codes of each length and the symbols in code order
struct Huff {
    uint16_t count[16];
    uint16_t symbol[320];
};

// builds the table from code lengths. Returns 0 when complete, >0 when incomplete, <0 when over-subscribed.
int build(Huff& h, const uint8_t* lengths, int n) {
    for (int i = 0; i < 16; i++) h.count[i] = 0;
    for (int s = 0; s < n; s++) h.count[lengths[s]]++;
    if (h.count[0] == n) return 0;                          // no codes at all
    int left = 1;
    for (int len = 1; len < 16; len++) {
        left <<= 1;
        left -= h.count[len];
        if (left < 0) return left;
    }
    uint16_t offs[16];
    offs[1] = 0;
    for (int len = 1; len < 15; len++) offs[len + 1] = (uint16_t)(offs[len] + h.count[len]);
    for (int s = 0; s < n; s++)
        if (lengths[s]) h.symbol[offs[lengths[s]]++] = (uint16_t)s;
    return left;
}

// decodes one symbol, reading one bit at a time
bool decode(Bits& b, const Huff& h, int& sym) {
    int code = 0, first = 0, index = 0;
    for (int len = 1; len < 16; len++) {
        uint32_t bit;
        if (!b.get(1, bit)) return false;
        code |= (int)bit;
        int count = h.count[len];
        if (code - count < first) {
            sym = h.symbol[index + (code - first)];
            return true;
        }
        index += count;
        first += count;
        first <<= 1;
        code <<= 1;
    }
    return false;                                           // not a valid code
}

const uint16_t kLenBase[29] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
                               35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
const uint8_t kLenExtra[29] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
                               3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
const uint16_t kDistBase[30] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
                                257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
const uint8_t kDistExtra[30] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
                                7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

// decodes literal/length and distance symbols until the end-of-block code
bool codes(Bits& b, Out& o, const Huff& lc, const Huff& dc, const char** err) {
    for (;;) {
        int sym;
        if (!decode(b, lc, sym)) { *err = "bad Huffman code in data"; return false; }
        if (sym < 256) {
            if (!o.reserve(1)) { *err = "out of memory"; return false; }
            o.p[o.n++] = (uint8_t)sym;
        } else if (sym == 256) {
            return true;
        } else {
            sym -= 257;
            if (sym >= 29) { *err = "bad length code"; return false; }
            uint32_t extra;
            if (!b.get(kLenExtra[sym], extra)) { *err = "truncated data"; return false; }
            size_t len = kLenBase[sym] + extra;
            int ds;
            if (!decode(b, dc, ds) || ds >= 30) { *err = "bad distance code"; return false; }
            uint32_t dextra;
            if (!b.get(kDistExtra[ds], dextra)) { *err = "truncated data"; return false; }
            size_t dist = kDistBase[ds] + dextra;
            if (dist > o.n) { *err = "distance reaches before the start of the output"; return false; }
            if (!o.reserve(len)) { *err = "out of memory"; return false; }
            for (size_t k = 0; k < len; k++) {              // byte by byte: copies may overlap their source
                o.p[o.n] = o.p[o.n - dist];
                o.n++;
            }
        }
    }
}

bool stored_block(Bits& b, Out& o, const char** err) {
    b.align();
    uint32_t len, nlen;
    if (!b.get(16, len) || !b.get(16, nlen)) { *err = "truncated stored block"; return false; }
    if ((len ^ 0xFFFFu) != nlen) { *err = "stored block length check failed"; return false; }
    if (b.pos + len > b.n) { *err = "truncated stored block"; return false; }
    if (!o.reserve(len)) { *err = "out of memory"; return false; }
    for (uint32_t i = 0; i < len; i++) o.p[o.n++] = b.in[b.pos + i];
    b.pos += len;
    return true;
}

bool fixed_block(Bits& b, Out& o, const char** err) {
    static Huff lc, dc;
    static bool ready = false;
    if (!ready) {
        uint8_t lengths[288 + 30];
        int i = 0;
        for (; i < 144; i++) lengths[i] = 8;
        for (; i < 256; i++) lengths[i] = 9;
        for (; i < 280; i++) lengths[i] = 7;
        for (; i < 288; i++) lengths[i] = 8;
        for (; i < 288 + 30; i++) lengths[i] = 5;
        build(lc, lengths, 288);
        build(dc, lengths + 288, 30);
        ready = true;
    }
    return codes(b, o, lc, dc, err);
}

bool dynamic_block(Bits& b, Out& o, const char** err) {
    uint32_t nlen, ndist, ncode;
    if (!b.get(5, nlen) || !b.get(5, ndist) || !b.get(4, ncode)) { *err = "truncated block header"; return false; }
    nlen += 257;
    ndist += 1;
    ncode += 4;
    if (nlen > 286 || ndist > 30) { *err = "bad block header counts"; return false; }

    static const uint8_t order[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    uint8_t lengths[288 + 32] = {};
    for (uint32_t i = 0; i < ncode; i++) {
        uint32_t v;
        if (!b.get(3, v)) { *err = "truncated block header"; return false; }
        lengths[order[i]] = (uint8_t)v;
    }
    Huff codeLen;
    if (build(codeLen, lengths, 19) != 0) { *err = "bad code-length code"; return false; }

    uint32_t total = nlen + ndist, index = 0;
    for (int i = 0; i < 288 + 32; i++) lengths[i] = 0;
    while (index < total) {
        int sym;
        if (!decode(b, codeLen, sym)) { *err = "bad code-length symbol"; return false; }
        if (sym < 16) {
            lengths[index++] = (uint8_t)sym;
            continue;
        }
        uint8_t prev = 0;
        uint32_t rep, r;
        if (sym == 16) {
            if (index == 0) { *err = "repeat with no previous length"; return false; }
            prev = lengths[index - 1];
            if (!b.get(2, r)) { *err = "truncated block header"; return false; }
            rep = 3 + r;
        } else if (sym == 17) {
            if (!b.get(3, r)) { *err = "truncated block header"; return false; }
            rep = 3 + r;
        } else {
            if (!b.get(7, r)) { *err = "truncated block header"; return false; }
            rep = 11 + r;
        }
        if (index + rep > total) { *err = "code lengths overflow"; return false; }
        while (rep--) lengths[index++] = prev;
    }
    if (lengths[256] == 0) { *err = "block has no end-of-block code"; return false; }

    Huff lc, dc;
    int e = build(lc, lengths, (int)nlen);
    if (e < 0 || (e > 0 && (int)nlen - lc.count[0] != 1)) { *err = "bad literal/length code"; return false; }
    e = build(dc, lengths + nlen, (int)ndist);
    if (e < 0 || (e > 0 && (int)ndist - dc.count[0] != 1)) { *err = "bad distance code"; return false; }
    return codes(b, o, lc, dc, err);
}

// all blocks of one DEFLATE stream
bool raw_stream(Bits& b, Out& o, const char** err) {
    for (;;) {
        uint32_t last, type;
        if (!b.get(1, last) || !b.get(2, type)) { *err = "truncated data"; return false; }
        bool ok;
        if (type == 0) ok = stored_block(b, o, err);
        else if (type == 1) ok = fixed_block(b, o, err);
        else if (type == 2) ok = dynamic_block(b, o, err);
        else { *err = "invalid block type"; return false; }
        if (!ok) return false;
        if (last) return true;
    }
}

}  // namespace

void free_buffer(uint8_t* p) { mem_free(p); }

bool deflate_decompress(const uint8_t* in, size_t n, uint8_t*& out, size_t& outn, const char** err) {
    out = nullptr;
    outn = 0;
    Bits b{in, n};
    Out o;
    if (!raw_stream(b, o, err)) { mem_free(o.p); return false; }
    out = o.p;
    outn = o.n;
    return true;
}

bool zlib_decompress(const uint8_t* in, size_t n, uint8_t*& out, size_t& outn, const char** err) {
    out = nullptr;
    outn = 0;
    if (n < 6) { *err = "zlib stream too short"; return false; }
    uint8_t cmf = in[0], flg = in[1];
    if ((cmf & 0x0F) != 8 || (cmf >> 4) > 7) { *err = "not a zlib deflate stream"; return false; }
    if (((cmf << 8) | flg) % 31 != 0) { *err = "bad zlib header check"; return false; }
    if (flg & 0x20) { *err = "zlib preset dictionaries are not supported"; return false; }

    Bits b{in + 2, n - 2};
    Out o;
    if (!raw_stream(b, o, err)) { mem_free(o.p); return false; }

    // the checksum starts at the next byte boundary after the last block
    size_t at = 2 + b.pos;
    if (at + 4 > n) { mem_free(o.p); *err = "missing Adler-32 checksum"; return false; }
    uint32_t want = ((uint32_t)in[at] << 24) | ((uint32_t)in[at + 1] << 16) | ((uint32_t)in[at + 2] << 8) | in[at + 3];
    if (adler32(o.p, o.n) != want) { mem_free(o.p); *err = "Adler-32 checksum mismatch"; return false; }
    out = o.p;
    outn = o.n;
    return true;
}

uint32_t crc32_ieee(const uint8_t* data, size_t n) { return hash::crc32(data, n); }

uint32_t adler32(const uint8_t* data, size_t n) {
    uint32_t a = 1, b = 0;
    for (size_t i = 0; i < n; i++) {
        a += data[i];
        if (a >= 65521u) a -= 65521u;
        b += a;
        if (b >= 65521u) b -= 65521u;
    }
    return (b << 16) | a;
}

}}  // namespace evgx::inflate
