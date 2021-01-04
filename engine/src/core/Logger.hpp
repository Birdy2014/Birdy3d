#ifndef BIRDY3D_LOGGER_HPP
#define BIRDY3D_LOGGER_HPP

#include <iostream>

class Logger {
public:
    static void debug(const std::string &message) {
        std::cout << "DEBUG: " << message << std::endl;
    }

    static void warn(const std::string &message) {
        std::cerr << "WARNING: " << message << std::endl;
    }

    static void error(const std::string &message) {
        std::cerr << "ERROR: " << message << std::endl;
        std::abort();
    }
};

#endif
