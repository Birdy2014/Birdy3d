#ifndef BIRDY3D_RECTANGLE_HPP
#define BIRDY3D_RECTANGLE_HPP

#include <glm/glm.hpp>
#include <glad/glad.h>
#include "render/Shader.hpp"
#include "core/Application.hpp"

class Rectangle {
public:
    Rectangle(Shader *shader, glm::ivec2 pos, glm::ivec2 size, float depth, bool filled, glm::vec4 color);
    Rectangle(Shader *shader, glm::ivec2 pos, glm::ivec2 size, float depth, unsigned int textureID);
    Rectangle(Shader *shader, glm::ivec2 pos, glm::ivec2 size, float depth, glm::vec4 textColor, unsigned int charTexture);
    void resize(glm::ivec2 size);
    void move(glm::ivec2 pos);
    void setTexture(unsigned int textureID);
    void setColor(glm::vec4 color);
    void setDepth(float depth);
    void draw();

private:
    Shader *shader;
    unsigned int vao, vbo = 0;
    bool dirty = true;
    // Global position and size in pixels (maybe dpi independent later)
    glm::ivec2 pos;
    glm::ivec2 size;
    int depth;
    bool filled;
    bool hasTexture;
    bool isText;
    unsigned int textureID;
    glm::vec4 color;

    void createBuffers();
    void updateVBO();
};

#endif
