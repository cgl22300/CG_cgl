#version 330 core
in vec3 vertexColor;
in vec2 TexUV;
out vec4 FragColor;

uniform float mixValue;
uniform sampler2D Texture;
uniform sampler2D Texture2;


void main() {
    //     FragColor = texture(Texture, TexUV)*vec4(vertexColor,1.0);


    FragColor = texture(Texture, mod(TexUV, 1.0) );
    //    FragColor = mix(texture(Texture, TexUV), texture(Texture2, TexUV), mixValue);

}