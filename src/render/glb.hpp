#pragma once
#include <stddef.h>
#include <stdint.h>
#include <evg/scene.hpp>
namespace evgx {
Model* glb_parse(const uint8_t* data, size_t size, const char** err);   // null + *err on failure
void   model_free(Model* m);
}
