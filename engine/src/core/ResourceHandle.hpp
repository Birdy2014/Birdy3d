#pragma once

#include "render/Forward.hpp"
#include "ui/Forward.hpp"
#include "utils/serializer/Adapter.hpp"
#include <memory>
#include <string>

namespace Birdy3d::core {

    template <class T>
    class ResourceHandle {
    public:
        ResourceHandle() = default;
        ResourceHandle(const std::string& name)
            : m_name(name) { load(name); }
        T& operator*() const { return *m_ptr; }
        T* operator->() const { return m_ptr.get(); }
        operator std::string() const { return m_name; }
        explicit operator bool() const { return (bool)m_ptr; }
        std::shared_ptr<T> ptr() const { return m_ptr; }
        bool load(std::string) { return false; }
        bool operator=(std::string new_name) { return load(new_name); }

    private:
        std::string m_name;
        std::shared_ptr<T> m_ptr;
    };

    template <>
    bool ResourceHandle<render::Shader>::load(std::string new_name);
    template <>
    bool ResourceHandle<ui::Theme>::load(std::string new_name);
    template <>
    bool ResourceHandle<render::Model>::load(std::string new_name);
    template <>
    bool ResourceHandle<render::Texture>::load(std::string new_name);

}

namespace Birdy3d::serializer {

    template <class T>
    std::unique_ptr<Value> adapter_save(core::ResourceHandle<T>& value) {
        return std::make_unique<String>((std::string)value);
    }

    template <class T>
    void adapter_load(Value* from, core::ResourceHandle<T>& to) {
        if (auto* string_ptr = from->as_string()) {
            to = core::ResourceHandle<T>(string_ptr->value);
        }
    }

}
