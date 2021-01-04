#ifndef BIRDY3D_RECTANGLE_HPP
#define BIRDY3D_RECTANGLE_HPP

#include <glm/glm.hpp>
#include <glad/glad.h>
#include "render/Shader.hpp"
#include "core/Application.hpp"

class Rectangle {
public:
    Rectangle(glm::ivec2 pos, glm::ivec2 size, bool filled, glm::vec4 color);
    Rectangle(glm::ivec2 pos, glm::ivec2 size, unsigned int textureID);
    Rectangle(glm::ivec2 pos, glm::ivec2 size, glm::vec4 textColor, unsigned int charTexture);
    glm::ivec2 getPos();
    glm::ivec2 getSize();
    void resize(glm::ivec2 size);
    void setPos(glm::ivec2 pos);
    void setTexture(unsigned int textureID);
    void setColor(glm::vec4 color);
    void setMove(glm::mat4 move);
    void draw();

private:
    Shader *shader;
    unsigned int vao, vbo = 0;
    bool dirty = true;
    bool hasMatrix = false;
    // Global position and size in pixels (maybe dpi independent later) or normalized
    glm::ivec2 pos;
    glm::ivec2 size;
    glm::mat4 move;
    bool filled;
    bool hasTexture;
    bool isText;
    unsigned int textureID;
    glm::vec4 color;

    void createBuffers();
    void updateVBO();
};

#endif
