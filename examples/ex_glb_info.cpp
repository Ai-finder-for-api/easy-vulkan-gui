// ex_glb_info: load a .glb and print what it contains (primitives, instances, materials, bounds).
#include <stdio.h>
#include <evg/evg.hpp>

int main(int argc, char** argv) {
    if (argc < 2) { printf("usage: ex_glb_info model.glb\n"); return 2; }
    evgx::Model* m = evg.loadglb(argv[1]);
    if (!m) { printf("load failed: %s\n", evg.error()); return 1; }

    int tris = 0;
    for (int i = 0; i < m->ninst; i++) tris += (int)(m->prims[m->inst[i].prim].nidx / 3);
    printf("file        %s\n", argv[1]);
    printf("primitives  %d (skipped %d)\n", m->nprims, m->skipped);
    printf("instances   %d\n", m->ninst);
    printf("triangles   %d\n", tris);
    printf("materials   %d (+1 default)\n", m->nmats - 1);
    printf("bounds      (%.3f %.3f %.3f) .. (%.3f %.3f %.3f)\n",
           m->bmin.x, m->bmin.y, m->bmin.z, m->bmax.x, m->bmax.y, m->bmax.z);
    evg.freemodel(m);
    return 0;
}
