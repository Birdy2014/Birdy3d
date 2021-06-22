#pragma once

#include "render/Color.hpp"
#include "ui/Utils.hpp"
#include <glm/glm.hpp>

namespace Birdy3d {

    class Shader;

    class Shape {
    public:
        enum Type {
            FILLED = 0,
            OUTLINE = 1,
            TEXT = 2,
            TEXTURE = 3
        };

        std::string name;
        Type type;

        Shape(UIVector position, UIVector size, Color color = Color::WHITE, Placement placement = Placement::BOTTOM_LEFT, glm::vec2 texCoordA = glm::vec2(0), glm::vec2 texCoordB = glm::vec2(1), std::string name = "")
            : name(name)
            , m_position(position)
            , m_size(size)
            , m_color(color)
            , m_placement(placement)
            , m_texCoordA(texCoordA)
            , m_texCoordB(texCoordB) { }
        virtual ~Shape() {};
        UIVector position() { return m_position; }
        UIVector position(UIVector position) {
            m_dirty = true;
            return m_position = position;
        }
        UIVector size() { return m_size; }
        UIVector size(UIVector size) {
            m_dirty = true;
            return m_size = size;
        }
        Color color() { return m_color; }
        Color color(Color color) { return m_color = color; }
        Placement placement() { return m_placement; }
        Placement placement(Placement placement) {
            m_dirty = true;
            return m_placement = placement;
        }
        glm::vec2 parentSize(glm::vec2 parentSize) {
            m_dirty = true;
            return m_parentSize = parentSize;
        }
        unsigned int texture() { return m_texture; }
        unsigned int texture(unsigned int texture) { return m_texture = texture; }
        void texCoords(glm::vec2 a, glm::vec2 b) {
            m_dirty = true;
            m_texCoordA = a;
            m_texCoordB = b;
        }
        virtual void draw(glm::mat4 move) = 0;
        virtual bool contains(glm::vec2 point) = 0;

    protected:
        Shader* m_shader;
        unsigned int m_vao = 0;
        unsigned int m_vbo = 0;
        bool m_dirty = true;
        UIVector m_position;
        UIVector m_size;
        Color m_color;
        Placement m_placement;
        unsigned int m_texture = 0;
        glm::vec2 m_texCoordA;
        glm::vec2 m_texCoordB;
        glm::vec2 m_parentSize;
    };

}
