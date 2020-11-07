#ifndef BIRDY3D_CAMERA_HPP
#define BIRDY3D_CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../api/Component.hpp"
#include "Shader.hpp"
#include "../ui/Widget.hpp"

class Camera : public Component {
public:
    Widget *canvas = nullptr;

    Camera(Shader *lightShader, int width, int height);
    Camera(Shader *lightShader, int width, int height, Widget *canvas);
    void start() override;
    void render();
    void resize(int width, int height);

private:
    unsigned int gBuffer, gPosition, gNormal, gAlbedoSpec, rboDepth;
    int width, height;
    glm::mat4 projectionMatrix;
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    Shader *lightShader;
    glm::vec3 worldUp = glm::vec3(0, 1, 0);

    void createGBuffer();
    void deleteGBuffer();
    void renderQuad();
};

#endif
