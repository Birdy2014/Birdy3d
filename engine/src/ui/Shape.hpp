#pragma once

#include "core/Application.hpp"
#include "core/Base.hpp"
#include "core/RessourceManager.hpp"
#include "ui/Units.hpp"
#include "utils/Color.hpp"

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

        Shape(UIVector position, UIVector size, Color::Name color = Color::Name::WHITE, Placement placement = Placement::BOTTOM_LEFT, glm::vec2 texCoordA = glm::vec2(0), glm::vec2 texCoordB = glm::vec2(1), std::string name = "")
            : name(name)
            , m_shader(RessourceManager::get_shader("ui"))
            , m_position(position)
            , m_rotation(0)
            , m_size(size)
            , m_color(color)
            , m_placement(placement)
            , m_texCoordA(texCoordA)
            , m_texCoordB(texCoordB) { }
        virtual ~Shape() = default;
        bool hidden() { return m_hidden; }
        bool hidden(bool hidden) { return m_hidden = hidden; }
        UIVector position() { return m_position; }
        void position(UIVector position) {
            if (m_position == position)
                return;
            m_dirty = true;
            m_position = position;
        }
        float rotation() { return m_rotation; }
        void rotation(float rotation) {
            if (m_rotation == rotation)
                return;
            m_dirty = true;
            m_rotation = rotation;
        }
        UIVector size() { return m_size; }
        void size(UIVector size) {
            if (m_size == size)
                return;
            m_dirty = true;
            m_size = size;
        }
        Color::Name color() { return m_color; }
        Color::Name color(Color::Name color) { return m_color = color; }
        Placement placement() { return m_placement; }
        Placement placement(Placement placement) {
            m_dirty = true;
            return m_placement = placement;
        }
        glm::vec2 parent_size(glm::vec2 parentSize) {
            m_dirty = true;
            return m_parentSize = parentSize;
        }
        unsigned int texture() { return m_texture; }
        unsigned int texture(unsigned int texture) { return m_texture = texture; }
        void texcoords(glm::vec2 a, glm::vec2 b) {
            m_dirty = true;
            m_texCoordA = a;
            m_texCoordB = b;
        }
        virtual void draw(glm::mat4 move) = 0;
        virtual bool contains(glm::vec2 point) = 0;

        glm::mat4 projection() {
            glm::vec2 viewport = Application::get_viewport_size();
            // The -1 is necessary, because the parameter describes the rightmost/top coordinate, not the screen size
            return glm::ortho(0.0f, viewport.x - 1, 0.0f, viewport.y - 1);
            // TODO: Flip y-coordinate by swapping the last two parameters
        }

    protected:
        std::shared_ptr<Shader> m_shader;
        unsigned int m_vao = 0;
        unsigned int m_vbo = 0;
        bool m_dirty = true;
        bool m_hidden = false;
        UIVector m_position;
        float m_rotation;
        UIVector m_size;
        glm::mat4 m_move_self;
        Color::Name m_color;
        Placement m_placement;
        unsigned int m_texture = 0;
        glm::vec2 m_texCoordA;
        glm::vec2 m_texCoordB;
        glm::vec2 m_parentSize;
    };

}
