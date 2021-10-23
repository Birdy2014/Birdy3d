#include "utils/serializer/Adapter.hpp"

namespace Birdy3d::serializer {

    /* --- Save --- */

    template <>
    std::unique_ptr<Value> adapter_save(std::string& value) {
        return std::make_unique<String>(value);
    }

    template <>
    std::unique_ptr<Value> adapter_save(bool& value) {
        return std::make_unique<Bool>(value);
    }

    template <>
    std::unique_ptr<Value> adapter_save(glm::vec2& value) {
        auto object = std::make_unique<Object>();
        object->value["x"] = std::make_unique<Number>(value.x);
        object->value["y"] = std::make_unique<Number>(value.y);
        return object;
    }

    template <>
    std::unique_ptr<Value> adapter_save(glm::vec3& value) {
        auto object = std::make_unique<Object>();
        object->value["x"] = std::make_unique<Number>(value.x);
        object->value["y"] = std::make_unique<Number>(value.y);
        object->value["z"] = std::make_unique<Number>(value.z);
        return object;
    }

    template <>
    std::unique_ptr<Value> adapter_save(glm::vec4& value) {
        auto object = std::make_unique<Object>();
        object->value["x"] = std::make_unique<Number>(value.x);
        object->value["y"] = std::make_unique<Number>(value.y);
        object->value["z"] = std::make_unique<Number>(value.z);
        object->value["w"] = std::make_unique<Number>(value.w);
        return object;
    }

    /* --- Load --- */

    template <>
    void adapter_load(Value* from, std::string& to) {
        if (auto* string_ptr = dynamic_cast<String*>(from))
            to = string_ptr->value;
    }

    template <>
    void adapter_load(Value* from, bool& to) {
        if (auto* bool_ptr = dynamic_cast<Bool*>(from))
            to = bool_ptr->value;
    }

    template <>
    void adapter_load(Value* from, glm::vec2& to) {
        if (auto* object_ptr = from->as_object()) {
            to.x = (*object_ptr)["x"]->as_number()->value;
            to.y = (*object_ptr)["y"]->as_number()->value;
        }
    }

    template <>
    void adapter_load(Value* from, glm::vec3& to) {
        if (auto* object_ptr = from->as_object()) {
            to.x = (*object_ptr)["x"]->as_number()->value;
            to.y = (*object_ptr)["y"]->as_number()->value;
            to.z = (*object_ptr)["z"]->as_number()->value;
        }
    }

    template <>
    void adapter_load(Value* from, glm::vec4& to) {
        if (auto* object_ptr = from->as_object()) {
            to.x = (*object_ptr)["x"]->as_number()->value;
            to.y = (*object_ptr)["y"]->as_number()->value;
            to.z = (*object_ptr)["z"]->as_number()->value;
            to.w = (*object_ptr)["w"]->as_number()->value;
        }
    }

}
