// Command-line arguments.
//   --flag              boolean option (no value)
//   --key=value         option with a value
//   --key value         option with a value, unless `key` is declared as a boolean flag
//   -x                  short boolean option
//   --                  everything after it is positional
// Pass the boolean flags to the constructor so `--json model.glb` keeps model.glb as a positional argument.
// No C library calls: uses the library's own string helpers.
#pragma once
#include <stddef.h>
#include <evg/common/str.hpp>

namespace evgx { namespace cli {

class Args {
public:
    static constexpr int kMax = 64;

    // every option takes a value when it is followed by a token that does not start with '-'
    Args(int argc, const char* const* argv) : Args(argc, argv, nullptr, 0) {}

    // `flags` names the options that never take a value; the array must outlive the Args object
    template <size_t N>
    Args(int argc, const char* const* argv, const char* const (&flags)[N]) : Args(argc, argv, flags, (int)N) {}

    bool has(const char* name) const { return find(name) >= 0; }

    // value of --name=value / --name value, or `fallback` when the option is absent or has no value
    const char* get(const char* name, const char* fallback = nullptr) const {
        int i = find(name);
        if (i < 0 || !vals_[i]) return fallback;
        return vals_[i];
    }

    int get_int(const char* name, int fallback = 0) const {
        const char* v = get(name, nullptr);
        int64_t out = 0;
        return (v && str::to_i64(str::View(v), out)) ? (int)out : fallback;
    }

    double get_double(const char* name, double fallback = 0.0) const {
        const char* v = get(name, nullptr);
        double out = 0.0;
        return (v && str::to_f64(str::View(v), out)) ? out : fallback;
    }

    int positional_count() const { return npos_; }
    const char* positional(int i) const { return (i >= 0 && i < npos_) ? pos_[i] : nullptr; }

private:
    Args(int argc, const char* const* argv, const char* const* flags, int nflags) : flags_(flags), nflags_(nflags) {
        bool options = true;
        for (int i = 1; i < argc; i++) {
            const char* a = argv[i];
            size_t alen = len(a);
            if (options && alen == 2 && a[0] == '-' && a[1] == '-') { options = false; continue; }
            if (options && alen > 2 && a[0] == '-' && a[1] == '-') {
                const char* body = a + 2;
                size_t blen = alen - 2;
                size_t eq = str::find(str::View(body, blen), '=');
                if (eq != (size_t)-1) add(body, eq, body + eq + 1);
                else if (!is_flag(body, blen) && i + 1 < argc && argv[i + 1][0] != '-') add(body, blen, argv[++i]);
                else add(body, blen, nullptr);
            } else if (options && alen == 2 && a[0] == '-') {
                add(a + 1, 1, nullptr);                              // -v
            } else if (npos_ < kMax) {
                pos_[npos_++] = a;
            }
        }
    }

    static size_t len(const char* s) { size_t n = 0; while (s[n]) n++; return n; }

    bool is_flag(const char* name, size_t n) const {
        for (int i = 0; i < nflags_; i++) if (str::equal(str::View(name, n), str::View(flags_[i]))) return true;
        return false;
    }

    void add(const char* name, size_t n, const char* value) {
        if (nopt_ >= kMax || n >= 64) return;
        for (size_t k = 0; k < n; k++) names_[nopt_][k] = name[k];
        names_[nopt_][n] = 0;
        vals_[nopt_] = value;
        nopt_++;
    }

    int find(const char* name) const {
        for (int i = 0; i < nopt_; i++) if (str::equal(str::View(names_[i]), str::View(name))) return i;
        return -1;
    }

    const char* const* flags_ = nullptr;
    int nflags_ = 0;
    char names_[kMax][64] = {};
    const char* vals_[kMax] = {};
    int nopt_ = 0;
    const char* pos_[kMax] = {};
    int npos_ = 0;
};

}}  // namespace evgx::cli
