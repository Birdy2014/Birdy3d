#include "Widget.hpp"

void Widget::addLine(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 color) {
    this->lines.push_back(UI_Vertex{pos1, color});
    this->lines.push_back(UI_Vertex{pos2, color});
}

void Widget::addTriangle(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 color) {
    this->triangles.push_back(UI_Vertex{pos1, color});
    this->triangles.push_back(UI_Vertex{pos2, color});
    this->triangles.push_back(UI_Vertex{pos3, color});
}

void Widget::addRectangle(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 color) {
    this->addTriangle(pos1, glm::vec3(pos1.x, pos2.y, pos2.z), pos2, color);
    this->addTriangle(pos1, glm::vec3(pos2.x, pos1.y, pos1.z), pos2, color);
}

void Widget::fillBuffer() {
    // lines
    glGenVertexArrays(1, &lines_VAO);
    glGenBuffers(1, &lines_VBO);
    glBindVertexArray(lines_VAO);
    // load vertices into VBO
    glBindBuffer(GL_ARRAY_BUFFER, lines_VBO);
    glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(UI_Vertex), &lines[0], GL_STATIC_DRAW);
    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(UI_Vertex), (void*)0);
    // vertex colors
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(UI_Vertex), (void*)offsetof(UI_Vertex, color));

    // triangles
    glGenVertexArrays(1, &triangles_VAO);
    glGenBuffers(1, &triangles_VBO);
    glBindVertexArray(triangles_VAO);
    // load vertices into VBO
    glBindBuffer(GL_ARRAY_BUFFER, triangles_VBO);
    glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(UI_Vertex), &triangles[0], GL_STATIC_DRAW);
    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(UI_Vertex), (void*)0);
    // vertex colors
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(UI_Vertex), (void*)offsetof(UI_Vertex, color));
}

void Widget::draw(glm::mat4 move, float parentSize[]) {
    if (hidden) {
        return;
    }
    
    this->shader->use();
    // get viewport size
    float *viewport = getViewportSize();
    this->shader->setInt("width", viewport[0]);
    this->shader->setInt("height", viewport[1]);
    // draw self
    glm::vec3 absPos = getAbsPos(parentSize);
    x = absPos.x / viewport[0] * 2;
    y = absPos.y / viewport[1] * -2;
    move = glm::translate(move, glm::vec3(x, y, pos.z));
    this->shader->setMat4("move", move);
    glBindVertexArray(lines_VAO);
    glDrawArrays(GL_LINES, 0, lines.size());
    glBindVertexArray(triangles_VAO);
    glDrawArrays(GL_TRIANGLES, 0, triangles.size());
    // draw children
    for (Widget w : children) {
        float *size = getSize();
        w.draw(move, size);
    }
}

float *Widget::getSize() {
    float *size = (float*)malloc(2 * sizeof(float));
    size[0] = 0;
    size[1] = 0;
    for (UI_Vertex v : lines) {
        if (v.position.x > size[0])
            size[0] = v.position.x;
        if (v.position.y > size[1])
            size[1] = v.position.y;
    }
    for (UI_Vertex v : triangles) {
        if (v.position.x > size[0])
            size[0] = v.position.x;
        if (v.position.y > size[1])
            size[1] = v.position.y;
    }
    return size;
}

float *Widget::getViewportSize() {
    GLint m_viewport[4];
    glGetIntegerv(GL_VIEWPORT, m_viewport);
    float *screen = (float*)malloc(2 * sizeof(float));
    screen[0] = m_viewport[2];
    screen[1] = m_viewport[3];
    return screen;
}

void Widget::updateEvents(GLFWwindow *window, glm::vec3 parentAbsPos, float parentSize[]) {
    if (hidden) {
        return;
    }
    
    // self
    glm::vec3 absPos = parentAbsPos + getAbsPos(parentSize);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        float *size = getSize();
        if (xpos > absPos.x && xpos < absPos.x + size[0] && ypos > absPos.y && ypos < absPos.y + size[1]) {
            clickHandler();
        }
    }
    // children
    float *size = getSize();
    for (Widget w : this->children) {
        w.updateEvents(window, absPos, size);
    }
}

glm::vec3 Widget::getAbsPos(float parentSize[]) {
    glm::vec3 absPos;
    if (placement == TOP_LEFT || placement == BOTTOM_LEFT || placement == CENTER_LEFT) {
        absPos.x = pos.x;
    } else if (placement == TOP_RIGHT || placement == BOTTOM_RIGHT || placement == CENTER_RIGHT) {
        absPos.x = parentSize[0] - pos.x;
    } else if (placement == TOP_CENTER || placement == BOTTOM_CENTER || placement == CENTER) {
        absPos.x = parentSize[0] / 2 + pos.x;
    }
    if (placement == TOP_LEFT || placement == TOP_RIGHT || placement == TOP_CENTER) {
        absPos.y = pos.y;
    } else if (placement == BOTTOM_LEFT || placement == BOTTOM_RIGHT || placement == BOTTOM_CENTER) {
        absPos.y = parentSize[1] - pos.y; 
    } else if (placement == CENTER_LEFT || placement == CENTER_RIGHT || placement == CENTER) {
        absPos.y = parentSize[1] / 2 + pos.y;
    }
    return absPos;
}
