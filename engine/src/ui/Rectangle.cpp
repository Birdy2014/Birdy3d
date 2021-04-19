#include "ui/Rectangle.hpp"

#include "core/RessourceManager.hpp"
#include "render/Shader.hpp"
#include <glad/glad.h>

namespace Birdy3d {

    Rectangle::Rectangle(UIVector pos, UIVector size, Color color, Type type, Placement placement)
        : Shape(pos, size, color, placement) {
        this->shader = RessourceManager::getShader("ui");
        this->type = type;
    }

    Rectangle::~Rectangle() {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    void Rectangle::draw(glm::mat4 move) {
        if (!vao || !vbo)
            this->createBuffers();

        if (dirty) {
            dirty = false;
            this->updateVBO();
        }

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->_texture);
        this->shader->use();
        this->shader->setInt("type", this->type);
        this->shader->setMat4("move", move);
        this->shader->setVec4("color", this->_color);
        this->shader->setInt("rectTexture", 0);
        glBindVertexArray(this->vao);
        if (type == Shape::OUTLINE)
            glDrawArrays(GL_LINE_LOOP, 0, 4);
        else
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    bool Rectangle::contains(glm::vec2 point) {
        glm::vec2 bottomLeft = Utils::getRelativePosition(_position, _size, _parentSize, _placement);
        glm::vec2 size = _size.toPixels(_parentSize);
        glm::vec2 topRight = bottomLeft + size;
        return point.x > bottomLeft.x && point.x < topRight.x && point.y > bottomLeft.y && point.y < topRight.y;
    }

    void Rectangle::createBuffers() {
        float vertices[4 * 4];
        // Create buffers
        glGenVertexArrays(1, &this->vao);
        glGenBuffers(1, &this->vbo);
        // Write to buffers
        glBindVertexArray(this->vao);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        // vertex colors
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    void Rectangle::updateVBO() {
        glBindVertexArray(this->vao);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glm::vec2 pos = Utils::getRelativePosition(_position, _size, _parentSize, _placement);
        float x = pos.x;
        float y = pos.y;
        glm::vec2 size = _size.toPixels(_parentSize);
        float w = size.x;
        float h = size.y;
        float ua = texCoordA.x;
        float va = texCoordA.y;
        float ub = texCoordB.x;
        float vb = texCoordB.y;
        // clang-format off
        if (type == Shape::OUTLINE) {
            float vertices[] = {
                x,     y,     ua, va,
                x,     y + h, ua, vb,
                x + w, y + h, ub, vb,
                x + w, y,     ub, va
            };
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]);
        } else {
            float vertices[] = {
                x,     y,     ua, vb,
                x + w, y,     ub, vb,
                x,     y + h, ua, va,
                x + w, y + h, ub, va
            };
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]);
        }
        // clang-format on
    }

}
