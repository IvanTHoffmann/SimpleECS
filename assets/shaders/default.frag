#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;
uniform sampler2D specularTex;
uniform vec2 tiling;

in mat3 tanToModel;

in vec3 viewPos;
in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 pixelColor;

const float v = .2f;

layout(std140) uniform Material {
    vec3 ambient;
    vec3 albedo;
    vec3 specular;
    float a;
} mat;

vec3 phong(in vec3 N, in vec3 L, in vec3 V, in vec3 ambient, in vec3 diffuse, in vec3 specular, in float a) {
    vec3 R = reflect(-L, N);
    return diffuse + max(dot(N, L)*0+1, 0.0) * ambient + pow(max(dot(R, V), 0.0), a) * specular;
}

void main() {
    vec2 texCoord = fragTexCoord * tiling;
    vec3 normal = tanToModel * normalize(texture(normalTex, texCoord).xyz * vec3(1, 1, 1) * 2 - 1);

    vec3 N = normalize(normal); // todo: transform normalmap value into world space
    vec3 lightPos = vec3(0, 100, 100);
    vec3 L = normalize(lightPos - fragPos);
    vec3 V = normalize(viewPos - fragPos);


    vec3 diffuse = texture(diffuseTex, texCoord).rgb;
    vec3 specular = vec3(1, .5, .5) * texture(specularTex, texCoord).rgb;
    vec3 ambient = vec3(1, 1, .7) * .05;

    vec3 color = phong(N, L, V, ambient, diffuse, specular, 0.2);

    pixelColor = vec4(color, 1.0);
    //gl_FragColor = vec4(texture(albedoTexture, fragTexCoord).rgb, 1.0);
    //gl_FragColor = vec4(N * 0.5 + 0.5, 1.0);
}