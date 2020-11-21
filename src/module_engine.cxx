#include "module.hxx"

#include <iostream>

namespace crankshaft_node {

Napi::Value craEngineNew(const Napi::CallbackInfo& info)
{
    Napi::HandleScope scope(info.Env());

    Module* module = static_cast<Module*>(info.Data());

    cra_engine_t engine = nullptr;

    // TODO(daaitch): status
    cra_engine_new(
        &engine,
        [](cra_callback_closure_t closure, void* user_data) {
            Module* module = static_cast<Module*>(user_data);
            module->EnqueueNodeCallback(closure);
            return CRA_KEEP_DATA;
        },
        module);

    CRAN_ASSERT(engine != nullptr);
    return Napi::External<icra_engine_s>::New(info.Env(), engine);
}

void craEngineInit(const Napi::CallbackInfo& info)
{
    Napi::HandleScope scope(info.Env());

    CRAN_CHECK_PARAM_ENGINE(info, 0);
    auto js_engine = CRAN_GET_PARAM_ENGINE(info, 0);

    CRAN_CHECK_PARAM_FUNCTION(info, 1);
    auto js_callback = CRAN_GET_PARAM_FUNCTION(info, 1);

    cra_engine_init(
        js_engine.Data(),
        [](cra_engine_t engine, cra_status status, void* data) {
            CRAN_ASSERT(engine != nullptr);
            CRAN_ASSERT(status == CRA_OK);
            CRAN_ASSERT(data != nullptr);

            auto closure = node_closure::Transfer(data);
            closure->Resolve(engine);
            std::cout << engine << ": init" << std::endl;
        },
        node_closure::Create(js_callback));
}

void craEngineDelete(const Napi::CallbackInfo& info)
{
    Napi::HandleScope scope(info.Env());

    CRAN_CHECK_PARAM_ENGINE(info, 0);
    auto engine = CRAN_GET_PARAM_ENGINE(info, 0);

    // TODO(daaitch): status
    cra_engine_delete(engine.Data());
}

void craEngineStop(const Napi::CallbackInfo& info)
{
    Napi::HandleScope scope(info.Env());

    CRAN_CHECK_PARAM_ENGINE(info, 0);
    auto engine = CRAN_GET_PARAM_ENGINE(info, 0);

    CRAN_CHECK_PARAM_FUNCTION(info, 1);
    auto callback = CRAN_GET_PARAM_FUNCTION(info, 1);

    // TODO(daaitch): status
    cra_engine_stop(
        engine.Data(),
        [](cra_engine_t engine, cra_status status, void* data) {
            CRAN_ASSERT(engine != nullptr);
            CRAN_ASSERT(status == CRA_OK);
            CRAN_ASSERT(data != nullptr);

            auto closure = node_closure::Transfer(data);

            closure->Resolve(engine);
            std::cout << engine << ": stopped" << std::endl;
        },
        node_closure::Create(callback));
}

void InitEngine(Napi::Env env, Napi::Object exports, Module* module)
{
    CRAN_EXPORTS(craEngineNew);
    CRAN_EXPORTS(craEngineInit);
    CRAN_EXPORTS(craEngineDelete);
    CRAN_EXPORTS(craEngineStop);
}

}