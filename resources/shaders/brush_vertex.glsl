#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in uint material_ID;
//uniform 
layout(std140) uniform CameraUBO{
    mat4 view;
    mat4 projection;
};
flat out vec3 Normal;
uniform vec3 UV_offset_position;

out vec3 WorldPos;   // World position passed to the fragment shader
//out vec3 Normal;     // Normal vector passed to the fragment shader
//out vec2 uv;
flat out uint matID;
void main() {
    WorldPos = aPos ;
    gl_Position = projection * view * vec4(aPos + UV_offset_position, 1.0) ;
    matID = material_ID;
    Normal = aNormal;
    //uv = normalized_position.yz;
    //Normal = normalize(aNormal); // Normalizing the normal vector
}
