#pragma once

#include "core/Base.hpp"
#include <set>

namespace Birdy3d::render {

    class Shader {
    public:
        Shader(const std::string& name, std::map<std::string, std::string> params);
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

        bool has_tesselation() const { return m_has_tesselation; }

    private:
        struct PreprocessedSources {
            std::string vertex_shader;
            std::string tesselation_control_shader;
            std::string tesselation_evaluation_shader;
            std::string geometry_shader;
            std::string fragment_shader;
            bool has_tesselation_control_shader { false };
            bool has_tesselation_evaluation_shader { false };
            bool has_geometry_shader { false };

            void operator+=(const PreprocessedSources&);
        };

        std::string m_name;
        std::map<std::string, std::string> m_params;
        std::set<std::string> m_valid_param_names;
        GLuint m_id;
        bool m_has_tesselation;

        bool check_compile_errors(GLuint shader, GLenum type);
        PreprocessedSources preprocess_file(std::string name);
        void compile(const PreprocessedSources& shader_sources);
    };

}
