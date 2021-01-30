#ifndef BIRDY3D_SHAPE_HPP
#define BIRDY3D_SHAPE_HPP

#include "render/Shader.hpp"
#include <glm/glm.hpp>

class Shape {
public:
    enum Type {
        FILLED = 0,
        OUTLINE = 1,
        TEXT = 2,
        TEXTURE = 3
    };

    Type type;

    Shape(glm::vec2 position, glm::vec2 size, glm::vec4 color = glm::vec4(1)) : _position(position), _size(size), _color(color) {}
    glm::vec2 position() { return _position; }
    glm::vec2 position(glm::vec2 position) { dirty = true; return _position = position; }
    glm::vec2 size() { return _size; }
    glm::vec2 size(glm::vec2 size) { dirty = true; return _size = size; }
    glm::vec4 color() { return _color; }
    glm::vec4 color(glm::vec4 color) { return _color = color; }
    unsigned int texture() { return _texture; }
    unsigned int texture(unsigned int texture) { return _texture = texture; }
    virtual void draw(glm::mat4 move) = 0;
    virtual bool contains(glm::vec2 point) = 0;

protected:
    Shader *shader;
    unsigned int vao, vbo = 0;
    bool dirty = true;
    glm::vec2 _position;
    glm::vec2 _size;
    glm::vec4 _color;
    unsigned int _texture;
};

#endif
