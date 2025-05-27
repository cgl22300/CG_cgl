#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float kernel_offset;

float offset = 1.0 / kernel_offset;

vec4 Inversion(vec4 color);
vec4 Grayscale(vec4 color);
vec4 KernelFluter();


void main()
{
    //    FragColor = texture(screenTexture, TexCoords);
    //    //    FragColor = Inversion(FragColor);
    //    FragColor = Grayscale(FragColor);


    FragColor = KernelFluter();
}

vec4 Inversion(vec4 color) {
    return vec4(1.f - color);
}
vec4 Grayscale(vec4 color) {
    return vec4(vec3(0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b), 1.0);
}
vec4 KernelFluter() {
    float offset = 1.0 / kernel_offset;

    vec2 offsets[9] = vec2[](
    vec2(-offset, offset), // 左上
    vec2(0.0f, offset), // 正上
    vec2(offset, offset), // 右上
    vec2(-offset, 0.0f), // 左
    vec2(0.0f, 0.0f), // 中
    vec2(offset, 0.0f), // 右
    vec2(-offset, -offset), // 左下
    vec2(0.0f, -offset), // 正下
    vec2(offset, -offset)  // 右下
    );

    //锐化
    float kernel[9] = float[](
    -1, -1, -1,
    -1, 9, -1,
    -1, -1, -1
    );
    //模糊
//    float kernel[9] = float[](
//    1.0 / 16, 2.0 / 16, 1.0 / 16,
//    2.0 / 16, 4.0 / 16, 2.0 / 16,
//    1.0 / 16, 2.0 / 16, 1.0 / 16
//    );

    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; i++)
    col += sampleTex[i] * kernel[i];
    return vec4(col, 1.0);
}