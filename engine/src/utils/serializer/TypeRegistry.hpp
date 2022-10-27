#pragma once

#include <map>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace Birdy3d::serializer {

    template <class Base, class Derived>
    Base* create_derived()
    {
        return new Derived();
    }

    template <class Base>
    struct BaseRegister {
        typedef Base* (*CreateInstanceFunction)();
        typedef std::map<std::string, CreateInstanceFunction> TypeCreateMap;
        typedef std::unordered_map<std::type_index, std::string> TypeNameMap;

        static Base* create_instance(std::string const& s)
        {
            return get_create_map()[s]();
        }

        static std::string instance_name(std::type_index i)
        {
            return get_name_map()[i];
        }

    protected:
        static TypeCreateMap* create_instance_map;
        static TypeNameMap* name_type_map;

        static TypeCreateMap& get_create_map()
        {
            if (!create_instance_map)
                create_instance_map = new TypeCreateMap;
            return *create_instance_map;
        }

        static TypeNameMap& get_name_map()
        {
            if (!name_type_map)
                name_type_map = new TypeNameMap;
            return *name_type_map;
        }
    };

    template <class Base, class Derived>
    struct DerivedRegister : public BaseRegister<Base> {
        DerivedRegister(std::string const& s)
        {
            BaseRegister<Base>::get_create_map()[s] = &create_derived<Base, Derived>;
            BaseRegister<Base>::get_name_map()[typeid(Derived)] = s;
        }
    };

    template <class Base>
    typename BaseRegister<Base>::TypeCreateMap* BaseRegister<Base>::create_instance_map;

    template <class Base>
    typename BaseRegister<Base>::TypeNameMap* BaseRegister<Base>::name_type_map;

#define BIRDY3D_REGISTER_DERIVED_TYPE_DEC(basename, derivedname) \
private:                                                         \
    static Birdy3d::serializer::DerivedRegister<basename, derivedname> _reg;
#define BIRDY3D_REGISTER_DERIVED_TYPE_DEF(basename, derivedname) Birdy3d::serializer::DerivedRegister<basename, derivedname> derivedname::_reg(#derivedname);
#define BIRDY3D_REGISTER_TYPE_DEC(name) BIRDY3D_REGISTER_DERIVED_TYPE_DEC(name, name)
#define BIRDY3D_REGISTER_TYPE_DEF(name) BIRDY3D_REGISTER_DERIVED_TYPE_DEF(name, name)

}
