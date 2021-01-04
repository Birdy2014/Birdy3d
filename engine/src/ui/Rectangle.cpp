#include "ui/Rectangle.hpp"

#include "core/RessourceManager.hpp"

Rectangle::Rectangle(glm::ivec2 pos, glm::ivec2 size, bool filled, glm::vec4 color) : pos(pos), size(size), filled(filled), color(color) {
    this->shader = RessourceManager::getShader("ui");
    this->hasTexture = false;
    this->isText = false;
}

Rectangle::Rectangle(glm::ivec2 pos, glm::ivec2 size, unsigned int textureID) : pos(pos), size(size), textureID(textureID) {
    this->shader = RessourceManager::getShader("ui");
    this->hasTexture = true;
    this->filled = true;
    this->isText = false;
}

Rectangle::Rectangle(glm::ivec2 pos, glm::ivec2 size, glm::vec4 textColor, unsigned int charTexture) : pos(pos), size(size), color(textColor), textureID(charTexture) {
    this->shader = RessourceManager::getShader("ui");
    this->isText = true;
    this->filled = true;
}

glm::ivec2 Rectangle::getPos() {
    return this->pos;
}

glm::ivec2 Rectangle::getSize() {
    return this->size;
}

void Rectangle::resize(glm::ivec2 size) {
    if (this->size != size) {
        this->size = size;
        this->dirty = true;
    }
}

void Rectangle::setPos(glm::ivec2 pos) {
    if (this->pos != pos) {
        this->pos = pos;
        this->dirty = true;
    }
}

void Rectangle::setTexture(unsigned int textureID) {
    this->textureID = textureID;
}

void Rectangle::setColor(glm::vec4 color) {
    this->color = color;
}

void Rectangle::setMove(glm::mat4 move) {
    this->hasMatrix = true;
    this->move = move;
}

void Rectangle::draw() {
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
    glBindTexture(GL_TEXTURE_2D, this->textureID);
    this->shader->use();
    this->shader->setBool("isText", this->isText);
    this->shader->setBool("hasTexture", this->hasTexture);
    this->shader->setBool("hasMatrix", this->hasMatrix);
    this->shader->setMat4("move", this->move);
    this->shader->setVec4("color", this->color);
    this->shader->setInt("rectTexture", 0);
    glBindVertexArray(this->vao);
    if (this->filled)
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    else
        glDrawArrays(GL_LINE_LOOP, 0, 4);
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
    float x = this->pos.x;
    float y = this->pos.y;
    float w = this->size.x;
    float h = this->size.y;
    if (this->filled) {
        float vertices[] = {
            x,     y,     0.0f, 1.0f,
            x + w, y,     1.0f, 1.0f,
            x,     y + h, 0.0f, 0.0f,
            x + w, y + h, 1.0f, 0.0f
        };
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]);
    } else {
        float vertices[] = {
            x,     y,     0.0f, 0.0f,
            x,     y + h, 0.0f, 1.0f,
            x + w, y + h, 1.0f, 1.0f,
            x + w, y,     1.0f, 0.0f,
        };
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]);
    }
}
