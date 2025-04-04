#version 330 core
in vec3 vertexColor;
in vec2 TexUV;

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float mixValue;
uniform sampler2D Texture;
uniform sampler2D Texture2;


void main() {
    vec4 ObjectColor = vec4(1.0f, 0.5f, 0.31f, 1.0f);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    FragColor = vec4((ambient + diffuse + specular), 1.0) * texture(Texture, mod(TexUV, 1.0));
//    FragColor = vec4((ambient + diffuse + specular), 1.0) * ObjectColor;
}