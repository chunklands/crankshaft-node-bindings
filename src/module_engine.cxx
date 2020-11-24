#include "module.hxx"

#include <iostream>

namespace crankshaft_node {

Napi::Value craEngineNew(const Napi::CallbackInfo& info)
{
    Napi::HandleScope scope(info.Env());

    cra_engine_t engine = nullptr;
    Module* module = new Module(info.Env());

    // TODO(daaitch): status
    cra_engine_new(
        &engine,
        [](cra_ulc_closure_t closure, void* data) {
            Module* module = static_cast<Module*>(data);
            module->EnqueueNodeCallback(closure);
            return cra_keep_data;
        },
        module);

    CRAN_ASSERT(engine != nullptr);
    cra_engine_set_user_pointer(engine, module);

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
            CRAN_ASSERT(status == cra_ok);
            CRAN_ASSERT(data != nullptr);

            auto closure = node_closure::Transfer(data);
            closure->Resolve(engine);
        },
        node_closure::Create(js_callback));
}

void craEngineDelete(const Napi::CallbackInfo& info)
{
    Napi::HandleScope scope(info.Env());

    // TODO(daaitch): check engine_instances, naked pointers are unsafe :(
    CRAN_CHECK_PARAM_ENGINE(info, 0);
    auto js_engine = CRAN_GET_PARAM_ENGINE(info, 0);

    cra_engine_t engine = js_engine.Data();

    Module* module = nullptr;
    cra_engine_get_user_pointer(engine, (void**)&module);
    CRAN_ASSERT(module != nullptr);

    delete module;
    cra_engine_set_user_pointer(engine, nullptr);

    // TODO(daaitch): status
    cra_engine_delete(engine);
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
            CRAN_ASSERT(status == cra_ok);
            CRAN_ASSERT(data != nullptr);

            auto closure = node_closure::Transfer(data);

            closure->Resolve(engine);
        },
        node_closure::Create(callback));
}

void InitEngine(Napi::Env env, Napi::Object exports)
{
    CRAN_EXPORTS(craEngineNew);
    CRAN_EXPORTS(craEngineInit);
    CRAN_EXPORTS(craEngineDelete);
    CRAN_EXPORTS(craEngineStop);
}

}