#include "utils/serializer/Adapter.hpp"
#include "utils/serializer/Reflector.hpp"

namespace Birdy3d::serializer {

    /* --- Save --- */

    template <>
    Value adapter_save(std::string& value)
    {
        return String(value);
    }

    template <>
    Value adapter_save(bool& value)
    {
        return Bool(value);
    }

    template <>
    Value adapter_save(glm::vec2& value)
    {
        auto object = Object();
        object.value["x"] = Number(value.x);
        object.value["y"] = Number(value.y);
        return object;
    }

    template <>
    Value adapter_save(glm::vec3& value)
    {
        auto object = Object();
        object.value["x"] = Number(value.x);
        object.value["y"] = Number(value.y);
        object.value["z"] = Number(value.z);
        return object;
    }

    template <>
    Value adapter_save(glm::vec4& value)
    {
        auto object = Object();
        object.value["x"] = Number(value.x);
        object.value["y"] = Number(value.y);
        object.value["z"] = Number(value.z);
        object.value["w"] = Number(value.w);
        return object;
    }

    /* --- Load --- */

    template <>
    void adapter_load(Value* from, std::string& to)
    {
        if (auto string_ptr = std::get_if<String>(from))
            to = string_ptr->value;
    }

    template <>
    void adapter_load(Value* from, bool& to)
    {
        if (auto bool_ptr = std::get_if<Bool>(from))
            to = bool_ptr->value;
    }

    template <>
    void adapter_load(Value* from, glm::vec2& to)
    {
        if (auto object_ptr = std::get_if<Object>(from)) {
            if (auto number_ptr = std::get_if<Number>(&(*object_ptr)["x"]))
                to.x = number_ptr->value;
            if (auto number_ptr = std::get_if<Number>(&(*object_ptr)["y"]))
                to.y = number_ptr->value;
        }
    }

    template <>
    void adapter_load(Value* from, glm::vec3& to)
    {
        if (auto object_ptr = std::get_if<Object>(from)) {
            if (auto number_ptr = std::get_if<Number>(&(*object_ptr)["x"]))
                to.x = number_ptr->value;
            if (auto number_ptr = std::get_if<Number>(&(*object_ptr)["y"]))
                to.y = number_ptr->value;
            if (auto number_ptr = std::get_if<Number>(&(*object_ptr)["z"]))
                to.z = number_ptr->value;
        }
    }

    template <>
    void adapter_load(Value* from, glm::vec4& to)
    {
        if (auto object_ptr = std::get_if<Object>(from)) {
            if (auto number_ptr = std::get_if<Number>(&(*object_ptr)["x"]))
                to.x = number_ptr->value;
            if (auto number_ptr = std::get_if<Number>(&(*object_ptr)["y"]))
                to.y = number_ptr->value;
            if (auto number_ptr = std::get_if<Number>(&(*object_ptr)["z"]))
                to.z = number_ptr->value;
            if (auto number_ptr = std::get_if<Number>(&(*object_ptr)["w"]))
                to.w = number_ptr->value;
        }
    }

    /* --- Reflection --- */

    std::map<void*, ReflectClass> Reflector::m_classes;

    void adapter_reflect(ReflectClass* reflect_class, std::string const& member_name, std::type_index member_type, void* member_ptr)
    {
        reflect_class->m_members.emplace_back(member_name, member_type, member_ptr);
    }

}
