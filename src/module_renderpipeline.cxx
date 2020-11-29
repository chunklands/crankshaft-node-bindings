#include "module.hxx"
#include <iostream>

namespace crankshaft_node {

void craRenderpipelineInit(const Napi::CallbackInfo& info)
{
    Napi::HandleScope scope(info.Env());

    CRAN_CHECK_PARAM_WINDOW(info, 0);
    auto js_window = CRAN_GET_PARAM_WINDOW(info, 0);

    CRAN_CHECK_PARAM_OBJECT(info, 1);
    auto js_params = CRAN_GET_PARAM_OBJECT(info, 1);

    CRAN_CHECK_PARAM_FUNCTION(info, 2);
    auto js_callback = CRAN_GET_PARAM_FUNCTION(info, 2);

    std::string gbuffer_vertex_shader = js_params.Get("gbufferVertexShader").ToString();
    std::string gbuffer_fragment_shader = js_params.Get("gbufferFragmentShader").ToString();
    std::string lighting_vertex_shader = js_params.Get("lightingVertexShader").ToString();
    std::string lighting_fragment_shader = js_params.Get("lightingFragmentShader").ToString();
    std::string selectblock_vertex_shader = js_params.Get("selectblockVertexShader").ToString();
    std::string selectblock_fragment_shader = js_params.Get("selectblockFragmentShader").ToString();
    std::string sprite_vertex_shader = js_params.Get("spriteVertexShader").ToString();
    std::string sprite_fragment_shader = js_params.Get("spriteFragmentShader").ToString();
    std::string text_vertex_shader = js_params.Get("textVertexShader").ToString();
    std::string text_fragment_shader = js_params.Get("textFragmentShader").ToString();

    cra_renderpipeline_init(
        js_window.Data(), {
            .gbuffer_vertex_shader = gbuffer_vertex_shader.c_str(),
            .gbuffer_fragment_shader = gbuffer_fragment_shader.c_str(),
            .lighting_vertex_shader = lighting_vertex_shader.c_str(),
            .lighting_fragment_shader = lighting_fragment_shader.c_str(),
            .selectblock_vertex_shader = selectblock_vertex_shader.c_str(),
            .selectblock_fragment_shader = selectblock_fragment_shader.c_str(),
            .sprite_vertex_shader = sprite_vertex_shader.c_str(),
            .sprite_fragment_shader = sprite_fragment_shader.c_str(),
            .text_vertex_shader = text_vertex_shader.c_str(),
            .text_fragment_shader = text_fragment_shader.c_str(),
        },
        [](cra_window_t window, cra_status status, void* data) {
            CRAN_ASSERT(window != nullptr);

            auto closure = node_closure::Transfer(data);

            closure->MaybeResolve(status);
        },
        node_closure::Create(js_callback));
}

void InitRenderpipeline(Napi::Env env, Napi::Object exports)
{
    CRAN_EXPORTS(craRenderpipelineInit);
}

}
