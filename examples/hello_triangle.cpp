// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui example – Hello Triangle                                  ║
// ║  Renders a coloured triangle using the low-level graphics API.  ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/vgui.hpp>

int main() {
    auto& win = vgui::make_screen("Hello Triangle", 800, 600);

    // Vertices in NDC space
    std::vector<vgui::Vertex3D> verts = {
        {{ 0.0f,  0.5f, 0.0f }, {0,0,1}, {0.5f, 0.0f}},
        {{ 0.5f, -0.5f, 0.0f }, {0,0,1}, {1.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.0f }, {0,0,1}, {0.0f, 1.0f}},
    };
    auto mesh = vgui::Mesh::create(verts);

    // Shaders (paths relative to executable; pre-compiled SPIR-V)
    auto vert = vgui::Shader::load_file("shaders/triangle.vert.spv", vgui::ShaderStage::Vertex);
    auto frag = vgui::Shader::load_file("shaders/triangle.frag.spv", vgui::ShaderStage::Fragment);

    vgui::PipelineDesc pd;
    pd.vertex_shader   = vert;
    pd.fragment_shader = frag;
    pd.depth.test_enable = false;
    auto pipeline = vgui::Pipeline::create(pd);

    auto cmd = vgui::CommandBuffer::create();

    vgui::Application::get().on_update = [&](float) {
        auto& r = vgui::Renderer::get();
        if (!r.begin_frame()) return;

        cmd->begin();
        cmd->begin_pass(/* default render pass to swapchain */);
        cmd->bind_pipeline(*pipeline);
        cmd->bind_vertex(mesh->vertex_buffer());
        cmd->draw(3);
        cmd->end_pass();
        cmd->end();
        cmd->submit();

        r.end_frame();
    };

    vgui::run();
}
