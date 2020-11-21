#ifndef __CRANKSHAFTNODEBINDING_MODULE_HXX__
#define __CRANKSHAFTNODEBINDING_MODULE_HXX__

#include <crankshaft.h>
#include <memory>
#include <napi.h>
#include <thread>
#include <variant>

#define CRAN_ASSERT(expr) assert(expr)

#define CRAN_CHECK_PARAM(info, n, predicate, what, ...)          \
    do {                                                         \
        if (!predicate((info)[(n)])) {                           \
            Napi::Error::New(                                    \
                (info).Env(),                                    \
                "expected parameter #" #n " to be \"" what "\"") \
                .ThrowAsJavaScriptException();                   \
            return __VA_ARGS__;                                  \
        }                                                        \
    } while (0)

#define CRAN_CHECK_PARAM_EXTERNAL(info, n, what, ...) \
    CRAN_CHECK_PARAM((info), (n), crankshaft_node::is_external, what, __VA_ARGS__)

#define CRAN_CHECK_PARAM_FUNCTION(info, n, ...) \
    CRAN_CHECK_PARAM((info), (n), crankshaft_node::is_function, "a function", __VA_ARGS__)

#define CRAN_CHECK_PARAM_ENGINE(info, n, ...) \
    CRAN_CHECK_PARAM_EXTERNAL((info), (n), "an engine", __VA_ARGS__)

#define CRAN_CHECK_PARAM_WINDOW(info, n, ...) \
    CRAN_CHECK_PARAM_EXTERNAL((info), (n), "a window", __VA_ARGS__)

#define CRAN_CHECK_PARAM_OBJECT(info, n, ...) \
    CRAN_CHECK_PARAM((info), (n), crankshaft_node::is_object, "an object", __VA_ARGS__)

#define CRAN_GET_PARAM_ENGINE(info, n) \
    (info)[(n)].As<Napi::External<icra_engine_s>>()

#define CRAN_GET_PARAM_WINDOW(info, n) \
    (info)[(n)].As<Napi::External<icra_window_s>>()

#define CRAN_GET_PARAM_FUNCTION(info, n) \
    (info)[(n)].As<Napi::Function>()

#define CRAN_GET_PARAM_OBJECT(info, n) \
    (info)[(n)].As<Napi::Object>()

#define CRAN_EXPORTS(name)                                                \
    do {                                                                  \
        exports[#name] = Napi::Function::New(env, (name), #name, module); \
    } while (0)

namespace crankshaft_node {

inline bool is_external(Napi::Value value)
{
    return value.IsExternal();
}

inline bool is_function(Napi::Value value)
{
    return value.IsFunction();
}

inline bool is_object(Napi::Value value)
{
    return value.IsObject() && !value.IsNull();
}

class Module {
public:
    explicit Module(Napi::Env env);
    ~Module();

    void EnqueueNodeCallback(cra_callback_closure_t closure);

private:
    Napi::ThreadSafeFunction tsfn_;
    std::thread::id main_thread_;
};

struct node_closure {
    static node_closure* Create(Napi::Function callback);
    static std::unique_ptr<node_closure> Transfer(void* data);

    Napi::Env Env() const
    {
        return ref.Env();
    }

    Napi::Value Null() const
    {
        return ref.Env().Null();
    }

    void Resolve(Napi::Value result)
    {
        auto null = ref.Env().Null();
        ref.Call(null, { null, result });
    }

    template <class T>
    void Resolve(T* result)
    {
        Resolve(Napi::External<T>::New(ref.Env(), result));
    }

    void Resolve(const std::monostate&)
    {
        auto null = ref.Env().Null();
        ref.Call(null, { null });
    }

    void Reject(Napi::Error error)
    {
        auto null = ref.Env().Null();
        ref.Call(null, { error.Value(), null });
    }

    void Reject(cra_status status)
    {
        CRAN_ASSERT(status != CRA_OK);

        std::string message = "crankshaft status error: ";
        message += std::to_string(status);

        Reject(Napi::Error::New(ref.Env(), std::move(message)));
    }

    template <class T>
    void MaybeResolve(cra_status status, T&& result)
    {
        if (status != CRA_OK) {
            Reject(status);
        } else {
            Resolve(std::forward<T>(result));
        }
    }

    void MaybeResolve(cra_status status)
    {
        MaybeResolve(status, std::monostate());
    }

    Napi::FunctionReference ref;
};

void InitEngine(Napi::Env env, Napi::Object exports, Module* module);
void InitWindow(Napi::Env env, Napi::Object exports, Module* module);

} // namespace crankshaft_node

#endif // __CRANKSHAFTNODEBINDING_MODULE_HXX__