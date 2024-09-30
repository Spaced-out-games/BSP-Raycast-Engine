#version 330 core

out vec4 FragColor;  // Output color
flat in uint matID;  // Input material ID
flat in vec3 Normal; // Normal vector from the vertex shader
in vec3 WorldPos;    // Input world position from vertex shader

uniform sampler2D texture1;  // Texture sampler
uniform vec3 lightPos;       // Position of the light source
const float num_textures_horizontal = 3.0;  // Number of textures in a row
const float num_textures_vertical = 3.0;    // Number of textures in a column
const float lightMultiplier = 1.0;          // Light intensity multiplier
const float distanceFactor = 1.0 / 20.0;    // Distance factor for falloff

// Function to calculate triplanar texture coordinates
vec4 sampleTriplanar(in vec3 worldPos, in vec3 normal) {
    // Normalize the normal vector
    vec3 absNormal = abs(normal);

    // Avoid division by zero by clamping the normal components
    absNormal = max(absNormal, vec3(0.001));

    // Sample the texture on each axis projection
    vec2 uvX = worldPos.yz / absNormal.x;
    vec2 uvY = worldPos.xz / absNormal.y;
    vec2 uvZ = worldPos.xy / absNormal.z;

    // Calculate texture grid offset based on matID
    vec2 uv_offset = vec2(mod(float(matID), num_textures_horizontal),
        floor(float(matID) / num_textures_horizontal)) /
        vec2(num_textures_horizontal, num_textures_vertical);

    // Normalize and scale the UV coordinates
    vec2 scaled_uvX = mod(uvX, 1.0) / vec2(num_textures_horizontal, num_textures_vertical);
    vec2 scaled_uvY = mod(uvY, 1.0) / vec2(num_textures_horizontal, num_textures_vertical);
    vec2 scaled_uvZ = mod(uvZ, 1.0) / vec2(num_textures_horizontal, num_textures_vertical);

    // Add the offset to place the texture in the grid
    vec2 final_uvX = scaled_uvX + uv_offset;
    vec2 final_uvY = scaled_uvY + uv_offset;
    vec2 final_uvZ = scaled_uvZ + uv_offset;

    // Sample the texture for each axis
    vec4 texColorX = texture(texture1, final_uvX);
    vec4 texColorY = texture(texture1, final_uvY);
    vec4 texColorZ = texture(texture1, final_uvZ);

    // Normalize the normal vector to get direction weights for blending
    float sum = absNormal.x + absNormal.y + absNormal.z;
    absNormal /= sum;

    // Blend the three projections based on the normal weights
    return texColorX * absNormal.x + texColorY * absNormal.y + texColorZ * absNormal.z;
}


// Function to calculate light intensity
float calculateLightIntensity(vec3 normal, vec3 worldPos) {
    return 1.0f;
    // Calculate the light direction and distance
    //vec3 lightDir = normalize(lightPos - worldPos);
    //float distance = length(lightPos - worldPos);

    // Calculate light intensity with distance falloff (inverse square law)
    //float lightIntensity = dot(normal, lightDir) * lightMultiplier;

    //return lightIntensity;
}

void main() {
    // Normalize the normal vector
    vec3 norm = normalize(Normal);

    // Calculate light intensity
    float lightIntensity = calculateLightIntensity(norm, WorldPos);

    // Define base and shaded colors
    vec3 baseColor = vec3(0.5);
    vec3 shadedColor = baseColor + lightIntensity;

    // Lerp between baseColor and shadedColor
    vec3 finalColor = mix(baseColor, shadedColor, lightIntensity);

    // Sample the texture using triplanar mapping
    vec4 textureColor = sampleTriplanar(WorldPos, norm);

    // Final color output, combining texture color with the lerped final color
    FragColor = textureColor * vec4(finalColor, 1.0);
}
