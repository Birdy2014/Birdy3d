#pragma once

#include <optional>
#include <string_view>
#include <vector>

namespace Birdy3d::utils {

    class TextureLoader {
    public:
        enum class ImageFormat {
            R,
            RG,
            RGB,
            RGBA,
        };

        struct Image {
            std::size_t width;
            std::size_t height;
            ImageFormat format;
            std::vector<unsigned char> data;
        };

        static std::optional<Image> from_file(std::string_view const file_path);
    };

};
