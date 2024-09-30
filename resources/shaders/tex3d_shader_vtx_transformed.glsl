// Vertex shader for 3D data with view / projection matrices
#version 330 core
layout(location = 0) in vec3 aPos;    // Vertex position
layout(location = 1) in vec2 aTexCoord; // Texture coordinate
layout(std140) uniform CameraUBO{
    mat4 view;
    mat4 projection;
};
uniform mat4 model;
out vec2 TexCoord; // Output texture coordinate to the fragment shader

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}