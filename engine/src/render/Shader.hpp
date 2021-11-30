#pragma once

#include "core/Base.hpp"

namespace Birdy3d::render {

    class Shader {
    public:
        static const int MAX_DIRECTIONAL_LIGHTS = 1;
        static const int MAX_POINTLIGHTS = 5;
        static const int MAX_SPOTLIGHTS = 2;

        unsigned int ID;

        Shader(const std::string& name);
        void use() const;
        void set_bool(const char* name, bool value) const;
        void set_int(const char* name, int value) const;
        void set_float(const char* name, float value) const;
        void set_vec2(const char* name, const glm::vec2& value) const;
        void set_vec2(const char* name, float x, float y) const;
        void set_vec3(const char* name, const glm::vec3& value) const;
        void set_vec3(const char* name, float x, float y, float z) const;
        void set_vec4(const char* name, const glm::vec4& value) const;
        void set_vec4(const char* name, float x, float y, float z, float w) const;
        void set_mat2(const char* name, const glm::mat2& mat) const;
        void set_mat3(const char* name, const glm::mat3& mat) const;
        void set_mat4(const char* name, const glm::mat4& mat) const;

        void set_bool(const std::string& name, bool value) const;
        void set_int(const std::string& name, int value) const;
        void set_float(const std::string& name, float value) const;
        void set_vec2(const std::string& name, const glm::vec2& value) const;
        void set_vec2(const std::string& name, float x, float y) const;
        void set_vec3(const std::string& name, const glm::vec3& value) const;
        void set_vec3(const std::string& name, float x, float y, float z) const;
        void set_vec4(const std::string& name, const glm::vec4& value) const;
        void set_vec4(const std::string& name, float x, float y, float z, float w) const;
        void set_mat2(const std::string& name, const glm::mat2& mat) const;
        void set_mat3(const std::string& name, const glm::mat3& mat) const;
        void set_mat4(const std::string& name, const glm::mat4& mat) const;

    private:
        std::string m_name;

        bool check_compile_errors(GLuint shader, GLenum type);
        std::unordered_map<GLenum, std::string> preprocess(std::string shaderSource);
        void compile(std::unordered_map<GLenum, std::string>& shaderSources);
    };

}
