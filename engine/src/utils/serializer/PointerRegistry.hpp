#pragma once

#include <map>
#include <memory>

namespace Birdy3d::serializer {

    class PointerRegistry {
    public:
        static void clear();
        // Serialization
        static int get_id_from_ptr(std::shared_ptr<void>); // Returns stored id or generates new
        static bool is_ptr_stored(std::shared_ptr<void>);
        // Deserialization
        static std::shared_ptr<void> get_ptr_from_id(int); // Returns stored pointer or nullptr?
        static void add_ptr_and_id(int, std::shared_ptr<void>);

    private:
        static std::map<int, std::shared_ptr<void>> m_id_to_ptr; // Serialization
        static std::map<void*, int> m_ptr_to_id; // Deserialization
        static int m_id_counter;
    };

}
