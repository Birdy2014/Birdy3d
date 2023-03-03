#include "utils/TextureLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Birdy3d::utils {

    std::optional<TextureLoader::Image> TextureLoader::from_file(std::string_view const file_path)
    {
        int width, height, channels;

        unsigned char* raw_data = stbi_load(file_path.data(), &width, &height, &channels, 0);

        if (!raw_data)
            return {};

        assert(width > 0);
        assert(height > 0);
        assert(channels >= 1 && channels <= 4);

        ImageFormat format;

        switch (channels) {
        case 1:
            format = ImageFormat::R;
            break;
        case 2:
            format = ImageFormat::RG;
            break;
        case 3:
            format = ImageFormat::RGB;
            break;
        case 4:
            format = ImageFormat::RGBA;
            break;
        default:
            stbi_image_free(raw_data);
            return {};
        }

        std::vector<unsigned char> data(raw_data, raw_data + width * height * channels);
        stbi_image_free(raw_data);

        return Image{
            .width = static_cast<std::size_t>(width),
            .height = static_cast<std::size_t>(height),
            .format = format,
            .data = std::move(data)};
    }

}
