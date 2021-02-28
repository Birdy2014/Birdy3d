#include "ui/Triangle.hpp"

#include "core/RessourceManager.hpp"
#include "render/Shader.hpp"
#include <glad/glad.h>

namespace Birdy3d {

    Triangle::Triangle(UIVector position, UIVector size, Color color, Type type, Placement placement)
        : Shape(position, size, color, placement) {
        this->shader = RessourceManager::getShader("ui");
        this->type = type;
    }

    void Triangle::draw(glm::mat4 move) {
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
            glDrawArrays(GL_LINE_LOOP, 0, 3);
        else
            glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    bool Triangle::contains(glm::vec2 point) {
        glm::vec2 position = Utils::getRelativePosition(_position, _size, _parentSize, _placement);
        glm::vec2 size = _size.toPixels(_parentSize);
        glm::vec2 a = position;
        glm::vec2 b = position + glm::vec2(size.x, 0);
        glm::vec2 c = position + glm::vec2(0, size.y);
        float area = this->area(a, b, c);
        float area1 = this->area(point, b, c);
        float area2 = this->area(a, point, c);
        float area3 = this->area(a, b, point);
        return (area == area1 + area2 + area3);
    }

    void Triangle::createBuffers() {
        float vertices[3 * 4];
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

    void Triangle::updateVBO() {
        glBindVertexArray(this->vao);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glm::vec2 pos = Utils::getRelativePosition(_position, _size, _parentSize, _placement);
        float x = pos.x;
        float y = pos.y;
        glm::vec2 size = _size.toPixels(_parentSize);
        float w = size.x;
        float h = size.y;
        if (type == Shape::OUTLINE) {
            float vertices[] = {
                x,
                y,
                0.0f,
                0.0f,
                x,
                y + h,
                0.0f,
                1.0f,
                x + w,
                y,
                1.0f,
                0.0f,
            };
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]);
        } else {
            float vertices[] = {
                x,
                y,
                0.0f,
                1.0f,
                x + w,
                y,
                1.0f,
                1.0f,
                x,
                y + h,
                0.0f,
                0.0f,
            };
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]);
        }
    }

    float Triangle::area(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
        return glm::length(glm::cross(glm::vec3(c - a, 0), glm::vec3(c - b, 0))) / 2;
    }

}
