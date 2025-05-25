//
// Created by 分子轨道mo法 on 25-5-25.
//

#include "FullScreenTexturePlane.h"
#include "glad/glad.h"
#include "Shader.h"

void FullScreenTexturePlane::InitVAO() {
    glGenVertexArrays(1, &VAO);


    float vertices[] = {
            // 底面                       //UV
            -1.f, 1.0f, 0.0f, 1.0f,
            -1.f, -1.f, 0.0f, 0.0f,
            1.f, -1.f, 1.0f, 0.0f,
            1.f, 1.0f, 1.0f, 1.0f

    };
    unsigned int indices[6] = {
            0, 1, 3,
            1, 2, 3
    };
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


}

FullScreenTexturePlane::FullScreenTexturePlane() : VAO(0), VBO(0), EBO(0) {
    InitVAO();
}

void FullScreenTexturePlane::Draw(const Shader &shader) {
    shader.use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
