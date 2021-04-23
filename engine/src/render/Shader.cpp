#include "render/Shader.hpp"

#include "core/Logger.hpp"
#include "core/RessourceManager.hpp"
#include <algorithm>
#include <cstring>

namespace Birdy3d {

    Shader::Shader(const std::string& shaderSource, const std::string& name)
        : name(name) {
        std::unordered_map<GLenum, std::string> shaderSources = preprocess(shaderSource);
        compile(shaderSources);
    }

    bool Shader::checkCompileErrors(GLuint shader, GLenum type) {
        std::string typeString = "Invalid type";
        if (type == GL_VERTEX_SHADER)
            typeString = "vertex";
        else if (type == GL_GEOMETRY_SHADER)
            typeString = "geometry";
        else if (type == GL_FRAGMENT_SHADER)
            typeString = "fragment";
        else if (type == 0)
            typeString = "program";
        else
            Logger::warn("invalid shader type");
        int success;
        char infoLog[1024];
        if (type != 0) {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                Logger::warn("shader compilation error: name: " + name + " type: " + typeString + "\n" + infoLog);
                return true;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                Logger::warn("program linking error: name: " + name + "\n" + infoLog);
                return true;
            }
        }
        return false;
    }

    std::unordered_map<GLenum, std::string> Shader::preprocess(std::string shaderSource) {
        // include
        const char* includeToken = "#include";
        size_t pos = shaderSource.find(includeToken, 0);
        while (pos != std::string::npos) {
            size_t eol = shaderSource.find_first_of('\n', pos);
            size_t pathStart = pos + strlen(includeToken) + 1;
            std::string includePath = shaderSource.substr(pathStart, eol - pathStart);
            std::string fileContent = RessourceManager::readFile(RessourceManager::getRessourcePath(includePath, RessourceManager::RessourceType::SHADER));
            shaderSource.erase(pos, eol - pos + 1);
            shaderSource.insert(pos, fileContent);
            pos = shaderSource.find(includeToken, pos + 1);
        }

        // constants
        auto replaceAll = [&](std::string toSearch, std::string replaceStr) {
            size_t pos = shaderSource.find(toSearch);
            while (pos != std::string::npos) {
                shaderSource.replace(pos, toSearch.size(), replaceStr);
                pos = shaderSource.find(toSearch, pos + replaceStr.size());
            }
        };
        replaceAll("MAX_DIRECTIONAL_LIGHTS", std::to_string(MAX_DIRECTIONAL_LIGHTS));
        replaceAll("MAX_POINTLIGHTS", std::to_string(MAX_POINTLIGHTS));
        replaceAll("MAX_SPOTLIGHTS", std::to_string(MAX_SPOTLIGHTS));

        // type
        std::unordered_map<GLenum, std::string> shaderSources;
        const char* typeToken = "#type";
        pos = shaderSource.find(typeToken, 0);
        while (pos != std::string::npos) {
            size_t eol = shaderSource.find_first_of('\n', pos);
            size_t nextLine = shaderSource.find_first_not_of('\n', eol);
            size_t typeStart = pos + strlen(typeToken) + 1;
            std::string type = shaderSource.substr(typeStart, eol - typeStart);

            GLenum typeEnum;
            if (type == "vertex")
                typeEnum = GL_VERTEX_SHADER;
            else if (type == "geometry")
                typeEnum = GL_GEOMETRY_SHADER;
            else if (type == "fragment")
                typeEnum = GL_FRAGMENT_SHADER;
            else
                Logger::warn("Invalid shader type");

            pos = shaderSource.find(typeToken, nextLine);
            unsigned int lineNr = std::count(shaderSource.begin(), shaderSource.begin() + nextLine, '\n');

            shaderSources[typeEnum] = shaderSource.substr(nextLine, (pos == std::string::npos ? shaderSources.size() : pos) - nextLine);
            shaderSources[typeEnum].insert(shaderSources[typeEnum].find_first_of('\n') + 1, "#line " + std::to_string(lineNr + 1) + '\n');
        }
        return shaderSources;
    }

    void Shader::compile(std::unordered_map<GLenum, std::string>& shaderSources) {
        ID = glCreateProgram();
        GLuint shaders[shaderSources.size()];
        size_t shaderNr = 0;
        for (std::pair<GLenum, std::string> s : shaderSources) {
            GLuint shader = glCreateShader(s.first);
            const char* sourceString = s.second.c_str();
            glShaderSource(shader, 1, &sourceString, nullptr);
            glCompileShader(shader);
            if (checkCompileErrors(shader, s.first)) {
                glDeleteShader(shader);
                for (size_t i = 0; i < shaderNr; i++) {
                    glDetachShader(ID, shaders[i]);
                    glDeleteShader(shaders[i]);
                    glDeleteProgram(ID);
                    return;
                }
            }
            glAttachShader(ID, shader);
            shaders[shaderNr++] = shader;
        }
        glLinkProgram(ID);
        checkCompileErrors(ID, 0);
        while (shaderNr-- > 0) {
            glDetachShader(ID, shaders[shaderNr]);
            glDeleteShader(shaders[shaderNr]);
        }
    }

    void Shader::use() {
        glUseProgram(ID);
    }

    void Shader::setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void Shader::setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Shader::setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void Shader::setVec2(const std::string& name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }

    void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void Shader::setVec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void Shader::setVec4(const std::string& name, float x, float y, float z, float w) {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    void Shader::setMat2(const std::string& name, const glm::mat2& mat) const {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::setMat3(const std::string& name, const glm::mat3& mat) const {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

}
