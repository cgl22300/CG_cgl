#version 330

in vec3 Normal;
in vec2 TexUV;

struct SkyTex
{
    sampler2D front;
    sampler2D back;
    sampler2D bottom;
    sampler2D top;
    sampler2D left;
    sampler2D right;
};

out vec4 FragColor;

uniform SkyTex sky_tex;

void main() {
    //    vec4 color = vec4(.5); // 默认颜色为白色

    //    // 根据法线方向选择不同的纹理进行采样
    //    if (Normal.z > 0.9) {
    //        color = texture (sky_tex.front, TexUV); // 使用前面的纹理
    //    } else if (Normal.z < -0.9) {
    //        color = texture(sky_tex.back, TexUV); // 使用后面的纹理
    //    } else if (Normal.y > 0.9) {
    //        color = texture(sky_tex.top, TexUV); // 使用顶部纹理
    //    } else if (Normal.y < -0.9) {
    //        color = texture(sky_tex.bottom, TexUV); // 使用底部纹理
    //    } else if (Normal.x > 0.9) {
    //        color = texture(sky_tex.left, TexUV); // 使用左侧纹理
    //    } else if (Normal.x < -0.9) {
    //        color = texture(sky_tex.right, TexUV); // 使用右侧纹理
    //    }

    FragColor = texture(sky_tex.front, TexUV);  // 将采样的颜色输出到片元
}

