#pragma once

#include "utils/serializer/Adapter.hpp"
#include "utils/serializer/TypeRegistry.hpp"
#include <map>
#include <string>
#include <typeindex>
#include <vector>

namespace Birdy3d::serializer {

    struct ReflectMember {
        std::string name;
        std::type_index type;
        void* value;
    };

    class ReflectClass {
    public:
        std::string name;
        std::vector<ReflectMember> m_members;
    };

    class Reflector {
    public:
        template <class T>
        static const ReflectClass& get_class(T* c) {
            if (m_classes.count(c) > 0)
                return m_classes[c];
            Adapter adapter(&m_classes[c]);
            c->serialize(adapter);
            m_classes[c].name = BaseRegister<T>::instance_name(typeid(*c));
            return m_classes[c];
        }

    private:
        static std::map<void*, ReflectClass> m_classes;
    };

}
