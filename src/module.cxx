#include "module.hxx"
#include <iostream>

namespace crankshaft_node {

Module::Module(Napi::Env env)
{
    tsfn_ = Napi::ThreadSafeFunction::New(env,
        Napi::Function::New(
            env, [](const Napi::CallbackInfo&) {}, "dummy"),
        "EngineBridgeCallback",
        0,
        1);

    main_thread_ = std::this_thread::get_id();

    cra_init([](int status, void*) {
        assert(status == cra_ok);
    },
        nullptr);
}

Module::~Module()
{
    cra_destroy();
}

void Module::EnqueueNodeCallback(cra_ulc_closure_t closure)
{
    assert(std::this_thread::get_id() != main_thread_);

    tsfn_.NonBlockingCall(closure, [](Napi::Env env, Napi::Function, cra_ulc_closure_t closure) {
        // TODO(daaitch): check status cra_ok or fatal
        cra_closure_execute_and_finalize(closure);
    });
}

node_closure* node_closure::Create(Napi::Function callback)
{
    node_closure* closure = new node_closure();
    closure->ref = Napi::Persistent(callback);

    return closure;
}

std::unique_ptr<node_closure> node_closure::Transfer(void* data)
{
    return std::unique_ptr<node_closure>(static_cast<node_closure*>(data));
}

} // crankshaft_node

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    using namespace crankshaft_node;

    Module* module = env.GetInstanceData<Module>();
    if (module == nullptr) {
        module = new Module(env);
        env.SetInstanceData(module);
    }

    InitEngine(env, exports, module);
    InitWindow(env, exports, module);

    return exports;
}

NODE_API_MODULE(hello, Init)
