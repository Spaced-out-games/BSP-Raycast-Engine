#version 330 core

out vec4 FragColor; // Output color

in vec2 TexCoord; // Input texture coordinate from the vertex shader

uniform sampler2D texture1; // Texture sampler

void main() {
    // Sample the texture at the given UV coordinate
    vec4 texColor = texture(texture1, TexCoord);

    // Optional: Modify the color before outputting
    // Uncomment and modify the line below if you need to apply custom colors or effects
    // texColor = vec4(1.0, 1.0, 1.0, 0.0); // Example: setting color to white with zero alpha

    // Set the final color output
    FragColor = texColor;
}
