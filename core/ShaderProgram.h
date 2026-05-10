//
// Created by expleoene on 4/1/26.
//

#ifndef VIEWER_SHADERPROGRAM_H
#define VIEWER_SHADERPROGRAM_H
#include <iostream>
#include <string>
#include <glad/glad.h>
#include <unordered_map>

#include "../helper_functions/helper.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace holubiho {
    enum class UBOBinding : GLuint {
        Matrices = 0,
        Lights = 1,
        Fog = 2
    };
    enum class AttributeLocation : GLuint {
        Position = 0,
        Normal = 1,
        TexCoords = 2,
        Color = 3,
        Tangent = 4,
        Bitangent = 5
    };

    enum class UniformType {
        Integer,
        Vec3,
        Vec2,
        Float,
    };

    struct AdditionalShaderInfo {
        std::string uniformName;
        const void* infoPtr = nullptr;
        UniformType type = UniformType::Integer;
    };

    class ShaderProgram {
        GLuint programID = 0;
        mutable std::unordered_map<std::string, GLuint> locations;
        std::vector<AdditionalShaderInfo> shaderInfo;

    public:
        std::string name;
        ShaderProgram(const std::string& name,const std::string& fragmentShaderPath, const std::string& vertexShaderPath) {
            this->name = name;
            std::string vertexSource = Helper::read_file(vertexShaderPath.c_str());
            std::string fragmentSource = Helper::read_file(fragmentShaderPath.c_str());
            programID = createProgram(vertexSource.c_str(),
                                  fragmentSource.c_str());
            GLuint blockIndex = glGetUniformBlockIndex(programID, "Matrices");
            glUniformBlockBinding(programID, blockIndex, static_cast<GLuint>(UBOBinding::Matrices));
            glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "Lights"), static_cast<GLuint>(UBOBinding::Lights));
            glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "Fog"), static_cast<GLuint>(UBOBinding::Fog));
        }

        void Use() const {
            glUseProgram(programID);
        }

        void SetUniformMat4(const char* name, glm::mat4& mat) const {
            glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
        }
        void SetUniformVec2(const char* name, const glm::vec2& vector) const {
            glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(vector));
        }

        void SetUniformVec3(const char* name, const glm::vec3& vector) const {
            glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(vector));
        }
        void SetUniformVec4(const char* name, const glm::vec4& vector) const {
            glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(vector));
        }
        void SetUniformVec4ui(const char* name, const int v0, const int v1, const int v2, const int v3) const {
            glUniform4ui(GetUniformLocation(name), v0, v1, v2, v3);
        }

        void SetUniformInt(const char* name, int value) const {
            glUniform1i(GetUniformLocation(name), value);
        }

        void SetUniformFloat(const char* name, float value) const {
            glUniform1f(GetUniformLocation(name), value);
        }

        void SetUniformMat3(const char* name, glm::mat3& mat) const {
            glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
        }

        GLint GetUniformLocation(const char* name) const {
            std::string nameStr(name);
            if (locations.find(nameStr) != locations.end()) {
                return locations.at(nameStr);
            }
            GLint location = glGetUniformLocation(programID, name);
            locations.emplace(nameStr, static_cast<GLuint>(location));
            if (location == -1) {
                std::cerr << "Warning: Uniform '" << name << "' not found in shader program \""<< this->name<<"\'." << std::endl;
            }else {
                std::cout << "Uniform '" << name << "' location: " << location <<"; in \"" << this->name << "\"."<< std::endl;
            }

            return location;
        }

        void SendAdditionalInfo() {
            for (auto& info : shaderInfo) {
                if (!info.infoPtr) {
                    continue;
                }

                switch (info.type) {
                    case UniformType::Integer:
                        SetUniformInt(info.uniformName.c_str(), *static_cast<const int*>(info.infoPtr));
                        break;
                    case UniformType::Vec3:
                        SetUniformVec3(info.uniformName.c_str(), *static_cast<const glm::vec3*>(info.infoPtr));
                        break;
                    case UniformType::Vec2:
                        SetUniformVec2(info.uniformName.c_str(), *static_cast<const glm::vec2*>(info.infoPtr));
                        break;
                    case UniformType::Float:
                        SetUniformFloat(info.uniformName.c_str(), *static_cast<const float*>(info.infoPtr));
                        break;
                }
            }
        }

        void AddInfo(const std::string& name, const void* ptr, const UniformType type) {
            shaderInfo.push_back({name, ptr, type});
        }

        ~ShaderProgram() {
            if (programID != 0) {
                glDeleteProgram(programID);
            }
        }
    private:


        GLuint compileShader(GLenum type, const char* source) {
            GLuint shader = glCreateShader(type);
            glShaderSource(shader, 1, &source, nullptr);
            glCompileShader(shader);

            int success;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetShaderInfoLog(shader, 512, nullptr, infoLog);
                std::cerr << "Shader Compilation Error:\n" << infoLog << std::endl;
            }
            return shader;
        }

        GLuint createProgram(const char* vShaderCode, const char* fShaderCode) {
            // std::cout << name << ", vertex: " << std::endl<< vShaderCode << std::endl << "Fragment: " << std::endl << fShaderCode << std::endl << std::endl;


            GLuint vShader = compileShader(GL_VERTEX_SHADER, vShaderCode);
            GLuint fShader = compileShader(GL_FRAGMENT_SHADER, fShaderCode);

            GLuint program = glCreateProgram();
            glAttachShader(program, vShader);
            glAttachShader(program, fShader);
            glLinkProgram(program);

            int success;
            glGetProgramiv(program, GL_LINK_STATUS, &success);
            if (!success) {
                char infoLog[512];
                glGetProgramInfoLog(program, 512, nullptr, infoLog);
                std::cerr << "Program Linking Error:\n" << infoLog << std::endl;
            }
            glDeleteShader(vShader);
            glDeleteShader(fShader);
            return program;
        }
    };
}


#endif //VIEWER_SHADERPROGRAM_H
