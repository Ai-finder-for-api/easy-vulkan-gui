// evg_info: inspect a .glb file. Usage: evg_info [--json] [--log] model.glb [more.glb ...]
#include <stdio.h>
#include <evg/evg.hpp>
#include <evg/common/args.hpp>
#include <evg/common/file.hpp>
#include <evg/common/timer.hpp>

using namespace evgx;

static void print_model(const char* path, const Model* m, bool json, double ms) {
    int tris = 0;
    for (int i = 0; i < m->ninst; i++) tris += (int)(m->prims[m->inst[i].prim].nidx / 3);
    long long bytes = file::size(path);
    if (json) {
        printf("{\"file\":\"%s\",\"bytes\":%lld,\"primitives\":%d,\"instances\":%d,\"triangles\":%d,"
               "\"materials\":%d,\"skipped\":%d,\"load_ms\":%.3f,\"bounds\":[%.4f,%.4f,%.4f,%.4f,%.4f,%.4f]}\n",
               path, bytes, m->nprims, m->ninst, tris, m->nmats - 1, m->skipped, ms,
               m->bmin.x, m->bmin.y, m->bmin.z, m->bmax.x, m->bmax.y, m->bmax.z);
    } else {
        printf("%s\n", path);
        printf("  size        %lld bytes\n", bytes);
        printf("  primitives  %d (skipped %d)\n", m->nprims, m->skipped);
        printf("  instances   %d\n", m->ninst);
        printf("  triangles   %d\n", tris);
        printf("  materials   %d\n", m->nmats - 1);
        printf("  load time   %.3f ms\n", ms);
        printf("  bounds      (%.3f %.3f %.3f) .. (%.3f %.3f %.3f)\n",
               m->bmin.x, m->bmin.y, m->bmin.z, m->bmax.x, m->bmax.y, m->bmax.z);
    }
}

int main(int argc, char** argv) {
    cli::Args args(argc, argv);
    if (args.positional_count() == 0 || args.has("help")) {
        printf("usage: evg_info [--json] model.glb [more.glb ...]\n");
        return args.has("help") ? 0 : 2;
    }
    bool json = args.has("json");
    int failed = 0;
    for (int i = 0; i < args.positional_count(); i++) {
        const char* path = args.positional(i);
        timer::Stopwatch sw;
        Model* m = evg.loadglb(path);
        double ms = sw.millis();
        if (!m) {
            fprintf(stderr, "%s: %s\n", path, evg.error());
            failed++;
            continue;
        }
        print_model(path, m, json, ms);
        evg.freemodel(m);
    }
    return failed ? 1 : 0;
}
