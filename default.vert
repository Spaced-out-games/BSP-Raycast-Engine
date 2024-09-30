// Vertex shader for 3D data with view / projection matrices
#version 330 core
layout(location = 0) in vec3 aPos;

layout(std140) uniform CameraUBO {
	mat4 view;
	mat4 projection;
};

void main()
{
	gl_Position = projection * view * vec4(aPos, 1.0);
}