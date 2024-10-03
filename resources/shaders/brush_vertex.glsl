#version 330 core  // GLSL 330 for OpenGL 3.3

// Input vertex attributes
layout(location = 0) in vec3 aPos;      // Vertex position
layout(location = 1) in vec3 aNormal;   // Normal for the vertex
layout(location = 2) in uint aMaterialID; // Material ID for the vertex

layout(std140) uniform CameraUBO{
    mat4 view;           // View matrix
    mat4 projection;     // Projection matrix
};

out vec3 WorldPos;      // World position passed to the fragment shader
flat out vec3 Normal;   // Normal vector passed to the fragment shader
flat out uint matID;    // Material ID passed to the fragment shader

uniform vec3 UV_offset_position; // Optional UV offset position

void main() {
    // Adjust world position using UV offset
    WorldPos = aPos + UV_offset_position;
    Normal = normalize(aNormal);        // Normalize the normal vector
    matID = aMaterialID;                // Pass the material ID to the fragment shader

    // Calculate the final position using the view and projection matrices
    gl_Position = projection * view * vec4(WorldPos, 1.0); // Transform position to clip space
}
