#include "Types.hpp"

namespace Birdy3d::serializer {

    String* Value::as_string() {
        return dynamic_cast<String*>(this);
    }

    Number* Value::as_number() {
        return dynamic_cast<Number*>(this);
    }

    Bool* Value::as_bool() {
        return dynamic_cast<Bool*>(this);
    }

    Null* Value::as_null() {
        return dynamic_cast<Null*>(this);
    }

    Array* Value::as_array() {
        return dynamic_cast<Array*>(this);
    }

    Object* Value::as_object() {
        return dynamic_cast<Object*>(this);
    }

}
