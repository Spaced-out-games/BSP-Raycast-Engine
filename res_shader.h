#pragma once
#include <GL/glew.h>
#include <iostream>
#include "resource.h"
#include <iostream>
#include <fstream>
#include <sstream>


// all functions should have two overrides, minimum...
// One for the contained ShaderProgram, as well as static alternatives that work using an external GLuint

struct res_shader : public Resource
{
    GLuint ShaderProgram = 0; // The ID of the shader program.

    //virtual void load(const std::string& path) = 0;
    //virtual void save(const std::string& path) = 0;
    //virtual void use() = 0;                       // Pure virtual function for using the resource

    // Copy function to be overridden in derived classes
    //virtual Resource* copy() override {};

    // Get's the name of the resource type
    const char* get_name() const { return "res_shader"; }


    // Default the shader to slot 0
    res_shader() = default;


    // load a file
    res_shader(const std::string& file_path);

    res_shader(const std::string& vertex_path, const std::string& fragment_path);

    // compile both shaders
    GLuint compile_all(const char* vertex_source, const char* fragment_source);

    GLuint compile(GLenum shader_t, const char* shader_source);

    // Use the shader
    void use() override;

    static void use(GLuint program_ID);

    
    static GLuint link(GLuint vertex_shader, GLuint fragment_shader);


    // set a uniform value
    template <typename T>
    void set_uniform(const std::string& name, const T& value);

    template <typename T>
    static void set_uniform(GLuint target_program, const std::string& name, const T& value);

    // Per the naming convention, both files must end in .frag and .vtx, respectively, with the same base name, and in the same directory. 
    void load(const std::string& path) override;


    // NEEDS IMPLEMENTED...
    void load(const std::string& vertex_path, const std::string& fragment_path) {};

    // no-op save function. Although Shader derives from Resource, there's not a need for saving functionality
    void save(const std::string& path) override {}


    GLuint load_program(const std::string& file_base);

private:
    std::string load_source(const std::string& file_name);

};



void res_shader::load(const std::string& path)
{
    ShaderProgram = load_program(path);
}

res_shader::res_shader(const std::string& file_path)
{
    load(file_path);
}

res_shader::res_shader(const std::string& vertex_path, const std::string& fragment_path)
{

    // load the sources
    std::string vertex_source = load_source(vertex_path);
    std::string fragment_source = load_source(fragment_path);

    // create the shader program

    ShaderProgram = compile_all(vertex_source.c_str(), fragment_source.c_str());

}

void res_shader::use()
{
    #ifdef _DEBUG
    // MIght move this to the in-engine console eventually
    std::cout << "shader " << ShaderProgram << "used.\n";
    #endif
    glUseProgram(ShaderProgram);
}

void res_shader::use(GLuint program_ID)
{
    #ifdef _DEBUG
    // MIght move this to the in-engine console eventually
    std::cout << "shader " << program_ID << "used.\n";
    #endif
    glUseProgram(program_ID);
}























GLuint res_shader::load_program(const std::string& file_base)
{
    // load the sources
    std::string vertex_source = load_source(file_base + ".vtx");
    std::string fragment_source = load_source(file_base + ".frag");

    // compile them
    GLuint program = compile_all(vertex_source.c_str(), fragment_source.c_str());

    return program;
    
}



GLuint res_shader::compile(GLenum shader_t, const char* shader_source)
{
    // compilation
    GLuint shader = glCreateShader(shader_t);
    glShaderSource(shader, 1, &shader_source, nullptr);
    glCompileShader(shader);

    // error checking...only important in development builds
    #ifdef _DEBUG

    GLint success;
    GLchar info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        std::cerr << "<SHADER COMPILATION FAILURE>\n" << info_log << "\n";
    }
    #endif
    return shader;
}

GLuint res_shader::compile_all(const char* vertex_source, const char* fragment_source)
{
    // compile the sources
    GLuint vertex_shader = compile(GL_VERTEX_SHADER, vertex_source);
    GLuint fragment_shader = compile(GL_FRAGMENT_SHADER, fragment_source);

    // link them
    GLuint program = link(vertex_shader, fragment_shader);

    // return the program
    return program;
}


/**
     * @brief Links two shaders
     *
     * @param vertex_shader the compiled vertex shader
     * @param fragment_shader the compiled fragment shader
     * @return the program code from linking the two programs
     */
GLuint res_shader::link(GLuint vertex_shader, GLuint fragment_shader)
{
    // make a new OpenGL program and link it
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // some debugging functionality

    #ifdef _DEBUG
    GLint success;
    GLchar info_log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, nullptr, info_log);
        std::cerr << "<LINKER ERROR>\n----------------------------------------------------------------------------------------" << info_log << "\n----------------------------------------------------------------------------------------\n";
    }
    #endif

    return program;
}




template <typename T>
static void res_shader::set_uniform(GLuint target_program, const std::string& name, const T& value) {
    GLint location = glGetUniformLocation(target_program, name.c_str());

    // Use std::enable_if to handle different types
    if constexpr (std::is_same<T, float>::value) {
        glUniform1f(location, value);
    }
    else if constexpr (std::is_same<T, int>::value) {
        glUniform1i(location, value);
    }
    else if constexpr (std::is_same<T, glm::vec2>::value) {
        glUniform2fv(location, 1, glm::value_ptr(value));
    }
    else if constexpr (std::is_same<T, glm::vec3>::value) {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
    else if constexpr (std::is_same<T, glm::vec4>::value) {
        glUniform4fv(location, 1, glm::value_ptr(value));
    }
    else if constexpr (std::is_same<T, glm::mat2>::value) {
        glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
    else if constexpr (std::is_same<T, glm::mat3>::value) {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
    else if constexpr (std::is_same<T, glm::mat4>::value) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
    else {
        static_assert(!std::is_same<T, T>::value, "Unsupported uniform type");
    }
}

template <typename T>
void res_shader::set_uniform(const std::string& name, const T& value)
{
    set_uniform<T>(ShaderProgram, name, value);
}


/**
     * @brief Loads shader source code from a file.
     *
     * @param filename The path to the shader file.
     * @return The source code read from the file.
     */
std::string res_shader::load_source(const std::string& file_name)
{
    std::ifstream file(file_name);
    if(!file.is_open())
    
    {
        std::cerr << "File '" << file_name << "' could not be found\n";
        return "";
    }
    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}
