
// Contains source for multiple shaders. Might be better to make these into actual files in the future
namespace shaderCollection
{
    struct ShaderSources
    {
        const char* vertex_source;
        const char* frag_source;
    };

    // Define static instances for different shader sources
    static const ShaderSources tex3D_shader_sources = {
        R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;    // Vertex position
        layout(location = 1) in vec2 aTexCoord; // Texture coordinate
        layout(std140) uniform CameraUBO {
            mat4 view;
            mat4 projection;
        };
        out vec2 TexCoord; // Output texture coordinate to the fragment shader

        void main() {
            gl_Position = projection * view * vec4(aPos, 1.0);
            TexCoord = aTexCoord;
        }
        )",

        R"(
        #version 330 core
        out vec4 FragColor; // Output color

        in vec2 TexCoord; // Input texture coordinate from the vertex shader

        uniform sampler2D texture1; // Texture sampler

        void main() {
            FragColor = texture(texture1, TexCoord); // Sample the texture and output the color
        }
        )"
    };
}