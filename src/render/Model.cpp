#include "Model.hpp"

void Model::update(float deltaTime) {

}

void Model::render() {
    glm::vec3 absPos = this->object->absPos();
    glm::vec3 absRot = this->object->absRot();
    glm::vec3 absScale = this->object->absScale();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, absPos);
    model = glm::rotate(model, absRot.x, glm::vec3(1, 0, 0));
    model = glm::rotate(model, absRot.y, glm::vec3(0, 1, 0));
    model = glm::rotate(model, absRot.z, glm::vec3(0, 0, 1));
    model = glm::scale(model, absScale);
    this->object->shader->use();
    this->object->shader->setMat4("model", model);
    for(unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].draw(this->object->shader);
    }
}

void Model::renderDepth(Shader *shader) {
    glm::vec3 absPos = this->object->absPos();
    glm::vec3 absRot = this->object->absRot();
    glm::vec3 absScale = this->object->absScale();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, absPos);
    model = glm::rotate(model, absRot.x, glm::vec3(1, 0, 0));
    model = glm::rotate(model, absRot.y, glm::vec3(0, 1, 0));
    model = glm::rotate(model, absRot.z, glm::vec3(0, 0, 1));
    model = glm::scale(model, absScale);
    shader->use();
    shader->setMat4("model", model);
    for (Mesh m : meshes) {
        m.renderDepth();
    }
}

void Model::loadModel(std::string path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_ConvertToLeftHanded | aiProcess_RemoveRedundantMaterials | aiProcess_FindInvalidData | aiProcess_GenUVCoords);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    // process own meshes
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(processMesh(mesh, scene));			
    }
    // children
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 position, normal;
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;
        vertex.position = position;
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;
        vertex.normal = normal;
        if (mesh->mTextureCoords[0]) {
            glm::vec2 texCoords;
            texCoords.x = mesh->mTextureCoords[0][i].x;
            texCoords.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = texCoords;
        } else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // process material
    if (useTexture) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        std::vector<Texture> emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissive");
        textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());

        return Mesh(vertices, indices, textures);
    } else {
        return Mesh(vertices, indices, color, specular, emissive);
    }
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++) {
            if (textures_loaded[j].path == std::string(str.C_Str())) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture texture((directory + "/" + std::string(str.C_Str())).c_str(), typeName, std::string(str.C_Str()));
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

void Model::cleanup() {
    for (Mesh &m : this->meshes) {
        m.cleanup();
    }
}

void Model::start() {
    this->object->shader->use();
    loadModel(this->path);
}