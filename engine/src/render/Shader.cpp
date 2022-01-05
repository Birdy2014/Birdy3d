#include "render/Shader.hpp"

#include "core/Logger.hpp"
#include "core/ResourceManager.hpp"
#include <cstring>

namespace Birdy3d::render {

    Shader::Shader(const std::string& name)
        : m_name(name) {
        std::string path = core::ResourceManager::get_resource_path(name, core::ResourceManager::ResourceType::SHADER);
        std::string source = core::ResourceManager::read_file(path);
        PreprocessedSources shader_sources = preprocess(source);
        compile(shader_sources);
    }

    bool Shader::check_compile_errors(GLuint shader, GLenum type) {
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
                core::Logger::error("shader compilation error: name: ", m_name + " type: ", type_string, "\n", info_log);
                return true;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, info_log);
                core::Logger::error("program linking error: name: ", m_name, "\n", info_log);
                return true;
            }
        }
        return false;
    }

    Shader::PreprocessedSources Shader::preprocess(std::string shader_source) {
        // include
        const char* include_token = "#include";
        size_t pos = shader_source.find(include_token, 0);
        while (pos != std::string::npos) {
            size_t eol = shader_source.find_first_of('\n', pos);
            size_t path_start = pos + strlen(include_token) + 1;
            std::string include_path = shader_source.substr(path_start, eol - path_start);
            std::string file_content = core::ResourceManager::read_file(core::ResourceManager::get_resource_path(include_path, core::ResourceManager::ResourceType::SHADER));
            shader_source.erase(pos, eol - pos + 1);
            shader_source.insert(pos, file_content);
            pos = shader_source.find(include_token, pos + 1);
        }

        // constants
        auto replace_all = [&](std::string toSearch, std::string replaceStr) {
            size_t pos = shader_source.find(toSearch);
            while (pos != std::string::npos) {
                shader_source.replace(pos, toSearch.size(), replaceStr);
                pos = shader_source.find(toSearch, pos + replaceStr.size());
            }
        };
        replace_all("MAX_DIRECTIONAL_LIGHTS", std::to_string(MAX_DIRECTIONAL_LIGHTS));
        replace_all("MAX_POINTLIGHTS", std::to_string(MAX_POINTLIGHTS));
        replace_all("MAX_SPOTLIGHTS", std::to_string(MAX_SPOTLIGHTS));

        // type
        PreprocessedSources shader_sources;
        const char* type_token = "#type";

        std::size_t next_line;

        for (pos = shader_source.find(type_token, 0); pos != std::string::npos;) {
            std::size_t eol = shader_source.find_first_of('\n', pos);
            next_line = shader_source.find_first_not_of('\n', eol);
            std::size_t type_start = pos + strlen(type_token) + 1;
            std::string type = shader_source.substr(type_start, eol - type_start);

            std::string* preprocessed_shader = nullptr;
            if (type == "vertex")
                preprocessed_shader = &shader_sources.vertex_shader;
            else if (type == "geometry")
                preprocessed_shader = &shader_sources.geometry_shader;
            else if (type == "fragment")
                preprocessed_shader = &shader_sources.fragment_shader;
            else {
                core::Logger::error("Invalid shader type");
            }

            pos = shader_source.find(type_token, next_line);

            // TODO: Add correct line numbers and file names
            // unsigned int line_nr = std::count(shader_source.cbegin(), shader_source.cbegin() + next_line, '\n');
            // preprocessed_shader += "#line " + std::to_string(line_nr + 1) + "\n";

            if (preprocessed_shader->empty())
                *preprocessed_shader += "#version 330 core\n";

            *preprocessed_shader += shader_source.substr(next_line, pos - next_line);
        }
        return shader_sources;
    }

    void Shader::compile(const Shader::PreprocessedSources& shader_sources) {
        m_id = glCreateProgram();

        GLuint vertex_shader = 0, geometry_shader = 0, fragment_shader = 0;

        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        const char* vertex_source_string = shader_sources.vertex_shader.c_str();
        glShaderSource(vertex_shader, 1, &vertex_source_string, nullptr);
        glCompileShader(vertex_shader);
        glAttachShader(m_id, vertex_shader);
        if (check_compile_errors(vertex_shader, GL_VERTEX_SHADER)) {
            glDeleteShader(vertex_shader);
            glDeleteProgram(m_id);
            return;
        }

        if (!shader_sources.geometry_shader.empty()) {
            geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
            const char* geometry_source_string = shader_sources.geometry_shader.c_str();
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
                return;
            }
        }

        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fragment_source_string = shader_sources.fragment_shader.c_str();
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
            return;
        }
        glLinkProgram(m_id);
        check_compile_errors(m_id, 0);

        glDetachShader(m_id, vertex_shader);
        glDeleteShader(vertex_shader);
        if (geometry_shader) {
            glDetachShader(m_id, geometry_shader);
            glDeleteShader(geometry_shader);
        }
        glDetachShader(m_id, fragment_shader);
        glDeleteShader(fragment_shader);
    }

    void Shader::use() const {
        glUseProgram(m_id);
    }

    void Shader::set_bool(const char* name, bool value) const {
        glUniform1i(glGetUniformLocation(m_id, name), (int)value);
    }

    void Shader::set_int(const char* name, int value) const {
        glUniform1i(glGetUniformLocation(m_id, name), value);
    }

    void Shader::set_float(const char* name, float value) const {
        glUniform1f(glGetUniformLocation(m_id, name), value);
    }

    void Shader::set_vec2(const char* name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(m_id, name), 1, &value[0]);
    }

    void Shader::set_vec2(const char* name, float x, float y) const {
        glUniform2f(glGetUniformLocation(m_id, name), x, y);
    }

    void Shader::set_vec3(const char* name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(m_id, name), 1, &value[0]);
    }

    void Shader::set_vec3(const char* name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(m_id, name), x, y, z);
    }

    void Shader::set_vec4(const char* name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(m_id, name), 1, &value[0]);
    }

    void Shader::set_vec4(const char* name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(m_id, name), x, y, z, w);
    }

    void Shader::set_mat2(const char* name, const glm::mat2& mat) const {
        glUniformMatrix2fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_mat3(const char* name, const glm::mat3& mat) const {
        glUniformMatrix3fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_mat4(const char* name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_bool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
    }

    void Shader::set_int(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
    }

    void Shader::set_float(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
    }

    void Shader::set_vec2(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
    }

    void Shader::set_vec2(const std::string& name, float x, float y) const {
        glUniform2f(glGetUniformLocation(m_id, name.c_str()), x, y);
    }

    void Shader::set_vec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
    }

    void Shader::set_vec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
    }

    void Shader::set_vec4(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
    }

    void Shader::set_vec4(const std::string& name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(m_id, name.c_str()), x, y, z, w);
    }

    void Shader::set_mat2(const std::string& name, const glm::mat2& mat) const {
        glUniformMatrix2fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_mat3(const std::string& name, const glm::mat3& mat) const {
        glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_mat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

}
