#pragma once

#include <iostream>

namespace Birdy3d {

    class Logger {
    public:
        static void debug(const std::string& message) {
            std::cout << "DEBUG: " << message << std::endl;
        }

        static void warn(const std::string& message) {
            std::cerr << "WARNING: " << message << std::endl;
        }

        static void error(const std::string& message) {
            std::cerr << "ERROR: " << message << std::endl;
            std::abort();
        }

        static void assertNotNull(void* obj, const std::string& message) {
            if (obj == nullptr)
                Logger::error("Assert not null failed: " + message);
        }
    };

}
