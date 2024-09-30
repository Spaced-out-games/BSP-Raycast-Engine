#version 330 core

out vec4 FragColor; // Output color

in vec2 TexCoord; // Input texture coordinate from the vertex shader

layout(std140) uniform CameraUBO{
    mat4 view;
    mat4 projection;
};

uniform sampler2D texture1;
//uniform vec2 texture_scale; // Scale for texture coordinates

vec4 get_color(sampler2D map, vec2 uv, vec2 texture_scale) {
    return texture(map, vec2(uv.x * texture_scale.x, mod(uv.y, 0.332) * texture_scale.y));
}

vec4 get_normal(sampler2D map, vec2 uv, vec2 texture_scale) {
    return texture(map, vec2(uv.x * texture_scale.x, (mod(uv.y, 0.332) * texture_scale.y)) + vec2(0.333, 0.0));
}

vec4 get_AODPR(sampler2D map, vec2 uv, vec2 texture_scale) {
    return texture(map, vec2(uv.x * texture_scale.x, (mod(uv.y, 0.332) * texture_scale.y)) + vec2(0.666, 0.0));
}

vec2 parallax_occlusion_mapping(vec2 uv, vec3 viewDir, float heightScale, int maxSteps, float minStepSize, vec2 texture_scale) {
    vec2 offset = uv;
    float depth = 0.0;
    vec2 currentUV = uv;

    for (int i = 0; i < maxSteps; ++i) {
        vec4 heightData = get_AODPR(texture1, currentUV, texture_scale);
        float height = heightData.g; // Use the red channel for height
        float currentDepth = height * heightScale;

        depth += currentDepth;
        offset -= viewDir.xy * minStepSize;
        currentUV = uv + offset;

        if (depth > length(viewDir)) {
            return currentUV;
        }
    }

    return currentUV;
}

void main() {
    float height_scalar = 1.0;
    int max_steps = 3;
    float min_step_size = 0.1;
    vec3 cameraPosition = -vec3(view[3].xyz); // Camera position in view space
    vec2 uv = TexCoord * vec2(3,1); // Original texture coordinates

    // Calculate view direction
    vec3 viewDir = normalize(cameraPosition - vec3(0.0, 0.0, 0.0));

    // Apply parallax occlusion mapping
    //vec2 parallaxUV = parallax_occlusion_mapping(uv, viewDir, height_scalar, max_steps, min_step_size, vec2(1,1));

    // Sample color from the adjusted UV coordinates
    vec4 color = get_color(texture1, uv, vec2(1,1));

    FragColor = color; // Final color output
}
