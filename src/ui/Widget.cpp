#include "Widget.hpp"
#include "../api/Engine.hpp"
#include "../api/Input.hpp"

Widget::Widget(Shader *shader, glm::vec3 pos, Placement placement) {
    this->shader = shader;
    this->pos = pos;
    this->placement = placement;
}

void Widget::addLine(glm::vec2 pos1, glm::vec2 pos2, glm::vec3 color, float depth, float opacity) {
    this->lines.push_back(UI_Vertex{glm::vec3(pos1, depth), glm::vec4(color, opacity)});
    this->lines.push_back(UI_Vertex{glm::vec3(pos2, depth), glm::vec4(color, opacity)});
}

void Widget::addTriangle(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec3 color, float depth, float opacity) {
    this->triangles.push_back(UI_Vertex{glm::vec3(pos1, depth), glm::vec4(color, opacity)});
    this->triangles.push_back(UI_Vertex{glm::vec3(pos2, depth), glm::vec4(color, opacity)});
    this->triangles.push_back(UI_Vertex{glm::vec3(pos3, depth), glm::vec4(color, opacity)});
}

void Widget::addRectangle(glm::vec2 pos1, glm::vec2 pos2, glm::vec3 color, float depth, float opacity) {
    this->addLine(pos1, glm::vec2(pos2.x, pos1.y), color, depth, opacity);
    this->addLine(glm::vec2(pos2.x, pos1.y), pos2, color, depth, opacity);
    this->addLine(pos1, glm::vec2(pos1.x, pos2.y), color, depth, opacity);
    this->addLine(glm::vec2(pos1.x, pos2.y), pos2, color, depth, opacity);
}

void Widget::addFilledRectangle(glm::vec2 pos1, glm::vec2 pos2, glm::vec3 color, float depth, float opacity) {
    this->addTriangle(pos1, glm::vec2(pos1.x, pos2.y), pos2, color, depth, opacity);
    this->addTriangle(pos1, pos2, glm::vec2(pos2.x, pos1.y), color, depth, opacity);
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
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(UI_Vertex), (void*)offsetof(UI_Vertex, color));

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
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(UI_Vertex), (void*)offsetof(UI_Vertex, color));
}

void Widget::draw(glm::mat4 move, glm::vec2 parentSize) {
    if (hidden)
        return;
    
    this->shader->use();
    // get viewport size
    glm::ivec2 viewport = Engine::getViewportSize();
    this->shader->setInt("width", viewport.x);
    this->shader->setInt("height", viewport.y);
    // draw self
    glm::vec3 absPos = getAbsPos(parentSize.x, parentSize.y);
    x = absPos.x / viewport[0] * 2;
    y = absPos.y / viewport[1] * -2;
    move = glm::translate(move, glm::vec3(x, y, pos.z));
    this->shader->setMat4("move", move);
    glBindVertexArray(lines_VAO);
    glDrawArrays(GL_LINES, 0, lines.size());
    glBindVertexArray(triangles_VAO);
    glDrawArrays(GL_TRIANGLES, 0, triangles.size());
    // draw children
    glm::vec2 size = getSize();
    for (Widget w : children) {
        w.draw(move, size);
    }
}

void Widget::draw() {
    draw(glm::mat4(1.0f), Engine::getViewportSize());
}

glm::vec2 Widget::getSize() {
    glm::vec2 size(0);
    for (UI_Vertex v : lines) {
        if (v.position.x > size.x)
            size.x = v.position.x;
        if (v.position.y > size.y)
            size.y = v.position.y;
    }
    for (UI_Vertex v : triangles) {
        if (v.position.x > size.x)
            size.x = v.position.x;
        if (v.position.y > size.y)
            size.y = v.position.y;
    }
    return size;
}

void Widget::updateEvents(glm::vec3 parentAbsPos, glm::vec2 parentSize) {
    if (hidden)
        return;

    glm::vec2 size = getSize();
    
    // self
    glm::vec3 absPos = parentAbsPos + getAbsPos(parentSize.x, parentSize.y);
    if (Input::buttonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 cursorPos = Input::cursorPos();
        if (cursorPos.x > absPos.x && cursorPos.x < absPos.x + size.x && cursorPos.y > absPos.y && cursorPos.y < absPos.y + size.y) {
            clickHandler();
        }
    }

    // children
    for (Widget w : this->children) {
        w.updateEvents(absPos, size);
    }
}

void Widget::updateEvents() {
    updateEvents(glm::vec3(0.0f), Engine::getViewportSize());
}

glm::vec3 Widget::getAbsPos(int parentWidth, int parentHeight) {
    glm::vec3 absPos;
    if (placement == Placement::TOP_LEFT || placement == Placement::BOTTOM_LEFT || placement == Placement::CENTER_LEFT) {
        absPos.x = pos.x;
    } else if (placement == Placement::TOP_RIGHT || placement == Placement::BOTTOM_RIGHT || placement == Placement::CENTER_RIGHT) {
        absPos.x = parentWidth - pos.x;
    } else if (placement == Placement::TOP_CENTER || placement == Placement::BOTTOM_CENTER || placement == Placement::CENTER) {
        absPos.x = parentWidth / 2 + pos.x;
    }
    if (placement == Placement::TOP_LEFT || placement == Placement::TOP_RIGHT || placement == Placement::TOP_CENTER) {
        absPos.y = pos.y;
    } else if (placement == Placement::BOTTOM_LEFT || placement == Placement::BOTTOM_RIGHT || placement == Placement::BOTTOM_CENTER) {
        absPos.y = parentHeight - pos.y; 
    } else if (placement == Placement::CENTER_LEFT || placement == Placement::CENTER_RIGHT || placement == Placement::CENTER) {
        absPos.y = parentHeight / 2 + pos.y;
    }
    return absPos;
}
