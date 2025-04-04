//
// Created by 分子轨道mo法 on 25-4-4.
//

#include "Light.h"

void Light::Init() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);



    glBindVertexArray(0);
}
