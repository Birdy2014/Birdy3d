#include "render/Mesh.hpp"

#include "render/Shader.hpp"
#include "render/Texture.hpp"
#include "render/Vertex.hpp"

namespace Birdy3d {

    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        setupMesh();
    }

    Mesh::~Mesh() {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void Mesh::setupMesh() {
        // generate VAO, VBO and EBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // load vertices into VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        // load indices into EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // inform OpenGL how to interpret the vertex data inside the VBO
        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
        // vertex tangent coords
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

        glBindVertexArray(0);
    }

    void Mesh::render(Shader* shader, const ModelOptions& options) {
        shader->setVec4("color", options.color);
        shader->setFloat("specular", options.specular);
        shader->setVec3("emissive", options.emissive);
        bool hasDiffuse = false;
        bool hasSpecular = false;
        bool hasNormal = false;
        bool hasHeight = false;
        bool hasEmissive = false;
        if (options.useTexture) {
            for (unsigned int i = 0; i < textures.size(); i++) {
                glActiveTexture(GL_TEXTURE0 + i);
                std::string name = textures[i]->type;
                if (name == "texture_diffuse") // diffuse map -> normal texture
                    hasDiffuse = true;
                else if (name == "texture_specular") // specular map -> light intensity
                    hasSpecular = true;
                else if (name == "texture_normal") // normal map -> stores normals, used to create smooth surfaces
                    hasNormal = true;
                else if (name == "texture_height") // height map -> store height information
                    hasHeight = true;
                else if (name == "texture_emissive") // emission map -> emit light (does not illuminate other objects)
                    hasEmissive = true;
                shader->setInt(name, i);
                glBindTexture(GL_TEXTURE_2D, textures[i]->id);
            }
            glActiveTexture(GL_TEXTURE0);
        }
        shader->setBool("hasDiffuse", hasDiffuse);
        shader->setBool("hasSpecular", hasSpecular);
        shader->setBool("hasNormal", hasNormal);
        shader->setBool("hasEmissive", hasEmissive);
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Mesh::renderDepth() {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    bool Mesh::hasTransparency(const ModelOptions& options) {
        if (options.useTexture) {
            for (Texture* t : this->textures) {
                if (t->nrChannels == 4)
                    return true;
            }
            return false;
        } else {
            return options.color.a < 1;
        }
    }

}
