#version 450

uniform sampler2D tex;
uniform vec2 tiling;

in vec3 viewPos;
in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 pixelColor;

const float v = .2f;

void main() {
    vec3 texColor = texture(tex, fragTexCoord * tiling).xyz;
    vec3 lightColor = vec3(1, .9, 1);
    vec3 lightDir = normalize(vec3(-.8, -1.2, -1));
    
    float ambientStrength = 0.3;
    float specularStrength = 1.0;
    
    vec3 ambient = ambientStrength * lightColor;

    float diff = max(dot(fragNormal, -lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(lightDir, fragNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * texColor;
    pixelColor = vec4(result, 1.0);
}