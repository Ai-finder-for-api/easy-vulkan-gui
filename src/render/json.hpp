// Tiny JSON tokenizer (jsmn-style): tokens in depth-first order, strings are not unescaped. No allocation here.
#pragma once
#include <stddef.h>
namespace evgx { namespace json {

enum { OBJ = 0, ARR = 1, STR = 2, PRIM = 3 };
struct Tok { int type, start, end, size; };      // size = #pairs (object) / #items (array)
struct Doc { const char* s; int len; Tok* t; int n; int cap; };

namespace detail {
struct P { const char* s; int len; int pos; Tok* t; int n; int cap; bool err; };
inline void ws(P& p) { while (p.pos < p.len) { char c = p.s[p.pos]; if (c == ' ' || c == '\n' || c == '\r' || c == '\t') p.pos++; else break; } }
inline int newtok(P& p, int type, int start) {
    if (p.n >= p.cap) { p.err = true; return -1; }
    Tok& k = p.t[p.n]; k.type = type; k.start = start; k.end = start; k.size = 0;
    return p.n++;
}
inline bool str(P& p) {
    int start = ++p.pos;
    while (p.pos < p.len) {
        char c = p.s[p.pos];
        if (c == '\\') { p.pos += 2; continue; }
        if (c == '"') { int t = newtok(p, STR, start); if (t < 0) return false; p.t[t].end = p.pos; p.pos++; return true; }
        p.pos++;
    }
    p.err = true; return false;
}
inline bool value(P& p, int depth) {
    if (depth > 64) { p.err = true; return false; }
    ws(p);
    if (p.pos >= p.len) { p.err = true; return false; }
    char c = p.s[p.pos];
    if (c == '{' || c == '[') {
        bool obj = c == '{'; char close = obj ? '}' : ']';
        int t = newtok(p, obj ? OBJ : ARR, p.pos); if (t < 0) return false;
        p.pos++; ws(p);
        int cnt = 0;
        if (p.pos < p.len && p.s[p.pos] == close) { p.pos++; p.t[t].end = p.pos; return true; }
        for (;;) {
            ws(p);
            if (obj) {
                if (p.pos >= p.len || p.s[p.pos] != '"') { p.err = true; return false; }
                if (!str(p)) return false;
                ws(p);
                if (p.pos >= p.len || p.s[p.pos] != ':') { p.err = true; return false; }
                p.pos++;
            }
            if (!value(p, depth + 1)) return false;
            cnt++; ws(p);
            if (p.pos >= p.len) { p.err = true; return false; }
            if (p.s[p.pos] == ',') { p.pos++; continue; }
            if (p.s[p.pos] == close) { p.pos++; break; }
            p.err = true; return false;
        }
        p.t[t].size = cnt; p.t[t].end = p.pos;
        return true;
    }
    if (c == '"') return str(p);
    int start = p.pos;
    while (p.pos < p.len) { char d = p.s[p.pos]; if (d == ',' || d == '}' || d == ']' || d == ' ' || d == '\n' || d == '\r' || d == '\t') break; p.pos++; }
    if (p.pos == start) { p.err = true; return false; }
    int t = newtok(p, PRIM, start); if (t < 0) return false;
    p.t[t].end = p.pos;
    return true;
}
}

// tokens: caller-provided array with room for `cap` tokens (len/2 + 8 is always enough)
inline bool parse(const char* s, int len, Tok* tokens, int cap, Doc* out) {
    detail::P p = {s, len, 0, tokens, 0, cap, false};
    if (!detail::value(p, 0) || p.err) return false;
    out->s = s; out->len = len; out->t = tokens; out->n = p.n; out->cap = cap;
    return true;
}

inline int skip(const Doc& d, int i) {
    const Tok& k = d.t[i];
    if (k.type == OBJ) { int j = i + 1; for (int c = 0; c < k.size; c++) { j = skip(d, j); j = skip(d, j); } return j; }
    if (k.type == ARR) { int j = i + 1; for (int c = 0; c < k.size; c++) j = skip(d, j); return j; }
    return i + 1;
}
inline bool streq(const Doc& d, int i, const char* lit) {
    if (i < 0) return false;
    int n = d.t[i].end - d.t[i].start, j = 0;
    for (; j < n; j++) if (!lit[j] || lit[j] != d.s[d.t[i].start + j]) return false;
    return lit[j] == 0;
}
inline int get(const Doc& d, int obj, const char* key) {       // value token of obj[key], or -1
    if (obj < 0 || d.t[obj].type != OBJ) return -1;
    int j = obj + 1;
    for (int c = 0; c < d.t[obj].size; c++) { int v = j + 1; if (streq(d, j, key)) return v; j = skip(d, v); }
    return -1;
}
inline int item(const Doc& d, int arr, int idx) {               // idx-th element of an array, or -1
    if (arr < 0 || d.t[arr].type != ARR || idx < 0 || idx >= d.t[arr].size) return -1;
    int j = arr + 1;
    for (int c = 0; c < idx; c++) j = skip(d, j);
    return j;
}
inline int count(const Doc& d, int arr) { return (arr >= 0 && d.t[arr].type == ARR) ? d.t[arr].size : 0; }
inline double number(const Doc& d, int i, double def = 0) {
    if (i < 0 || d.t[i].type != PRIM) return def;
    const char* s = d.s + d.t[i].start; const char* e = d.s + d.t[i].end;
    bool neg = false; if (s < e && *s == '-') { neg = true; s++; } else if (s < e && *s == '+') s++;
    if (s >= e || !((*s >= '0' && *s <= '9') || *s == '.')) return def;
    double v = 0;
    while (s < e && *s >= '0' && *s <= '9') v = v * 10 + (*s++ - '0');
    if (s < e && *s == '.') { s++; double f = 0.1; while (s < e && *s >= '0' && *s <= '9') { v += (*s++ - '0') * f; f *= 0.1; } }
    if (s < e && (*s == 'e' || *s == 'E')) {
        s++; bool eneg = false; if (s < e && *s == '-') { eneg = true; s++; } else if (s < e && *s == '+') s++;
        int ex = 0; while (s < e && *s >= '0' && *s <= '9') { if (ex < 100000) ex = ex * 10 + (*s - '0'); s++; }   // capped: no int overflow
        if (ex > 308) ex = 308;
        double m = 1; for (int k = 0; k < ex; k++) m *= 10;
        v = eneg ? v / m : v * m;
    }
    return neg ? -v : v;
}
inline bool boolean(const Doc& d, int i, bool def = false) {
    if (i < 0 || d.t[i].type != PRIM) return def;
    return streq(d, i, "true") ? true : (streq(d, i, "false") ? false : def);
}
}} // namespace evgx::json
