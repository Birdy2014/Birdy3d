#ifndef BIRDY3D_MODEL_HPP
#define BIRDY3D_MODEL_HPP

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>
#include "core/Component.hpp"
#include "core/GameObject.hpp"
#include "render/Mesh.hpp"
#include "render/Light.hpp"

class Model : public Component {
public:

    Model(std::string path, bool useTexture = true, glm::vec4 color = glm::vec4(0.0f), float specular = 1, glm::vec3 emissive = glm::vec3(0.0f)) {
        this->path = path;
        this->useTexture = useTexture;
        this->color = color;
        this->specular = specular;
        this->emissive = emissive;
    }
    void cleanup() override;
    void start() override;
    void render(Shader *shader, bool transparent);
    void renderDepth(Shader *shader);

private:
    std::string path;
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;
    bool useTexture;
    glm::vec4 color;
    float specular;
    glm::vec3 emissive;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif
