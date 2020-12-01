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

cra_block_facetype face_type_by_string(const std::string& type)
{
    if (type == "left") {
        return cra_block_facetype::LEFT;
    }

    if (type == "right") {
        return cra_block_facetype::RIGHT;
    }

    if (type == "top") {
        return cra_block_facetype::TOP;
    }

    if (type == "bottom") {
        return cra_block_facetype::BOTTOM;
    }

    if (type == "front") {
        return cra_block_facetype::FRONT;
    }

    if (type == "back") {
        return cra_block_facetype::BACK;
    }

    return cra_block_facetype::UNKNOWN;
}

void craEngineBlockNew(const Napi::CallbackInfo& info)
{
    Napi::HandleScope scope(info.Env());

    CRAN_CHECK_PARAM_ENGINE(info, 0);
    auto js_engine = CRAN_GET_PARAM_ENGINE(info, 0);

    CRAN_CHECK_PARAM_OBJECT(info, 1);
    auto js_params = CRAN_GET_PARAM_OBJECT(info, 1);

    CRAN_CHECK_PARAM_FUNCTION(info, 2);
    auto js_callback = CRAN_GET_PARAM_FUNCTION(info, 2);

    std::string id = js_params.Get("id").ToString();

    auto js_faces = js_params.Get("faces").ToObject();
    auto js_faces_keys = js_faces.GetPropertyNames();

    std::vector<cra_block_face> faces;
    uint32_t keys_length = js_faces_keys.Length();
    faces.resize(keys_length);

    for (int i = 0; i < keys_length; i++) {
        auto js_face_key = js_faces_keys.Get(i).ToString();
        std::string face_key = js_face_key;
        auto js_face_data = js_faces.Get(js_face_key).As<Napi::ArrayBuffer>();
        size_t byte_length = js_face_data.ByteLength();

        CRAN_CHECK(info, byte_length % sizeof(cra_vao_element_block) == 0);
        int items = byte_length / sizeof(cra_vao_element_block);

        faces[i].elements = new cra_vao_element_block[items];
        std::memcpy(faces[i].elements, js_face_data.Data(), byte_length);

        faces[i].elements_length = items;
        faces[i].facetype = face_type_by_string(face_key);
    }

    cra_block_new_params params = {
        .id = id.c_str(),
        .opaque = js_params.Get("opaque").ToBoolean(),
        .texture_content = nullptr,
        .texture_content_length = 0,
        .faces = faces.data(),
        .faces_length = (int)faces.size()
    };

    if (js_params.Has("texture")) {
        auto js_texture = js_params.Get("texture").As<Napi::Buffer<unsigned char>>();
        params.texture_content = js_texture.Data();
        params.texture_content_length = (int)js_texture.ByteLength();
    }

    cra_engine_block_new(
        js_engine.Data(), &params, [](cra_engine_t engine, cra_status status, void* data) {
            auto closure = node_closure::Transfer(data);
            closure->MaybeResolve(status, engine);
        },
        node_closure::Create(js_callback));

    std::for_each(faces.begin(), faces.end(), [](cra_block_face& face) {
        delete[] face.elements;
    });
}

void InitEngine(Napi::Env env, Napi::Object exports)
{
    CRAN_EXPORTS(craEngineNew);
    CRAN_EXPORTS(craEngineInit);
    CRAN_EXPORTS(craEngineDelete);
    CRAN_EXPORTS(craEngineStop);
    CRAN_EXPORTS(craEngineBlockNew);
}

}