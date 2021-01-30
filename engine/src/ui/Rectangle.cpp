#include "ui/Rectangle.hpp"

#include "core/Application.hpp"
#include "core/RessourceManager.hpp"

Rectangle::Rectangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color, Type type) : Shape(pos, size, color) {
    this->shader = RessourceManager::getShader("ui");
    this->type = type;
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
    glm::vec2 bottomLeft = _position;
    glm::vec2 topRight = _position + _size;
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
    float x = this->_position.x;
    float y = this->_position.y;
    float w = this->_size.x;
    float h = this->_size.y;
    if (type == Shape::OUTLINE) {
        float vertices[] = {
            x,     y,     0.0f, 0.0f,
            x,     y + h, 0.0f, 1.0f,
            x + w, y + h, 1.0f, 1.0f,
            x + w, y,     1.0f, 0.0f,
        };
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]);
    } else {
        float vertices[] = {
            x,     y,     0.0f, 1.0f,
            x + w, y,     1.0f, 1.0f,
            x,     y + h, 0.0f, 0.0f,
            x + w, y + h, 1.0f, 0.0f
        };
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]);
    }
}
