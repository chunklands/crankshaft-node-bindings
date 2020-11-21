#include "module.hxx"
#include <iostream>

namespace crankshaft_node {

void craWindowNew(const Napi::CallbackInfo& info)
{
    Napi::HandleScope scope(info.Env());

    CRAN_CHECK_PARAM_ENGINE(info, 0);
    auto js_engine = CRAN_GET_PARAM_ENGINE(info, 0);

    CRAN_CHECK_PARAM_OBJECT(info, 1);
    auto js_params = CRAN_GET_PARAM_OBJECT(info, 1);

    CRAN_CHECK_PARAM_FUNCTION(info, 2);
    auto js_callback = CRAN_GET_PARAM_FUNCTION(info, 2);

    std::string title = js_params.Get("title").ToString();

    cra_window_new_params_s params
        = { .width = js_params.Get("width").ToNumber(),
              .height = js_params.Get("height").ToNumber(),
              .title = title.data() };

    // TODO(daaitch): status
    cra_window_new(
        js_engine.Data(), &params,
        [](cra_window_t window, cra_status status, void* data) {
            CRAN_ASSERT(window != nullptr);
            CRAN_ASSERT(status == CRA_OK);

            auto closure = node_closure::Transfer(data);

            std::cout << window << ": created window" << std::endl;
            closure->Resolve(window);
        },
        node_closure::Create(js_callback));
}

void craWindowLoadGL(const Napi::CallbackInfo& info)
{
    Napi::HandleScope scope(info.Env());

    CRAN_CHECK_PARAM_ENGINE(info, 0);
    auto js_window = CRAN_GET_PARAM_WINDOW(info, 0);

    CRAN_CHECK_PARAM_FUNCTION(info, 1);
    auto js_callback = CRAN_GET_PARAM_FUNCTION(info, 1);

    cra_window_load_gl(
        js_window.Data(), [](cra_window_t window, cra_status status, void* data) {
            CRAN_ASSERT(window != nullptr);
            CRAN_ASSERT(status == CRA_OK || status == CRA_ERR_LIB);
            CRAN_ASSERT(data != nullptr);

            auto closure = node_closure::Transfer(data);

            closure->MaybeResolve(status);
        },
        node_closure::Create(js_callback));
}

void InitWindow(Napi::Env env, Napi::Object exports, Module* module)
{
    CRAN_EXPORTS(craWindowNew);
    CRAN_EXPORTS(craWindowLoadGL);
}

}
