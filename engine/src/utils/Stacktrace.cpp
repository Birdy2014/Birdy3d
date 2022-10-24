#include "utils/Stacktrace.hpp"

#include "core/Base.hpp"

#ifdef BIRDY3D_PLATFORM_LINUX
    #include <execinfo.h>
    #include <regex>
    #include <string>
#endif

namespace Birdy3d::utils {

    void print_stacktrace() {
#ifdef BIRDY3D_PLATFORM_LINUX
        void* trace_array[10];
        size_t trace_size;

        trace_size = backtrace(trace_array, 10);
        auto messages = backtrace_symbols(trace_array, trace_size);

        printf("[bt] Execution path:\n");
        auto line_regex = std::regex { "(.+)\\((\\+0x[0-9a-f]+)\\) \\[0x[0-9a-f]+\\]" };
        for (std::size_t i = 1; i < trace_size; ++i) {
            printf("[bt] #%lu %s\n", i, messages[i]);

            std::smatch matches;
            auto line = std::string { messages[i] };

            if (std::regex_match(line, matches, line_regex)) {
                auto executable_path = matches[1];
                if (executable_path.str().starts_with("/usr/lib"))
                    continue;
                auto offset = matches[2];
                char syscom[256];
                sprintf(syscom, "addr2line %s -e %s", offset.str().c_str(), executable_path.str().c_str());
                system(syscom);
            } else {
                std::cout << "UNEXPECTED BACKTRACE FORMAT" << std::endl;
            }
        }

        free(messages);
#endif
    }

}
