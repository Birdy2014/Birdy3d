#ifndef BIRDY3D_CAMERA_HPP
#define BIRDY3D_CAMERA_HPP

#include <glm/glm.hpp>
#include "core/Component.hpp"
#include "render/Shader.hpp"
#include "ui/Widget.hpp"

class Camera : public Component {
public:
    Widget *canvas = nullptr;

    Camera(int width, int height, bool deferred);
    Camera(int width, int height, bool deferred, Widget *canvas);
    void start() override;
    void cleanup() override;
    void render();
    void resize(int width, int height);
    void reloadModels();

private:
    unsigned int gBuffer, gPosition, gNormal, gAlbedoSpec, rboDepth;
    int width, height;
    glm::mat4 projectionMatrix;
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    Shader *deferredGeometryShader, *deferredLightShader, *forwardShader;
    glm::vec3 worldUp = glm::vec3(0, 1, 0);
    bool deferred;

    void createGBuffer();
    void deleteGBuffer();
    void renderQuad();
    void renderDeferred();
    void renderForward(bool renderOpaque);
};

#endif
