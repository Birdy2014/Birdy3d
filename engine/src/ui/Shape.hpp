#pragma once

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

        Type type;

        Shape(UIVector position, UIVector size, Color color = Color::WHITE, Placement placement = Placement::BOTTOM_LEFT)
            : _position(position)
            , _size(size)
            , _color(color)
            , _placement(placement) { }
        UIVector position() { return _position; }
        UIVector position(UIVector position) {
            dirty = true;
            return _position = position;
        }
        UIVector size() { return _size; }
        UIVector size(UIVector size) {
            dirty = true;
            return _size = size;
        }
        Color color() { return _color; }
        Color color(Color color) { return _color = color; }
        Placement placement() { return _placement; }
        Placement placement(Placement placement) {
            dirty = true;
            return _placement = placement;
        }
        glm::vec2 parentSize(glm::vec2 parentSize) {
            dirty = true;
            return _parentSize = parentSize;
        }
        unsigned int texture() { return _texture; }
        unsigned int texture(unsigned int texture) { return _texture = texture; }
        virtual void draw(glm::mat4 move) = 0;
        virtual bool contains(glm::vec2 point) = 0;

    protected:
        Shader* shader;
        unsigned int vao, vbo = 0;
        bool dirty = true;
        UIVector _position;
        UIVector _size;
        Color _color;
        unsigned int _texture;
        Placement _placement;
        glm::vec2 _parentSize;
    };

}
