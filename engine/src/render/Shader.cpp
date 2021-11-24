#include "render/Shader.hpp"

#include "core/Logger.hpp"
#include "core/ResourceManager.hpp"
#include <cstring>

namespace Birdy3d {

    Shader::Shader(const std::string& name)
        : m_name(name) {
        std::string path = ResourceManager::get_resource_path(name, ResourceManager::ResourceType::SHADER);
        std::string source = ResourceManager::read_file(path);
        std::unordered_map<GLenum, std::string> shader_sources = preprocess(source);
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
            Logger::warn("invalid shader type");
        int success;
        char info_log[1024];
        if (type != 0) {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, info_log);
                Logger::warn("shader compilation error: name: ", m_name + " type: ", type_string, "\n", info_log);
                return true;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, info_log);
                Logger::warn("program linking error: name: ", m_name, "\n", info_log);
                return true;
            }
        }
        return false;
    }

    std::unordered_map<GLenum, std::string> Shader::preprocess(std::string shaderSource) {
        // include
        const char* include_token = "#include";
        size_t pos = shaderSource.find(include_token, 0);
        while (pos != std::string::npos) {
            size_t eol = shaderSource.find_first_of('\n', pos);
            size_t path_start = pos + strlen(include_token) + 1;
            std::string include_path = shaderSource.substr(path_start, eol - path_start);
            std::string file_content = ResourceManager::read_file(ResourceManager::get_resource_path(include_path, ResourceManager::ResourceType::SHADER));
            shaderSource.erase(pos, eol - pos + 1);
            shaderSource.insert(pos, file_content);
            pos = shaderSource.find(include_token, pos + 1);
        }

        // constants
        auto replace_all = [&](std::string toSearch, std::string replaceStr) {
            size_t pos = shaderSource.find(toSearch);
            while (pos != std::string::npos) {
                shaderSource.replace(pos, toSearch.size(), replaceStr);
                pos = shaderSource.find(toSearch, pos + replaceStr.size());
            }
        };
        replace_all("MAX_DIRECTIONAL_LIGHTS", std::to_string(MAX_DIRECTIONAL_LIGHTS));
        replace_all("MAX_POINTLIGHTS", std::to_string(MAX_POINTLIGHTS));
        replace_all("MAX_SPOTLIGHTS", std::to_string(MAX_SPOTLIGHTS));

        // type
        std::unordered_map<GLenum, std::string> shader_sources;
        const char* type_token = "#type";
        pos = shaderSource.find(type_token, 0);
        while (pos != std::string::npos) {
            size_t eol = shaderSource.find_first_of('\n', pos);
            size_t next_line = shaderSource.find_first_not_of('\n', eol);
            size_t type_start = pos + strlen(type_token) + 1;
            std::string type = shaderSource.substr(type_start, eol - type_start);

            GLenum type_enum;
            if (type == "vertex")
                type_enum = GL_VERTEX_SHADER;
            else if (type == "geometry")
                type_enum = GL_GEOMETRY_SHADER;
            else if (type == "fragment")
                type_enum = GL_FRAGMENT_SHADER;
            else
                Logger::warn("Invalid shader type");

            pos = shaderSource.find(type_token, next_line);
            unsigned int line_nr = std::count(shaderSource.begin(), shaderSource.begin() + next_line, '\n');

            shader_sources[type_enum] = shaderSource.substr(next_line, (pos == std::string::npos ? shader_sources.size() : pos) - next_line);
            shader_sources[type_enum].insert(shader_sources[type_enum].find_first_of('\n') + 1, "#line " + std::to_string(line_nr + 1) + '\n');
        }
        return shader_sources;
    }

    void Shader::compile(std::unordered_map<GLenum, std::string>& shaderSources) {
        ID = glCreateProgram();
        std::vector<GLuint> shaders;
        for (std::pair<GLenum, std::string> s : shaderSources) {
            GLuint shader = glCreateShader(s.first);
            const char* source_string = s.second.c_str();
            glShaderSource(shader, 1, &source_string, nullptr);
            glCompileShader(shader);
            if (check_compile_errors(shader, s.first)) {
                glDeleteShader(shader);
                for (auto s : shaders) {
                    glDetachShader(ID, s);
                    glDeleteShader(s);
                    glDeleteProgram(ID);
                    return;
                }
            }
            glAttachShader(ID, shader);
            shaders.push_back(shader);
        }
        glLinkProgram(ID);
        check_compile_errors(ID, 0);
        for (auto s : shaders) {
            glDetachShader(ID, s);
            glDeleteShader(s);
        }
    }

    void Shader::use() const {
        glUseProgram(ID);
    }

    void Shader::set_bool(const char* name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name), (int)value);
    }

    void Shader::set_int(const char* name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name), value);
    }

    void Shader::set_float(const char* name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name), value);
    }

    void Shader::set_vec2(const char* name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(ID, name), 1, &value[0]);
    }

    void Shader::set_vec2(const char* name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name), x, y);
    }

    void Shader::set_vec3(const char* name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(ID, name), 1, &value[0]);
    }

    void Shader::set_vec3(const char* name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name), x, y, z);
    }

    void Shader::set_vec4(const char* name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(ID, name), 1, &value[0]);
    }

    void Shader::set_vec4(const char* name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
    }

    void Shader::set_mat2(const char* name, const glm::mat2& mat) const {
        glUniformMatrix2fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_mat3(const char* name, const glm::mat3& mat) const {
        glUniformMatrix3fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_mat4(const char* name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_bool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void Shader::set_int(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Shader::set_float(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void Shader::set_vec2(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void Shader::set_vec2(const std::string& name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }

    void Shader::set_vec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void Shader::set_vec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    void Shader::set_vec4(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void Shader::set_vec4(const std::string& name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    void Shader::set_mat2(const std::string& name, const glm::mat2& mat) const {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_mat3(const std::string& name, const glm::mat3& mat) const {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::set_mat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

}
