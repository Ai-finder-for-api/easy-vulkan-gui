// Strings and UTF-8: views, trimming, splitting, number parsing and formatting, code point encode/decode and validation.
#include <string.h>
#include "../common/check.hpp"
#include <evg/common/str.hpp>
#include <evg/common/utf8.hpp>

using namespace evgx;

int main() {
    // views and comparison
    str::View s("hello, world");
    CHECK(s.size == 12);
    CHECK(str::equal(s.sub(0, 5), "hello"));
    CHECK(str::starts_with(s, str::View("hello")) && !str::starts_with(s, str::View("world")));
    CHECK(str::ends_with(s, str::View("world")) && !str::ends_with(s, str::View("hello")));
    CHECK(str::find(s, ',') == 5 && str::find(s, 'z') == (size_t)-1);
    CHECK(str::equal(s.sub(100), "") && str::equal(s.sub(7, 999), "world"));

    // trimming
    CHECK(str::equal(str::trim(str::View("  \t hi there \r\n")), "hi there"));
    CHECK(str::trim(str::View("   ")).empty());

    // splitting keeps empty fields
    int fields = 0;
    size_t total = 0;
    str::split(str::View("a,,bc,"), ',', [&](str::View piece) { fields++; total += piece.size; });
    CHECK(fields == 4 && total == 3);                       // a, "", bc, "": 1 + 0 + 2 + 0 characters

    // integers
    int64_t i = 0;
    CHECK(str::to_i64(str::View("  -42 "), i) && i == -42);
    CHECK(str::to_i64(str::View("+7"), i) && i == 7);
    CHECK(str::to_i64(str::View("9223372036854775807"), i) && i == INT64_MAX);
    CHECK(str::to_i64(str::View("-9223372036854775808"), i) && i == INT64_MIN);
    CHECK(!str::to_i64(str::View("9223372036854775808"), i));   // overflow
    CHECK(!str::to_i64(str::View("12x"), i));
    CHECK(!str::to_i64(str::View(""), i) && !str::to_i64(str::View("-"), i));

    // decimals
    double d = 0;
    CHECK(str::to_f64(str::View("3.25"), d) && d == 3.25);
    CHECK(str::to_f64(str::View("-0.5e2"), d) && d == -50.0);
    CHECK(str::to_f64(str::View(".5"), d) && d == 0.5);
    CHECK(!str::to_f64(str::View("1e"), d));
    CHECK(!str::to_f64(str::View("abc"), d));

    // formatting
    char buf[32];
    CHECK(str::format_i64(-1203, buf, sizeof buf) == 5 && strcmp(buf, "-1203") == 0);
    CHECK(str::format_i64(0, buf, sizeof buf) == 1 && strcmp(buf, "0") == 0);
    CHECK(str::format_i64(12345, buf, 4) == 0);            // does not fit in 4 bytes incl. NUL

    // UTF-8: ASCII, two-, three- and four-byte sequences
    const uint8_t text[] = {'a', 0xC3, 0xA9, 0xE2, 0x82, 0xAC, 0xF0, 0x9F, 0x98, 0x80};   // a, é, €, 😀
    size_t i2 = 0;
    CHECK(utf8::decode(text, sizeof text, i2) == 'a' && i2 == 1);
    CHECK(utf8::decode(text, sizeof text, i2) == 0xE9 && i2 == 3);
    CHECK(utf8::decode(text, sizeof text, i2) == 0x20AC && i2 == 6);
    CHECK(utf8::decode(text, sizeof text, i2) == 0x1F600 && i2 == 10);
    CHECK(utf8::count(text, sizeof text) == 4);
    CHECK(utf8::valid(text, sizeof text));

    // encoding round trips
    uint8_t out[4];
    CHECK(utf8::encode(0x1F600, out) == 4 && out[0] == 0xF0);
    CHECK(utf8::encode(0xD800, out) == 0);                 // surrogates are not encodable
    CHECK(utf8::encode(0x110000, out) == 0);               // beyond Unicode

    // invalid input: lone continuation byte, overlong encoding, truncated sequence
    const uint8_t bad1[] = {0x80};
    const uint8_t bad2[] = {0xC0, 0xAF};                   // overlong '/'
    const uint8_t bad3[] = {0xE2, 0x82};                   // truncated
    size_t k = 0;
    CHECK(utf8::decode(bad1, 1, k) == 0xFFFD && k == 1);
    k = 0;
    CHECK(utf8::decode(bad2, 2, k) == 0xFFFD && k == 1);
    CHECK(!utf8::valid(bad1, 1) && !utf8::valid(bad2, 2) && !utf8::valid(bad3, 2));
    const uint8_t replacement[] = {0xEF, 0xBF, 0xBD};     // a real U+FFFD is valid
    CHECK(utf8::valid(replacement, 3));

    return evgtest::finish("common/str_utf8");
}
