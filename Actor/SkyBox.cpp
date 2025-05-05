//
// Created by 分子轨道mo法 on 25-4-29.
//

#include "SkyBox.h"
#include "geometry/box.h"
#include "ImageLoader.h"

SkyBox::SkyBox() {




    glGenVertexArrays(1, &VAO);



    BindBoxMesh(VAO);

    SetWorldScale(20.f, 20.f, 20.f);

}

void SkyBox::Draw(mat4 transMat4, mat4 modelMat4,Shader& shader) {


    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    int loc = glGetUniformLocation(shader.ID, "sky_tex.front");
    if (loc == -1) {
        std::cerr << "Uniform 'sky_tex.front' not found in shader!" << std::endl;
    }
    loc = glGetUniformLocation(shader.ID, "sky_tex.back");
    if (loc == -1) {
        std::cerr << "Uniform 'sky_tex.back' not found in shader!" << std::endl;
    }

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL Error in SkyBox draw: " << err << std::endl;
    }

}



