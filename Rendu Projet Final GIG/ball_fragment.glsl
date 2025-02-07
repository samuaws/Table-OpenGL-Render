#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 ViewDir;

uniform vec3 objectColor = vec3(1.0, 0.2, 0.2); // Base color
uniform vec3 fresnelColor = vec3(1.0, 1.0, 1.0); // White rim color
uniform float fresnelStrength = 1.5; // Adjust rim intensity
uniform float fresnelPower = 3.0; // Adjust rim sharpness

void main()
{
    float fresnel = fresnelStrength * pow(1.0 - max(dot(normalize(Normal), ViewDir), 0.0), fresnelPower);
    vec3 finalColor = mix(objectColor, fresnelColor, fresnel);
    FragColor = vec4(finalColor, 1.0);
}
