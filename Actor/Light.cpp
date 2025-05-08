//
// Created by 分子轨道mo法 on 25-4-4.
//

#include "Light.h"
#include "geometry/box.h"


Light::Light() {

    glGenVertexArrays(1, &VAO);
    BindBoxMesh(VAO,BOX_FACE::OUTWARD);
}


void Light::Draw(mat4 transMat4, Shader& shader) {
//    if(shader.isValid()) {
        shader.use();
        shader.setMat4("transMatrix", transMat4);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

}




