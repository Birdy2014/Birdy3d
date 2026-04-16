#pragma once

#include "core/Base.hpp"
#include <set>

namespace Birdy3d::render {

    // TODO: Create a single set_uniform template

    class Shader {
    public:
        Shader(std::string const& name, std::map<std::string, std::string> params);
        unsigned int id() const { return m_id; }
        void use() const;
        void set_bool(char const* name, bool value) const;
        void set_int(char const* name, int value) const;
        void set_float(char const* name, float value) const;
        void set_vec2(char const* name, glm::vec2 const& value) const;
        void set_vec3(char const* name, glm::vec3 const& value) const;
        void set_vec4(char const* name, glm::vec4 const& value) const;
        void set_mat2(char const* name, glm::mat2 const& mat) const;
        void set_mat3(char const* name, glm::mat3 const& mat) const;
        void set_mat4(char const* name, glm::mat4 const& mat) const;

        void set_bool(std::string const& name, bool value) const;
        void set_int(std::string const& name, int value) const;
        void set_float(std::string const& name, float value) const;
        void set_vec2(std::string const& name, glm::vec2 const& value) const;
        void set_vec3(std::string const& name, glm::vec3 const& value) const;
        void set_vec4(std::string const& name, glm::vec4 const& value) const;
        void set_mat2(std::string const& name, glm::mat2 const& mat) const;
        void set_mat3(std::string const& name, glm::mat3 const& mat) const;
        void set_mat4(std::string const& name, glm::mat4 const& mat) const;

        int get_uniform_location(char const* name) const { return glGetUniformLocation(m_id, name); }

        template <typename T>
        void set_uniform(int location, T value) const;
        template <>
        void set_uniform(int location, bool value) const;
        template <>
        void set_uniform(int location, int value) const;
        template <>
        void set_uniform(int location, float value) const;
        template <>
        void set_uniform(int location, glm::vec4 value) const;

    private:
        struct PreprocessedSources {
            std::string vertex_shader;
            std::string geometry_shader;
            std::string fragment_shader;
            bool has_geometry_shader{false};

            void operator+=(PreprocessedSources const&);
        };

        std::string m_name;
        std::map<std::string, std::string> m_params;
        std::set<std::string> m_valid_param_names;
        GLuint m_id;
        mutable bool m_printed_error = false;
        std::map<std::string, GLint, std::less<void>> m_uniform_locations;

        bool check_compile_errors(GLuint shader, GLenum type);
        PreprocessedSources preprocess_file(std::string name);
        void compile(PreprocessedSources const& shader_sources);
        [[nodiscard]] bool check_program_valid() const;
    };

}
