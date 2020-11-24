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
            CRAN_ASSERT(status == cra_ok);

            auto closure = node_closure::Transfer(data);

            closure->Resolve(window);
        },
        node_closure::Create(js_callback));
}

void craWindowDelete(const Napi::CallbackInfo& info)
{
    Napi::HandleScope scope(info.Env());

    CRAN_CHECK_PARAM_WINDOW(info, 0);
    auto js_window = CRAN_GET_PARAM_WINDOW(info, 0);

    CRAN_CHECK_PARAM_FUNCTION(info, 1);
    auto js_callback = CRAN_GET_PARAM_FUNCTION(info, 1);

    cra_window_delete(
        js_window.Data(), [](void* void_result, cra_status status, void* data) {
            CRAN_ASSERT(void_result == nullptr);
            CRAN_ASSERT(status == cra_ok);
            auto closure = node_closure::Transfer(data);
            closure->Resolve();
        },
        node_closure::Create(js_callback));
}

void craWindowLoadGL(const Napi::CallbackInfo& info)
{
    Napi::HandleScope scope(info.Env());

    CRAN_CHECK_PARAM_WINDOW(info, 0);
    auto js_window = CRAN_GET_PARAM_WINDOW(info, 0);

    CRAN_CHECK_PARAM_FUNCTION(info, 1);
    auto js_callback = CRAN_GET_PARAM_FUNCTION(info, 1);

    cra_window_load_gl(
        js_window.Data(), [](cra_window_t window, cra_status status, void* data) {
            CRAN_ASSERT(window != nullptr);
            CRAN_ASSERT(status == cra_ok || status == cra_err_lib);
            CRAN_ASSERT(data != nullptr);

            auto closure = node_closure::Transfer(data);

            closure->MaybeResolve(status);
        },
        node_closure::Create(js_callback));
}

void craWindowOnClose(const Napi::CallbackInfo& info)
{
    Napi::HandleScope scope(info.Env());

    CRAN_CHECK_PARAM_WINDOW(info, 0);
    auto js_window = CRAN_GET_PARAM_WINDOW(info, 0);

    CRAN_CHECK_PARAM_FUNCTION(info, 1);
    auto js_callback = CRAN_GET_PARAM_FUNCTION(info, 1);

    cra_window_on_close(
        js_window.Data(), [](cra_window_t window, void* data) {
            CRAN_ASSERT(window != nullptr);
            CRAN_ASSERT(data != nullptr);

            auto closure = node_closure::From(data);

            closure->Resolve();
        },
        node_closure::Create(js_callback));
}

void InitWindow(Napi::Env env, Napi::Object exports)
{
    CRAN_EXPORTS(craWindowNew);
    CRAN_EXPORTS(craWindowDelete);
    CRAN_EXPORTS(craWindowLoadGL);
    CRAN_EXPORTS(craWindowOnClose);
}

}
