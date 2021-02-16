#pragma once

#include "core/Component.hpp"
#include <glm/glm.hpp>

namespace Birdy3d {

    class Shader;
    class Canvas;

    class Camera : public Component {
    public:
        Canvas* canvas = nullptr;

        Camera(int width, int height, bool deferred);
        Camera(int width, int height, bool deferred, Canvas* canvas);
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

}
