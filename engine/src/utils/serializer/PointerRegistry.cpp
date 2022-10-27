#include "utils/serializer/PointerRegistry.hpp"

namespace Birdy3d::serializer {

    std::map<int, std::shared_ptr<void>> PointerRegistry::m_id_to_ptr;
    std::map<void*, int> PointerRegistry::m_ptr_to_id;
    int PointerRegistry::m_id_counter = 0;

    void PointerRegistry::clear()
    {
        m_id_to_ptr.clear();
        m_ptr_to_id.clear();
        m_id_counter = 0;
    }

    int PointerRegistry::get_id_from_ptr(std::shared_ptr<void> ptr)
    {
        if (m_ptr_to_id.count(ptr.get()) > 0)
            return m_ptr_to_id[ptr.get()];
        int id = m_id_counter++;
        m_ptr_to_id[ptr.get()] = id;
        m_id_to_ptr[id] = ptr;
        return id;
    }

    bool PointerRegistry::is_ptr_stored(std::shared_ptr<void> ptr)
    {
        return m_ptr_to_id.count(ptr.get()) > 0;
    }

    std::shared_ptr<void> PointerRegistry::get_ptr_from_id(int id)
    {
        if (m_id_to_ptr.count(id) > 0)
            return m_id_to_ptr[id];
        return nullptr;
    }

    void PointerRegistry::add_ptr_and_id(int id, std::shared_ptr<void> ptr)
    {
        m_ptr_to_id[ptr.get()] = id;
        m_id_to_ptr[id] = ptr;
    }

}
