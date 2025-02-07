#version 460 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform sampler2D texture1;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);

    // Ambient lighting
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;

    // Sample the texture
    vec3 textureColor = texture(texture1, TexCoord).rgb;

    // Final color
    vec3 result = (ambient + diffuse) * textureColor;
    FragColor = vec4(result, 1.0);
}
