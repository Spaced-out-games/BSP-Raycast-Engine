#pragma once

#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "shader.h"

#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

// Shader manager class that manages multiple shader programs
class shader_manager
{
public:

    Shader get_shader(int shaderID);

    void add_shader(const Shader& s);

    void use(int index) { shaders.at(index).use(); }
    
    //static shader_manager* getInstance();

    shader_manager() {};
    ~shader_manager() = default;

private:
    

    std::vector<Shader> shaders;
    int currentShaderID = -1;

    //static shader_manager* master_shader_manager;
};




// Add a shader to the manager
void shader_manager::add_shader(const Shader& s)
{
    shaders.push_back(s);
}

// Get a shader by ID
Shader shader_manager::get_shader(int shaderID)
{
    return shaders.at(shaderID);

}

#endif