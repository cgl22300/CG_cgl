#version 330

struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D texture_normal;
    sampler2D texture_height;
    float shininess;
};


uniform Material material;
out vec4 FragColor;
in vec2 TexCoords;

void main() {
    FragColor = texture(material.texture_diffuse, TexCoords);
    if(FragColor.a < 0.1)
        discard;
}