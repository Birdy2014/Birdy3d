#include "ui/Triangle.hpp"

#include "core/RessourceManager.hpp"
#include "render/Shader.hpp"
#include <glad/glad.h>

namespace Birdy3d {

    Triangle::Triangle(UIVector position, UIVector size, Color color, Type type, Placement placement)
        : Shape(position, size, color, placement) {
        m_shader = RessourceManager::getShader("ui");
        this->type = type;
    }

    Triangle::~Triangle() {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
    }

    void Triangle::draw(glm::mat4 move) {
        if (!m_vao || !m_vbo)
            createBuffers();

        if (m_dirty) {
            m_dirty = false;
            updateVBO();
        }

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        m_shader->use();
        m_shader->setInt("type", this->type);
        m_shader->setMat4("move", move);
        m_shader->setVec4("color", m_color);
        m_shader->setInt("rectTexture", 0);
        glBindVertexArray(m_vao);
        if (type == Shape::OUTLINE)
            glDrawArrays(GL_LINE_LOOP, 0, 3);
        else
            glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    bool Triangle::contains(glm::vec2 point) {
        glm::vec2 position = Utils::getRelativePosition(m_position, m_size, m_parentSize, m_placement);
        glm::vec2 size = m_size.toPixels(m_parentSize);
        glm::vec2 a = position;
        glm::vec2 b = position + glm::vec2(size.x, 0);
        glm::vec2 c = position + glm::vec2(0, size.y);
        float triangleArea = area(a, b, c);
        float area1 = area(point, b, c);
        float area2 = area(a, point, c);
        float area3 = area(a, b, point);
        return (triangleArea == area1 + area2 + area3);
    }

    void Triangle::createBuffers() {
        float vertices[3 * 4];
        // Create buffers
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        // Write to buffers
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        // vertex colors
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    void Triangle::updateVBO() {
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glm::vec2 pos = Utils::getRelativePosition(m_position, m_size, m_parentSize, m_placement);
        float x = pos.x;
        float y = pos.y;
        glm::vec2 size = m_size.toPixels(m_parentSize);
        float w = size.x;
        float h = size.y;
        if (type == Shape::OUTLINE) {
            // clang-format off
            float vertices[] = {
                x,     y,     0.0f, 0.0f,
                x,     y + h, 0.0f, 1.0f,
                x + w, y,     1.0f, 0.0f,
            };
            // clang-format on
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]);
        } else {
            // clang-format off
            float vertices[] = {
                x,     y,     0.0f, 1.0f,
                x + w, y,     1.0f, 1.0f,
                x,     y + h, 0.0f, 0.0f,
            };
            // clang-format on
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]);
        }
    }

    float Triangle::area(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
        return glm::length(glm::cross(glm::vec3(c - a, 0), glm::vec3(c - b, 0))) / 2;
    }

}
