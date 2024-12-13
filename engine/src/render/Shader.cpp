#include "render/Shader.hpp"

#include "core/Logger.hpp"
#include "core/ResourceManager.hpp"
#include <fstream>
#include <regex>

namespace Birdy3d::render {

    Shader::Shader(std::string const& name, std::map<std::string, std::string> params)
        : m_name(name)
        , m_params(params)
    {
        PreprocessedSources shader_sources = preprocess_file(name);

        for (auto const& param : m_params) {
            auto it = std::find(m_valid_param_names.cbegin(), m_valid_param_names.cend(), param.first);
            if (it == m_valid_param_names.cend())
                core::Logger::warn("Shader '{}': invalid parameter '{}'", name, param.first);
        }

        if (!shader_sources.vertex_shader.empty())
            shader_sources.vertex_shader.insert(0, "#version 460 core\n");
        if (!shader_sources.geometry_shader.empty())
            shader_sources.geometry_shader.insert(0, "#version 460 core\n");
        if (!shader_sources.fragment_shader.empty())
            shader_sources.fragment_shader.insert(0, "#version 460 core\n");
        compile(shader_sources);
    }

    bool Shader::check_compile_errors(GLuint shader, GLenum type)
    {
        std::string type_string = "Invalid type";
        if (type == GL_VERTEX_SHADER)
            type_string = "vertex";
        else if (type == GL_GEOMETRY_SHADER)
            type_string = "geometry";
        else if (type == GL_FRAGMENT_SHADER)
            type_string = "fragment";
        else if (type == 0)
            type_string = "program";
        else
            core::Logger::warn("invalid shader type");
        int success;
        char info_log[1024];
        if (type != 0) {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, info_log);
                core::Logger::error("shader compilation error: name: {}, type: {}\n{}", m_name, type_string, info_log);
                return true;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, info_log);
                core::Logger::error("program linking error: name: {}\n{}", m_name, info_log);
                return true;
            }
        }
        return false;
    }

    Shader::PreprocessedSources Shader::preprocess_file(std::string name)
    {
        PreprocessedSources preprocessed_file;

        std::string path = core::ResourceManager::get_resource_path(name, core::ResourceType::SHADER);
        if (path.empty())
            return preprocessed_file;

        std::ifstream stream;
        stream.open(path);

        std::regex const regex_include("#include ([a-zA-Z0-9_./]+)");
        std::regex const regex_type("#type (vertex|geometry|fragment)");
        std::regex const regex_parameter_empty("#parameter ([a-zA-Z0-9_]+)");
        std::regex const regex_parameter_default("#parameter ([a-zA-Z0-9_]+) ([a-zA-Z0-9_\".]+)");

        std::string* current_shader_source = nullptr;
        std::size_t line_number = 1;
        for (std::string line; std::getline(stream, line); ++line_number) {
            std::smatch matches;
            if (std::regex_match(line, matches, regex_include)) {
                auto include_name = matches[1];
                preprocessed_file += preprocess_file(include_name);
                continue;
            }

            if (std::regex_match(line, matches, regex_type)) {
                auto type_name = matches[1];
                if (type_name == "vertex") {
                    current_shader_source = &preprocessed_file.vertex_shader;
                } else if (type_name == "geometry") {
                    preprocessed_file.has_geometry_shader = true;
                    current_shader_source = &preprocessed_file.geometry_shader;
                } else if (type_name == "fragment") {
                    current_shader_source = &preprocessed_file.fragment_shader;
                } else {
                    core::Logger::error("Shader preprocessing error in file '{}': invalid type '{}'", name, static_cast<std::string>(type_name));
                    return {};
                }
                *current_shader_source += "#line " + std::to_string(line_number + 1) + "\n";
                continue;
            }

            if (std::regex_match(line, matches, regex_parameter_empty)) {
                std::string parameter_name = matches[1];
                m_valid_param_names.insert(parameter_name);
                auto define = "#define " + parameter_name + " " + m_params[parameter_name] + "\n";
                preprocessed_file.vertex_shader += define;
                preprocessed_file.geometry_shader += define;
                preprocessed_file.fragment_shader += define;
                continue;
            }

            if (std::regex_match(line, matches, regex_parameter_default)) {
                std::string parameter_name = matches[1];
                std::string parameter_default = matches[2];
                m_valid_param_names.insert(parameter_name);
                auto define = (m_params.count(parameter_name) > 0) ? "#define " + parameter_name + " " + m_params[parameter_name] + "\n"
                                                                   : "#define " + parameter_name + " " + parameter_default + "\n";
                preprocessed_file.vertex_shader += define;
                preprocessed_file.geometry_shader += define;
                preprocessed_file.fragment_shader += define;
                continue;
            }

            if (current_shader_source)
                *current_shader_source += line + "\n";
        }

        return preprocessed_file;
    }

    void Shader::compile(Shader::PreprocessedSources const& shader_sources)
    {
        m_id = glCreateProgram();

        GLuint vertex_shader = 0, geometry_shader = 0, fragment_shader = 0;

        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        char const* vertex_source_string = shader_sources.vertex_shader.c_str();
        glShaderSource(vertex_shader, 1, &vertex_source_string, nullptr);
        glCompileShader(vertex_shader);
        glAttachShader(m_id, vertex_shader);
        if (check_compile_errors(vertex_shader, GL_VERTEX_SHADER)) {
            glDeleteShader(vertex_shader);
            glDeleteProgram(m_id);
            m_id = 0;
            return;
        }

        if (shader_sources.has_geometry_shader) {
            geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
            char const* geometry_source_string = shader_sources.geometry_shader.c_str();
            glShaderSource(geometry_shader, 1, &geometry_source_string, nullptr);
            glCompileShader(geometry_shader);
            glAttachShader(m_id, geometry_shader);
            if (check_compile_errors(geometry_shader, GL_GEOMETRY_SHADER)) {
                if (geometry_shader) {
                    glDeleteShader(geometry_shader);
                    glDetachShader(m_id, vertex_shader);
                }
                glDeleteShader(vertex_shader);
                glDeleteProgram(m_id);
                m_id = 0;
                return;
            }
        }

        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        char const* fragment_source_string = shader_sources.fragment_shader.c_str();
        glShaderSource(fragment_shader, 1, &fragment_source_string, nullptr);
        glCompileShader(fragment_shader);
        glAttachShader(m_id, fragment_shader);
        if (check_compile_errors(fragment_shader, GL_FRAGMENT_SHADER)) {
            glDeleteShader(fragment_shader);
            glDetachShader(m_id, vertex_shader);
            glDeleteShader(vertex_shader);
            if (geometry_shader) {
                glDetachShader(m_id, geometry_shader);
                glDeleteShader(geometry_shader);
            }
            glDeleteProgram(m_id);
            m_id = 0;
            return;
        }
        glLinkProgram(m_id);
        if (check_compile_errors(m_id, 0)) {
            glDetachShader(m_id, vertex_shader);
            glDeleteShader(vertex_shader);
            if (geometry_shader) {
                glDetachShader(m_id, geometry_shader);
                glDeleteShader(geometry_shader);
            }
            glDetachShader(m_id, fragment_shader);
            glDeleteShader(fragment_shader);
            glDeleteProgram(m_id);
            m_id = 0;
            return;
        }

        glDetachShader(m_id, vertex_shader);
        glDeleteShader(vertex_shader);
        if (geometry_shader) {
            glDetachShader(m_id, geometry_shader);
            glDeleteShader(geometry_shader);
        }
        glDetachShader(m_id, fragment_shader);
        glDeleteShader(fragment_shader);
    }

    void Shader::use() const
    {
        if (!check_program_valid())
            return;
        glUseProgram(m_id);
    }

    void Shader::set_bool(char const* name, bool value) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform1i(m_id, glGetUniformLocation(m_id, name), (int)value);
    }

    void Shader::set_int(char const* name, int value) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform1i(m_id, glGetUniformLocation(m_id, name), value);
    }

    void Shader::set_float(char const* name, float value) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform1f(m_id, glGetUniformLocation(m_id, name), value);
    }

    void Shader::set_vec2(char const* name, glm::vec2 const& value) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform2fv(m_id, glGetUniformLocation(m_id, name), 1, &value[0]);
    }

    void Shader::set_vec2(char const* name, float x, float y) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform2f(m_id, glGetUniformLocation(m_id, name), x, y);
    }

    void Shader::set_vec3(char const* name, glm::vec3 const& value) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform3fv(m_id, glGetUniformLocation(m_id, name), 1, &value[0]);
    }

    void Shader::set_vec3(char const* name, float x, float y, float z) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform3f(m_id, glGetUniformLocation(m_id, name), x, y, z);
    }

    void Shader::set_vec4(char const* name, glm::vec4 const& value) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform4fv(m_id, glGetUniformLocation(m_id, name), 1, &value[0]);
    }

    void Shader::set_vec4(char const* name, float x, float y, float z, float w) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform4f(m_id, glGetUniformLocation(m_id, name), x, y, z, w);
    }

    void Shader::set_mat2(char const* name, glm::mat2 const& mat) const
    {
        if (!check_program_valid())
            return;
        glProgramUniformMatrix2fv(m_id, glGetUniformLocation(m_id, name), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_mat3(char const* name, glm::mat3 const& mat) const
    {
        if (!check_program_valid())
            return;
        glProgramUniformMatrix3fv(m_id, glGetUniformLocation(m_id, name), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_mat4(char const* name, glm::mat4 const& mat) const
    {
        if (!check_program_valid())
            return;
        glProgramUniformMatrix4fv(m_id, glGetUniformLocation(m_id, name), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_bool(std::string const& name, bool value) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform1i(m_id, glGetUniformLocation(m_id, name.c_str()), (int)value);
    }

    void Shader::set_int(std::string const& name, int value) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform1i(m_id, glGetUniformLocation(m_id, name.c_str()), value);
    }

    void Shader::set_float(std::string const& name, float value) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform1f(m_id, glGetUniformLocation(m_id, name.c_str()), value);
    }

    void Shader::set_vec2(std::string const& name, glm::vec2 const& value) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform2fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
    }

    void Shader::set_vec2(std::string const& name, float x, float y) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform2f(m_id, glGetUniformLocation(m_id, name.c_str()), x, y);
    }

    void Shader::set_vec3(std::string const& name, glm::vec3 const& value) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform3fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
    }

    void Shader::set_vec3(std::string const& name, float x, float y, float z) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform3f(m_id, glGetUniformLocation(m_id, name.c_str()), x, y, z);
    }

    void Shader::set_vec4(std::string const& name, glm::vec4 const& value) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform4fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
    }

    void Shader::set_vec4(std::string const& name, float x, float y, float z, float w) const
    {
        if (!check_program_valid())
            return;
        glProgramUniform4f(m_id, glGetUniformLocation(m_id, name.c_str()), x, y, z, w);
    }

    void Shader::set_mat2(std::string const& name, glm::mat2 const& mat) const
    {
        if (!check_program_valid())
            return;
        glProgramUniformMatrix2fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_mat3(std::string const& name, glm::mat3 const& mat) const
    {
        if (!check_program_valid())
            return;
        glProgramUniformMatrix3fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_mat4(std::string const& name, glm::mat4 const& mat) const
    {
        if (!check_program_valid())
            return;
        glProgramUniformMatrix4fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::PreprocessedSources::operator+=(PreprocessedSources const& other)
    {
        vertex_shader += other.vertex_shader;
        geometry_shader += other.geometry_shader;
        fragment_shader += other.fragment_shader;
        if (other.has_geometry_shader)
            has_geometry_shader = true;
    }

    [[nodiscard]] bool Shader::check_program_valid() const
    {
        if (m_id == 0) {
            if (!m_printed_error)
                core::Logger::error("attempting to use invalid shader '{}'", m_name);
            m_printed_error = true;
            return false;
        }
        return true;
    }

}
