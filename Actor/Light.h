//
// Created by 分子轨道mo法 on 25-4-4.
//

#ifndef CG_E1_1_LIGHT_H
#define CG_E1_1_LIGHT_H

#include "glad/glad.h"
#include "Actor.h"






class Light : public Actor {


    unsigned int VAO;
public:

    Light();

    void Draw(mat4 transMat4,Shader& shader);

};


#endif //CG_E1_1_LIGHT_H
