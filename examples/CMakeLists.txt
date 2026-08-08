# ── Examples ──────────────────────────────────────────────
cmake_minimum_required(VERSION 3.21)

function(add_example name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE vgui::vgui)
    set_target_properties(${name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples)
endfunction()

add_example(hello_window)
add_example(ui_demo)
add_example(hello_triangle)
add_example(textured_cube)
add_example(model_loading)
