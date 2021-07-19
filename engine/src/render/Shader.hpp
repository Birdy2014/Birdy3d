#pragma once

#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace Birdy3d {

    class Shader {
    public:
        static const int MAX_DIRECTIONAL_LIGHTS = 1;
        static const int MAX_POINTLIGHTS = 5;
        static const int MAX_SPOTLIGHTS = 2;

        unsigned int ID;

        Shader(const std::string& shaderSource, const std::string& name);
        void use() const;
        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setVec2(const std::string& name, const glm::vec2& value) const;
        void setVec2(const std::string& name, float x, float y) const;
        void setVec3(const std::string& name, const glm::vec3& value) const;
        void setVec3(const std::string& name, float x, float y, float z) const;
        void setVec4(const std::string& name, const glm::vec4& value) const;
        void setVec4(const std::string& name, float x, float y, float z, float w) const;
        void setMat2(const std::string& name, const glm::mat2& mat) const;
        void setMat3(const std::string& name, const glm::mat3& mat) const;
        void setMat4(const std::string& name, const glm::mat4& mat) const;

    private:
        std::string m_name;

        bool checkCompileErrors(GLuint shader, GLenum type);
        std::unordered_map<GLenum, std::string> preprocess(std::string shaderSource);
        void compile(std::unordered_map<GLenum, std::string>& shaderSources);
    };

}
