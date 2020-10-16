#ifndef BIRDY3D_MODEL_HPP
#define BIRDY3D_MODEL_HPP

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.hpp"
#include "../objects/Component.hpp"
#include "../objects/GameObject.hpp"
#include "Light.hpp"

class Model : public Component {
public:
    Model(GameObject *o, std::string path, bool useTexture = true, glm::vec3 color = glm::vec4(0.0f), float specular = 16, glm::vec3 emissive = glm::vec3(0.0f)) : Component(o) {
        this->path = path;
        this->useTexture = useTexture;
        this->color = color;
        this->specular = specular;
        this->emissive = emissive;
    }
    void cleanup() override;
    void start() override;
    void update(float deltaTime);
    void render();
    void renderDepth();
    const std::type_info &getTypeid() override {
        return typeid(Model);
    }

private:
    std::string path;
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;
    bool useTexture;
    glm::vec3 color;
    float specular;
    glm::vec3 emissive;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif