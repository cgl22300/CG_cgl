#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexUV;



uniform mat4 transMatrix;

uniform float scaleValue;
uniform float offset_u;
uniform float offset_v;
//插值相位
out vec3 ThisPosition;
out vec3 vertexColor;
out vec2 TexUV;


void main()
{
    // 修正为 1.0
    gl_Position = transMatrix*vec4(aPos , 1.0);
    ThisPosition = aPos;
    vertexColor = aColor;

    TexUV = vec2(offset_u,offset_v) + scaleValue * aTexUV;
}