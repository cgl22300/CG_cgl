//
// Created by 分子轨道mo法 on 25-5-25.
//

#ifndef CG_E2_FULLSCREENTEXTUREPLANE_H
#define CG_E2_FULLSCREENTEXTUREPLANE_H

#include "Shader.h"

/**
 * 纹理缓冲对象目标对象，包含一个占据屏幕空间的平面
 */
class FullScreenTexturePlane {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
public:
    FullScreenTexturePlane();

    void InitVAO();

    void Draw(const Shader &shader);

};


#endif //CG_E2_FULLSCREENTEXTUREPLANE_H
