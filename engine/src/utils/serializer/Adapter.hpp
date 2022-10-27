#pragma once

#include "core/Forward.hpp"
#include "utils/serializer/PointerRegistry.hpp"
#include "utils/serializer/Types.hpp"
#include <glm/glm.hpp>

namespace Birdy3d::serializer {

    // Utils
    template <typename T>
    concept ArithmeticType = std::is_arithmetic_v<T>;

    // Save
    template <typename T>
    Value adapter_save(T& value);
    template <ArithmeticType T>
    Value adapter_save(T& value);
    template <class T>
    Value adapter_save(T& value);
    template <>
    Value adapter_save(std::string& value);
    template <>
    Value adapter_save(bool& value);
    template <>
    Value adapter_save(glm::vec2& value);
    template <>
    Value adapter_save(glm::vec3& value);
    template <>
    Value adapter_save(glm::vec4& value);
    template <typename T>
    Value adapter_save(std::vector<T>& value);
    template <typename T, std::size_t N>
    Value adapter_save(std::array<T, N>& value);
    template <typename T>
    Value adapter_save(std::unique_ptr<T>& value);
    template <typename T>
    Value adapter_save(std::shared_ptr<T>& value);
    template <typename T>
    Value adapter_save(std::weak_ptr<T>& value);
    template <class T>
    Value adapter_save(core::ResourceHandle<T>& value);

    // Load
    template <typename T>
    void adapter_load(Value* from, T& to);
    template <ArithmeticType T>
    void adapter_load(Value* from, T& to);
    template <class T>
    void adapter_load(Value* from, T& to);
    template <>
    void adapter_load(Value* from, std::string& to);
    template <>
    void adapter_load(Value* from, bool& to);
    template <>
    void adapter_load(Value* from, glm::vec2& to);
    template <>
    void adapter_load(Value* from, glm::vec3& to);
    template <>
    void adapter_load(Value* from, glm::vec4& to);
    template <typename T>
    void adapter_load(Value* from, std::vector<T>& to);
    template <typename T, std::size_t N>
    void adapter_load(Value* from, std::array<T, N>& to);
    template <typename T>
    void adapter_load(Value* from, std::unique_ptr<T>& to);
    template <typename T>
    void adapter_load(Value* from, std::shared_ptr<T>& to);
    template <typename T>
    void adapter_load(Value* from, std::weak_ptr<T>& to);
    template <class T>
    void adapter_load(Value* from, core::ResourceHandle<T>& to);

    // Reflection
    class ReflectClass;
    void adapter_reflect(ReflectClass* reflect_class, std::string const& member_name, std::type_index member_type, void* member_ptr);

    class Adapter {
    public:
        enum class Mode {
            SAVE,
            LOAD,
            REFLECT
        };

        Adapter(Object* object, Mode mode)
            : m_object(object)
            , m_mode(mode)
        { }

        Adapter(ReflectClass* reflect_class)
            : m_class(reflect_class)
            , m_mode(Mode::REFLECT)
        { }

        template <typename T>
        void operator()(std::string const& key, T& value)
        {
            switch (m_mode) {
            case Mode::SAVE:
                m_object->value[key] = adapter_save(value);
                break;
            case Mode::LOAD:
                adapter_load(&m_object->value[key], value);
                break;
            case Mode::REFLECT:
                adapter_reflect(m_class, key, typeid(value), &value);
                break;
            }
        }

        Mode mode()
        {
            return m_mode;
        }

    private:
        Object* m_object = nullptr;
        ReflectClass* m_class = nullptr;
        Mode m_mode;
    };

    /* --- Save --- */

    // Implementations
    template <ArithmeticType T>
    Value adapter_save(T& value)
    {
        return Number(value);
    }

    template <class T>
    Value adapter_save(T& value)
    {
        auto object = Object();
        Adapter adapter(&object, Adapter::Mode::SAVE);
        value.serialize(adapter);
        return object;
    }

    template <typename T>
    Value adapter_save(std::vector<T>& value)
    {
        auto array = Array();
        for (T& item : value) {
            array.value.push_back(adapter_save(item));
        }
        return array;
    }

    template <typename T, std::size_t N>
    Value adapter_save(std::array<T, N>& value)
    {
        auto array = Array();
        for (T& item : value) {
            array.value.push_back(adapter_save(item));
        }
        return array;
    }

    template <typename T>
    Value adapter_save(std::unique_ptr<T>& value)
    {
        if (value == nullptr)
            return Null();
        auto object = Object();
        object.value["type"] = String(BaseRegister<T>::instance_name(typeid(*value.get())));
        object.value["data"] = adapter_save(*value.get());
        return object;
    }

    template <typename T>
    Value adapter_save(std::shared_ptr<T>& value)
    {
        if (value == nullptr)
            return Null();
        auto object = Object();
        if (!PointerRegistry::is_ptr_stored(value)) {
            object.value["type"] = String(BaseRegister<T>::instance_name(typeid(*value.get())));
            object.value["data"] = adapter_save(*value.get());
        }
        object.value["id"] = Number(PointerRegistry::get_id_from_ptr(value));
        return object;
    }

    template <typename T>
    Value adapter_save(std::weak_ptr<T>& value)
    {
        auto ptr = value.lock();
        return adapter_save(ptr);
    }

    /* --- Load --- */

    // Implementations
    template <ArithmeticType T>
    void adapter_load(Value* from, T& to)
    {
        if (auto number_ptr = std::get_if<Number>(from))
            to = number_ptr->value;
    }

    template <class T>
    void adapter_load(Value* from, T& to)
    {
        if (auto object_ptr = std::get_if<Object>(from)) {
            Adapter adapter(object_ptr, Adapter::Mode::LOAD);
            to.serialize(adapter);
        }
    }

    template <typename T>
    void adapter_load(Value* from, std::vector<T>& to)
    {
        if (auto array_ptr = std::get_if<Array>(from)) {
            to.resize(array_ptr->value.size());
            for (std::size_t i = 0; i < array_ptr->value.size(); i++) {
                adapter_load(&array_ptr->value[i], to[i]);
            }
        }
    }

    template <typename T, std::size_t N>
    void adapter_load(Value* from, std::array<T, N>& to)
    {
        if (auto array_ptr = std::get_if<Array>(from)) {
            for (std::size_t i = 0; i < array_ptr->value.size(); i++) {
                adapter_load(&array_ptr->value[i], to[i]);
            }
        }
    }

    template <class T>
    void adapter_load(Value* from, std::unique_ptr<T>& to)
    {
        if (auto object_ptr = std::get_if<Object>(from)) {
            String* type_string_ptr;
            if ((type_string_ptr = std::get_if<String>(&object_ptr->value["type"])) && std::holds_alternative<Object>(object_ptr->value["data"])) {
                to = std::unique_ptr<T>(BaseRegister<T>::create_instance(type_string_ptr->value));
                adapter_load(&object_ptr->value["data"], *to.get());
                return;
            }
        }
        to = nullptr;
    }

    template <class T>
    void adapter_load(Value* from, std::shared_ptr<T>& to)
    {
        if (auto object_ptr = std::get_if<Object>(from)) {
            if (auto id_number_ptr = std::get_if<Number>(&object_ptr->value["id"])) {
                std::shared_ptr<void> ptr = PointerRegistry::get_ptr_from_id(id_number_ptr->value);
                if (ptr) {
                    to = std::static_pointer_cast<T>(ptr);
                    return;
                }
                String* type_string_ptr;
                if ((type_string_ptr = std::get_if<String>(&object_ptr->value["type"])) && std::holds_alternative<Object>(object_ptr->value["data"])) {
                    to = std::unique_ptr<T>(BaseRegister<T>::create_instance(type_string_ptr->value));
                    adapter_load(&object_ptr->value["data"], *to.get());
                    PointerRegistry::add_ptr_and_id(id_number_ptr->value, to);
                    return;
                }
            }
        }
        to = nullptr;
    }

    template <class T>
    void adapter_load(Value* from, std::weak_ptr<T>& to)
    {
        std::shared_ptr<T> ptr;
        adapter_load(from, ptr);
        to = ptr;
    }
}
