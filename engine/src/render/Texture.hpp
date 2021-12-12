#pragma once

#include "core/Base.hpp"
#include "utils/Color.hpp"

namespace Birdy3d::render {

    class Texture {
    public:
        GLuint m_id;

        Texture(const std::string& filePath);
        Texture(const utils::Color& color);
        ~Texture();
        bool transparent() const;
        void bind(int texture_unit);

    private:
        int m_width, m_height, m_channels;
        bool m_transparent;
    };

}
