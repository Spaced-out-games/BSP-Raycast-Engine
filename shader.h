#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "glm_master.h"
#include "shader_collection.h"

struct Shader {
    GLuint ShaderProgram; /// The ID of the shader program.

    /**
     * @brief Default constructor for Shader.
     */
    Shader() : ShaderProgram(0) {}

    /**
     * @brief Constructs a Shader object and initializes it with the given vertex and fragment shader sources.
     *
     * @param vertexShaderSource The source code for the vertex shader.
     * @param fragmentShaderSource The source code for the fragment shader.
     */
    Shader(const char* vertexShaderSource, const char* fragmentShaderSource) {
        init(vertexShaderSource, fragmentShaderSource);
    }

    /**
     * @brief Initializes the Shader with the given vertex and fragment shader sources.
     *
     * @param vertexShaderSource The source code for the vertex shader.
     * @param fragmentShaderSource The source code for the fragment shader.
     */
    void init(const char* vertexShaderSource, const char* fragmentShaderSource) {
        ShaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
        std::cout << "2. Create a shader either from a file or from source code.\n";
    }

    /**
     * @brief Initializes the Shader with default vertex and fragment shader sources.
     */
    void init() {
        ShaderProgram = createShaderProgram(shaderCollection::tex3D_shader_sources.vertex_source, shaderCollection::tex3D_shader_sources.frag_source);
        std::cout << "2. Create a default shader program.\n";
    }

    /**
     * @brief Destructor for Shader.
     *
     * Cleans up the shader program by deleting it.
     */
    ~Shader() {
        std::cout << "Destroyed shader with ID: " << ShaderProgram << std::endl;
        if (ShaderProgram != 0) {
            glDeleteProgram(ShaderProgram);
        }
    }

    /**
     * @brief Compiles a shader from source code.
     *
     * @param ShaderType The type of shader to compile (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER).
     * @param ShaderSource The source code of the shader.
     * @return The ID of the compiled shader.
     */
    GLuint compileShader(GLenum ShaderType, const char* ShaderSource) {
        GLuint shader = glCreateShader(ShaderType);
        glShaderSource(shader, 1, &ShaderSource, nullptr);
        glCompileShader(shader);

        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "Shader Compilation Error: " << infoLog << std::endl;
        }

        return shader;
    }

    /**
     * @brief Creates a shader program by compiling and linking vertex and fragment shaders.
     *
     * @param vertexShaderSource The source code for the vertex shader.
     * @param fragmentShaderSource The source code for the fragment shader.
     * @return The ID of the created shader program.
     */
    GLuint createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
        GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        GLint success;
        GLchar infoLog[512];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            std::cerr << "Shader Program Linking Error: " << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return program;
    }

    /**
     * @brief Use the shader program for rendering.
     *
     * This function activates the shader program, making it the current one for rendering.
     */
    void use() const {
        #ifdef _DEBUG
        //std::cout << "Using shader with ID: " << ShaderProgram << '\n';
        //std::cout << "5. Use the shader associated with this Mesh3D by calling glUseProgram with the shader's program ID.\n";
        #endif
        glUseProgram(ShaderProgram);
    }

    /**
     * @brief Sets a uniform matrix4 value in the shader program.
     *
     * @param uniform_ID The name of the uniform variable.
     * @param matrix4 The mat4 value to set.
     */
    void setUniform(const char* uniform_ID, const mat4& matrix4) const {
        CHECK_GL_ERROR();

        GLint uniformCode = glGetUniformLocation(ShaderProgram, uniform_ID);
        #ifdef _DEBUG
        if (uniformCode != -1) {
            //std::cout << "6. Set any required uniforms.\n";
            glUniformMatrix4fv(uniformCode, 1, GL_FALSE, glm::value_ptr(matrix4));
        }
        else {
            std::cerr << "WARNING: Uniform '" << uniform_ID << "' not found in shader program " << ShaderProgram << std::endl;
        }
        #else
        glUniformMatrix4fv(uniformCode, 1, GL_FALSE, glm::value_ptr(matrix4));
        #endif
    }
    void setUniform(const char* uniform_ID, const int& integer) const {
        CHECK_GL_ERROR();

        GLint uniformCode = glGetUniformLocation(ShaderProgram, uniform_ID);
        #ifdef _DEBUG
        if (uniformCode != -1) {
            // Set the integer uniform correctly
            glUniform1i(uniformCode, integer);
        }
        else {
            std::cerr << "WARNING: Uniform '" << uniform_ID << "' not found in shader program " << ShaderProgram << std::endl;
        }
        #else
        glUniform1i(uniformCode, integer);
        #endif
        }

    /**
     * @brief Sets a uniform vec3 value in the shader program.
     *
     * @param uniform_ID The name of the uniform variable.
     * @param vector3 The vec3 value to set.
     */
    void setUniform(const char* uniform_ID, const vec3& vector3) const {
        GLint uniformCode = glGetUniformLocation(ShaderProgram, uniform_ID);
        #ifdef _DEBUG
        if (uniformCode != -1) {
            //std::cout << "6. Set any required uniforms.\n";
            glUniform3f(uniformCode, vector3.x, vector3.y, vector3.z);
        }
        else {
            std::cerr << "WARNING: Uniform '" << uniform_ID << "' not found in shader program " << ShaderProgram << std::endl;
        }
        #else
        glUniform3f(uniformCode, vector3.x, vector3.y, vector3.z);
        #endif
    }

    /**
     * @brief Loads shader source code from a file.
     *
     * @param filename The path to the shader file.
     * @return The source code read from the file.
     */
    static std::string loadShaderSourceFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open shader file: " << filename << std::endl;
            return "";
        }

        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    /**
     * @brief Sets up the shader using source code loaded from files.
     *
     * @param vertexShaderFile Path to the vertex shader source file.
     * @param fragmentShaderFile Path to the fragment shader source file.
     */
    void initFromFiles(const char* vertexShaderFile, const char* fragmentShaderFile) {
        std::string vertexSource = loadShaderSourceFromFile(vertexShaderFile);
        std::string fragmentSource = loadShaderSourceFromFile(fragmentShaderFile);
        if (vertexSource.empty() || fragmentSource.empty()) {
            std::cerr << "Failed to load shader source from files." << std::endl;
            return;
        }

        ShaderProgram = createShaderProgram(vertexSource.c_str(), fragmentSource.c_str());
        std::cout << "2. Create a shader from files.\n";
    }



};






#endif // SHADER_H
