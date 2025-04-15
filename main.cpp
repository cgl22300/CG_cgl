#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <stb_image.h>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Shader.h"


#include "Actor.h"

using namespace glm;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

void CalculateColor(const float &t, float &red, float &green, float &blue);

unsigned int Load_Tex(const char *filename);

unsigned int Load_Tex4f(const char *filename);

// settings
int WIDTH = 800;
int HEIGHT = 600;
// stores how much we're
// seeing of either texture
float angle_X = 0;
float angle_Y = 0;
float angle_Z = 0;
float offset_u = 0, offset_v = 0;
float scaleValue = 1;
float mixValue = 0.2f;
bool flag = true;
//std::function<mat4(float, float, float, float)> GetProjectionMatrix = GetPerspectiveProjectionMatrix;

Camera camera = Camera(vec3(0.f, 0.f, 3.f), vec3(0.f, 1.0f, 0.f), vec3(0.f, 0.f, -1.f));

vec3 position = {0, 0, 0};

const char *VertexShader_Path = "../Shaders/VertexShader.glsl";
const char *PointLightFrag_Path = "../Shaders/FragmentShader.frag";
const char *DirectionLightFrag_Path = "../Shaders/DirectionLight.frag";
const char *FragmentSharder_Path2 = "../Shaders/FragmentShader2.frag";
const char *FragmentSharder_Path3 = "../Shaders/LightFragment.frag";
//const char *VertexShader_Path = "Shaders/VertexShader.glsl";
//const char *PointLightFrag_Path = "Shaders/FragmentShader.frag";
//const char *FragmentSharder_Path2 = "Shaders/FragmentShader2.frag";

//
const char *container_path = "../tex/container.jpg";
const char *container2_path = "../tex/container2.png";
const char *container2_specular_path = "../tex/container2_specular.png";
const char *Tex1_Path = "../tex/tex1.jpg";
const char *Tex2_Path = "../tex/tex2.jpg";
const char *EmissionMap_Path = "../tex/EmissionMap.jpg";
const char *SmileFace_Path = "../tex/awesomeface.png";

//const char *SmileFace_Path="tex/awesomeface.png";
//const char *container_path = "tex/container.jpg";
//const char *Tex1_Path = "tex/tex1.jpg";
//const char *Tex2_Path = "tex/tex2.jpg";


int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "ComputerGraphics_E1", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        //终止glfw,否则glfw在初次初始化后将不再接收任何初始化参数
        glfwTerminate();
        return -1;
    }
    //window被设置为当前的上下文主线程对象
    glfwMakeContextCurrent(window);

    //当窗口大小被改变后将会调用该回调函数,调整为新的的glViewpoint
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 隐藏鼠标并让其自由移动
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
        static double CursorX = xpos, CursorY = ypos;
        camera.TurnX(xpos - CursorX);
        camera.TurnY(ypos - CursorY);
        CursorX = xpos;
        CursorY = ypos;
    });


    //开启垂直同步
    glfwSwapInterval(1);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    int nrAttributes;


    //准备微程序
    Shader shader_PointLight = Shader(VertexShader_Path, PointLightFrag_Path);
    Shader shader_DirectLight = Shader(VertexShader_Path, DirectionLightFrag_Path);
//    Shader shader2 = Shader(VertexShader_Path, FragmentSharder_Path2);
    Shader Lampshader = Shader(VertexShader_Path, FragmentSharder_Path3);
    //着色器准备完毕
    Shader &CurrentSharder = shader_PointLight;

    //渲染目标信息准备
    //------------
    //纹理生成
    stbi_set_flip_vertically_on_load(true);
    unsigned int Tex_box2 = Load_Tex4f(container2_path);
    unsigned int SpecularTex_box2 = Load_Tex4f(container2_specular_path);
    unsigned int Tex_EmissionMap = Load_Tex(EmissionMap_Path);
    unsigned int Tex_SmileFace = Load_Tex4f(SmileFace_Path);
    unsigned int texture3 = Load_Tex(Tex1_Path);
    unsigned int Tex_Cat = Load_Tex(Tex2_Path);

    float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
    unsigned int indices[36];
    for (unsigned int i = 0; i < 36; i++) {
        indices[i] = i;
    }

    Actor box1;


    Actor Lamp;

    unsigned int VBO0, EBO0;

    glGenBuffers(1, &VBO0);
    glGenBuffers(1, &EBO0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO0);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    float posiotions[] = {0.f, 0.f, 0.f,
                          -1.f, -1.f, -1.f,
                          -1.f, 1.f, -1.f,
                          1.f, -1.f, -1.f,
                          1.f, 1.f, -1.f
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-180.f, 180.f);

    Actor boxes[5];
    for (int i = 0; i < 5; i++) {
        boxes[i].Mesh->LoadMesh(VBO0, EBO0, sizeof(indices) / sizeof(unsigned int));
        boxes[i].Mesh->SetVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
        boxes[i].Mesh->SetVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                                              (void *) (3 * sizeof(float)));
        boxes[i].Mesh->SetVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                                              (void *) (6 * sizeof(float)));
        boxes[i].SetWorldLocation(posiotions[i * 3], posiotions[i * 3 + 1], posiotions[i * 3 + 2]);
//        boxes[i].SetWorldRotation(dis(gen), dis(gen), dis(gen));

    }
    Lamp.Mesh->LoadMesh(VBO0, EBO0, sizeof(indices) / sizeof(unsigned int));
    Lamp.Mesh->SetVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    Lamp.SetWorldScale(0.1f, 0.1f, 0.1f);
    position = vec3(1.f, 1.f, 1.f);
    Lamp.SetWorldLocation(1.f, 1.f, 1.f);

    float floor_vertices[] = {
            //     ---- 位置 ----
            2.f, -2.f, -2.f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // 右上
            2.f, -2.f, 2.f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // 右下
            -2.f, -2.f, 2.f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // 左下
            -2.f, -2.f, -2.f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f // 左上
    };
    unsigned int floor_ind[] = {
            0, 1, 3, // first triangle
            1, 2, 3 // second triangle
    };

    Actor floor;
    floor.Mesh->LoadMesh(floor_vertices, sizeof(floor_vertices), floor_ind, sizeof(floor_ind));
    floor.Mesh->SetVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    floor.Mesh->SetVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    floor.Mesh->SetVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));


    CurrentSharder.use();
    auto LightPos = Lamp.GetWorldLocation();
    CurrentSharder.setVec3("light.position", LightPos.x, LightPos.y, LightPos.z);
    CurrentSharder.setVec3("light.direction", -LightPos.x, -LightPos.y, -LightPos.z);

    CurrentSharder.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    CurrentSharder.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    CurrentSharder.setVec3("light.specular", 1.0f, 1.0f, 1.0f);


    CurrentSharder.setFloat("material.shininess", 32.0f);
    CurrentSharder.setInt("material.diffuse", 0);
    CurrentSharder.setInt("material.specular", 1);
    CurrentSharder.setInt("material.emission", 2);

    mat4 ViewMatrix = glm::mat4(1.0f);
    mat4 ProjectionMatrix = glm::mat4(1.0f);
    mat4 modelMatrix4f = glm::mat4(1.0f);
    mat4 transMatrix = glm::mat4(1.0f);

    //开启深度测试
    glEnable(GL_DEPTH_TEST);
//    glDisable(GL_CULL_FACE);
    while (!glfwWindowShouldClose(window)) {

        // input
        // -----
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        ViewMatrix = camera.GetViewMatrix();
        ProjectionMatrix = glm::perspective(glm::radians(camera.Zoom), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);

        // render container
        CurrentSharder.use();
        LightPos = Lamp.GetWorldLocation();
        CurrentSharder.setVec3("light.position", LightPos);
        CurrentSharder.setVec3("light.direction", -LightPos);
        CurrentSharder.setFloat("mixValue", mixValue);
        CurrentSharder.setFloat("offset_u", offset_u);
        CurrentSharder.setFloat("offset_v", offset_v);
        CurrentSharder.setFloat("scaleValue", scaleValue);

        auto ViewPoint = camera.GetViewPoint();
        CurrentSharder.setVec3("viewPos", camera.Position);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Tex_box2);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, SpecularTex_box2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, Tex_EmissionMap);


        //计算变换矩阵
        for (auto &boxe: boxes) {
            boxe.SetWorldRotation(angle_X, angle_Y, angle_Z);
            modelMatrix4f = boxe.GetModelMatrix4f();
            transMatrix = ProjectionMatrix * ViewMatrix * modelMatrix4f;
            CurrentSharder.setMat4("transMatrix", transMatrix);
            CurrentSharder.setMat4("model", modelMatrix4f);
            boxe.Mesh->DrawElement();
        }


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Tex_Cat);

        floor.SetWorldScale(5.f, 5.f, 5.f);
//        floor.SetWorldRotation(90.f, 0.f, 0.f);
        modelMatrix4f = floor.GetModelMatrix4f();
        transMatrix = ProjectionMatrix * ViewMatrix * modelMatrix4f;
        CurrentSharder.setMat4("transMatrix", transMatrix);
        CurrentSharder.setMat4("model", modelMatrix4f);
        floor.Mesh->DrawElement();


        Lampshader.use();

        Lamp.SetWorldLocation(position.x, position.y, position.z);
        modelMatrix4f = Lamp.GetModelMatrix4f();
        transMatrix = ProjectionMatrix * ViewMatrix * modelMatrix4f;
        Lampshader.setMat4("transMatrix", transMatrix);
        Lamp.Mesh->DrawElement();

        //划线模式和填充模式
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        //交换双缓冲内容,交换缓冲区并轮询输入输出（IO）事件（如按键按下 / 释放、鼠标移动等）。
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteBuffers(1, &VBO0);
    glDeleteTextures(1, &Tex_box2);
    glDeleteTextures(1, &Tex_SmileFace);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {

    static bool HasPress = false;
//    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !HasPress) {
//        if (flag) {
//            flag = false;
//            GetProjectionMatrix = GetPerspectiveProjectionMatrix;
//        } else {
//            flag = true;
//            GetProjectionMatrix = GetOrthographicProjectionMatrix;
//        }
//        HasPress = true;
//    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
        HasPress = false;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        mixValue += 0.01f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (mixValue >= 1.0f)
            mixValue = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS) {
        mixValue -= 0.01f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (mixValue <= 0.0f)
            mixValue = 0.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        offset_v += 0.01f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (offset_v >= 1.0f)
            offset_v = offset_v - 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        offset_v -= 0.01f;
        if (offset_v <= 0.0f)
            offset_v = 1.0f - offset_v;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        offset_u += 0.01f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (offset_u >= 1.0f)
            offset_u = offset_u - 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        offset_u -= 0.01f;
        if (offset_u <= 0.0f)
            offset_u = 1.0f - offset_u;
    }

    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
        scaleValue += 0.01f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (scaleValue >= 1.0f)
            scaleValue = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
        scaleValue -= 0.01f;
        if (scaleValue <= 0.0f)
            scaleValue = 0.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        angle_X -= 0.2f;
        if (angle_X <= 0.0f) {
            angle_X = 360 + angle_Y;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        angle_Y -= 0.2f;
        if (angle_Y <= 0.0f) {
            angle_Y = 360 + angle_Y;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        angle_Z -= 0.2f;
        if (angle_Z <= 0.0f) {
            angle_Z = 360 + angle_Y;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS) {
        position.z -= 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS) {
        position.z += 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS) {
        position.x -= 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS) {
        position.x += 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS) {
        position.y -= 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS) {
        position.y += 0.01;
    }


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { ;
        camera.MoveForward(0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.MoveForward(-0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.MoveRight(-0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.MoveRight(0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera.MoveRise(+0.1f);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.MoveRise(-0.1f);
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.

    WIDTH = width;
    HEIGHT = height;
    glViewport(0, 0, width, height);
}

void CalculateColor(const float &t, float &red, float &green, float &blue) {
    // 使用相位差为120度的正弦波（2π/3弧度）
    red = 0.5f + 0.5f * sin(t);
    green = 0.5f + 0.5f * sin(t + 2.0f * M_PI / 3.0f);
    blue = 0.5f + 0.5f * sin(t + 4.0f * M_PI / 3.0f);
}

unsigned int Load_Tex(const char *filename) {
    //纹理生成
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //tex读取
    int width, height, nrChannels;
    //分别获取图片高度宽度以及颜色通道数
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        /**
     *texture内存解释:
     *target:意味着此函数影响当前绑定的2D纹理对象,而1D和3D不受影响;
     *level:生成mipmap级别
     *internalformat:gl存储纹理类型
     *width,height:不解释
     *border:gl的屎山遗留物，不用管
     *format:原图像格式
     *type:数据类型,此处为char(byte)数组
     *图像数据
     */
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        //生成mipmap(如果不需要则可省略);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
        return texture;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    std::cout << "Failed to load texture" << std::endl;
    return 0;
}

unsigned int Load_Tex4f(const char *filename) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    //tex读取
    int width, height, nrChannels;
    //分别获取图片高度宽度以及颜色通道数
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        return texture;
    }

    std::cout << "Failed to load texture" << std::endl;
    return 0;
}


