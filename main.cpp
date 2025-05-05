#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <stb_image.h>
#include <random>
#include <glm/glm.hpp>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Shader.h"


#include "Actor/Actor.h"
#include "Actor/Light.h"
#include "ImageLoader.h"
#include "Actor/SkyBox.h"

using namespace glm;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

void CalculateColor(const float &t, float &red, float &green, float &blue);


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
bool FlashSwitch = false;
std::unordered_map<int, bool> keyPressTable;
//std::function<mat4(float, float, float, float)> GetProjectionMatrix = GetPerspectiveProjectionMatrix;

Camera camera = Camera(vec3(0.f, 0.f, 3.f), vec3(0.f, 1.0f, 0.f), vec3(0.f, 0.f, -1.f));

vec3 position = {0, 0, 0};

const char *VertexShader_Path = "../Shaders/VertexShader.glsl";

//片段着色器
const char *PointLightFrag_Path = "../Shaders/PointLight.frag";
const char *DirectionLightFrag_Path = "../Shaders/DirectionLight.frag";
const char *FlashLightFrag_Path = "../Shaders/FlashLight.frag";
const char *MultiLightFrag_Path = "../Shaders/MultiLight.frag";

const char *LampFrag_Path = "../Shaders/LightFragment.frag";
//const char *VertexShader_Path = "Shaders/VertexShader.glsl";
//const char *PointLightFrag_Path = "Shaders/PointLight.frag";
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



enum class ShaderMode {
    PointLight,
    DirectLight,
    FlashLight,
    MultiLight
};

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

    unsigned int front;
    unsigned int back;
    unsigned int bottom;
    unsigned int top;
    unsigned int left;
    unsigned int right;

    front = Load_Tex("../model/skybox/front.jpg");
    back = Load_Tex("../model/skybox/back.jpg");;
    bottom = Load_Tex("../model/skybox/bottom.jpg");;
    top = Load_Tex("../model/skybox/top.jpg");;
    left = Load_Tex("../model/skybox/left.jpg");;
    right = Load_Tex("../model/skybox/right.jpg");;

    //准备微程序
    Shader shader_PointLight = Shader(VertexShader_Path, PointLightFrag_Path);
    Shader shader_DirectLight = Shader(VertexShader_Path, DirectionLightFrag_Path);
    Shader shader_FlashLight = Shader(VertexShader_Path, FlashLightFrag_Path);
    Shader shader_MultiLight = Shader(VertexShader_Path, MultiLightFrag_Path);

    Shader Lampshader = Shader(VertexShader_Path, LampFrag_Path);

    Shader SkyBoxShader = Shader(VertexShader_Path, "../Shaders/SkyBoxFragment.frag");
    SkyBoxShader.use();
    SkyBoxShader.setInt("sky_tex.front", 0);
    SkyBoxShader.setInt("sky_tex.back", 1);
    SkyBoxShader.setInt("sky_tex.bottom", 2);
    SkyBoxShader.setInt("sky_tex.top", 3);
    SkyBoxShader.setInt("sky_tex.left", 4);
    SkyBoxShader.setInt("sky_tex.right", 5);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL SkyBoxShader Init Error: " << err << std::endl;
    }

    //渲染目标信息准备
    //------------
    //纹理生成
//    stbi_set_flip_vertically_on_load(true);

    unsigned int Tex_box2 = Load_Tex4f(container2_path);
    unsigned int SpecularTex_box2 = Load_Tex4f(container2_specular_path);


    shared_ptr<Model> model = std::make_shared<Model>(
            Model("I:/OpenGL/workspace/CG-E1-1/model/moulder_hand_sickle/mhs.obj"));

    Actor BackPack(model);
    BackPack.SetWorldLocation(1.f, 1.f, 1.f);


    Light Lamp[4];

    Lamp[0].SetWorldScale(0.1f, 0.1f, 0.1f);
    Lamp[0].SetWorldLocation(2.0f, 2.0f, -2.0f);

    Lamp[1].SetWorldScale(0.1f, 0.1f, 0.1f);
    Lamp[1].SetWorldLocation(-2.0f, 2.0f, -2.0f);

    Lamp[2].SetWorldScale(0.1f, 0.1f, 0.1f);
    Lamp[2].SetWorldLocation(2.0f, -2.0f, -2.0f);

    Lamp[3].SetWorldScale(0.1f, 0.1f, 0.1f);
    Lamp[3].SetWorldLocation(-2.0f, -2.0f, -2.0f);

    SkyBox skyBox;


    //着色器准备完毕
    Shader &CurrentSharder = shader_MultiLight;
    ShaderMode shaderMode = ShaderMode::MultiLight;

    switch (shaderMode) {
        case ShaderMode::PointLight:
            CurrentSharder = shader_PointLight;
            CurrentSharder.use();
            CurrentSharder.setVec3("light.position", Lamp[0].GetWorldLocation());
            CurrentSharder.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
            CurrentSharder.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
            CurrentSharder.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
            break;
        case ShaderMode::DirectLight:
            CurrentSharder = shader_DirectLight;
            CurrentSharder.use();
            CurrentSharder.setVec3("light.direction", Lamp[0].GetWorldLocation());
            CurrentSharder.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
            CurrentSharder.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
            CurrentSharder.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
            break;
        case ShaderMode::FlashLight:
            CurrentSharder = shader_FlashLight;
            CurrentSharder.use();
            CurrentSharder.setVec3("light.position", camera.Position);
            CurrentSharder.setVec3("light.direction", camera.Lookat);
            CurrentSharder.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
            CurrentSharder.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
            CurrentSharder.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
            CurrentSharder.setVec3("light.diffuse", 0.95f, 0.95f, 0.95f);
            CurrentSharder.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
            CurrentSharder.setFloat("light.constant", 1.0f);
            CurrentSharder.setFloat("light.linear", 0.09f);
            CurrentSharder.setFloat("light.quadratic", 0.032f);
            break;
        case ShaderMode::MultiLight:
            CurrentSharder = shader_MultiLight;
            CurrentSharder.use();
            CurrentSharder.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
            CurrentSharder.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
            CurrentSharder.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
            CurrentSharder.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

            CurrentSharder.setVec3("pointLights[0].position", Lamp[0].GetWorldLocation());
            CurrentSharder.setVec3("pointLights[0].ambient", 0.f, 0.f, 0.f);
            CurrentSharder.setVec3("pointLights[0].diffuse", 0.5f, 0.5f, 0.5f);
            CurrentSharder.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
            CurrentSharder.setFloat("pointLights[0].constant", 1.0f);
            CurrentSharder.setFloat("pointLights[0].linear", 0.09f);
            CurrentSharder.setFloat("pointLights[0].quadratic", 0.032f);
            // point light 2
            CurrentSharder.setVec3("pointLights[1].position", Lamp[1].GetWorldLocation());
            CurrentSharder.setVec3("pointLights[1].ambient", 0.f, 0.f, 0.f);
            CurrentSharder.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
            CurrentSharder.setVec3("pointLights[1].specular", 0.8f, 0.8f, 0.8f);
            CurrentSharder.setFloat("pointLights[1].constant", 1.0f);
            CurrentSharder.setFloat("pointLights[1].linear", 0.09f);
            CurrentSharder.setFloat("pointLights[1].quadratic", 0.032f);
            // point light 3
            CurrentSharder.setVec3("pointLights[2].position", Lamp[2].GetWorldLocation());
            CurrentSharder.setVec3("pointLights[2].ambient", 0.f, 0.f, 0.f);
            CurrentSharder.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
            CurrentSharder.setVec3("pointLights[2].specular", 0.8f, 0.8f, 0.8f);
            CurrentSharder.setFloat("pointLights[2].constant", 1.0f);
            CurrentSharder.setFloat("pointLights[2].linear", 0.09f);
            CurrentSharder.setFloat("pointLights[2].quadratic", 0.032f);
            // point light 4
            CurrentSharder.setVec3("pointLights[3].position", Lamp[3].GetWorldLocation());
            CurrentSharder.setVec3("pointLights[3].ambient", 0.f, 0.f, 0.f);
            CurrentSharder.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
            CurrentSharder.setVec3("pointLights[3].specular", 0.8f, 0.8f, 0.8f);
            CurrentSharder.setFloat("pointLights[3].constant", 1.0f);
            CurrentSharder.setFloat("pointLights[3].linear", 0.09f);
            CurrentSharder.setFloat("pointLights[3].quadratic", 0.032f);

            CurrentSharder.setVec3("flashLight.position", camera.Position);
            CurrentSharder.setVec3("flashLight.direction", camera.Lookat);
            CurrentSharder.setFloat("flashLight.cutOff", glm::cos(glm::radians(12.5f)));
            CurrentSharder.setFloat("flashLight.outerCutOff", glm::cos(glm::radians(17.5f)));
            CurrentSharder.setVec3("flashLight.ambient", 0.f, 0.f, 0.f);
            CurrentSharder.setVec3("flashLight.diffuse", 0.8f, 0.8f, 0.8f);
            CurrentSharder.setVec3("flashLight.specular", 1.0f, 1.0f, 1.0f);
            CurrentSharder.setFloat("flashLight.constant", 1.0f);
            CurrentSharder.setFloat("flashLight.linear", 0.09f);
            CurrentSharder.setFloat("flashLight.quadratic", 0.032f);

            break;
    }


    CurrentSharder.setFloat("material.shininess", 32.0f);
    CurrentSharder.setInt("material.diffuse", 0);
    CurrentSharder.setInt("material.specular", 1);
//    CurrentSharder.setInt("material.emission", 2);

    mat4 ViewMatrix = glm::mat4(1.0f);
    mat4 ProjectionMatrix = glm::mat4(1.0f);
    mat4 modelMatrix4f = glm::mat4(1.0f);
    mat4 transMatrix = glm::mat4(1.0f);

    if (!SkyBoxShader.isValid()) {
        std::cout << "有效1\n";
    }
    if (!Lampshader.isValid()) {
        std::cout << "有效2\n";
    }
    //开启深度测试
    glEnable(GL_DEPTH_TEST);
//    glDisable(GL_CULL_FACE);
    while (!glfwWindowShouldClose(window)) {

        // input
        // -----
        processInput(window);

        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        ViewMatrix = camera.GetViewMatrix();
        ProjectionMatrix = glm::perspective(glm::radians(camera.Zoom), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);

        // render container
        CurrentSharder.use();


        CurrentSharder.setVec3("flashLight.position", camera.Position);
        CurrentSharder.setVec3("flashLight.direction", camera.Lookat);

        CurrentSharder.setFloat("mixValue", mixValue);
        CurrentSharder.setFloat("offset_u", offset_u);
        CurrentSharder.setFloat("offset_v", offset_v);
        CurrentSharder.setFloat("scaleValue", scaleValue);

        auto ViewPoint = camera.GetViewPoint();
        CurrentSharder.setVec3("viewPos", camera.Position);


        CurrentSharder.setVec3("viewPos", camera.Position);
        CurrentSharder.setBool("FlashSwitch", FlashSwitch);


        BackPack.SetWorldLocation(position.x, position.y, position.z);
        modelMatrix4f = BackPack.GetModelMatrix4f();
        transMatrix = ProjectionMatrix * ViewMatrix * modelMatrix4f;
        BackPack.Draw(transMatrix, modelMatrix4f, CurrentSharder);


        if (!SkyBoxShader.isValid()) {
            GLenum err = glGetError();
            if (err != GL_NO_ERROR) {
                std::cerr << "OpenGL SkyBoxShader Error: " << err << std::endl;
            }

        }

        SkyBoxShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, front);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, back);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, bottom);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, top);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, left);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, right);

        modelMatrix4f = skyBox.GetModelMatrix4f();
        transMatrix = ProjectionMatrix * ViewMatrix * modelMatrix4f;
        SkyBoxShader.setMat4("transMatrix", transMatrix);
        SkyBoxShader.setMat4("model", modelMatrix4f);
        skyBox.Draw(transMatrix, modelMatrix4f, SkyBoxShader);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL  Lampshader Error: " << err << std::endl;
        }




        if (!Lampshader.isValid()) {
            GLenum err = glGetError();
            if (err != GL_NO_ERROR) {
                std::cerr << "OpenGL  Lampshader Error: " << err << std::endl;
            }

        }
        Lampshader.use();
        for (int i = 0; i < 4; i++) {

            modelMatrix4f = Lamp[i].GetModelMatrix4f();
            transMatrix = ProjectionMatrix * ViewMatrix * modelMatrix4f;

            Lamp[i].Draw(transMatrix, Lampshader);
        }



         err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL Error: " << err << std::endl;
        }
        //划线模式和填充模式
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        //交换双缓冲内容,交换缓冲区并轮询输入输出（IO）事件（如按键按下 / 释放、鼠标移动等）。
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteTextures(1, &Tex_box2);
//    glDeleteTextures(1, &Tex_SmileFace);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {

    static bool HasPress = false;
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

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {

        auto it = keyPressTable.find(GLFW_KEY_V);
        if (it == keyPressTable.end()) {
            keyPressTable.emplace(GLFW_KEY_V, true);
            FlashSwitch = !FlashSwitch;
        } else if (!it->second) {
            FlashSwitch = !FlashSwitch;
        }

    } else if (glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE) {
        auto it = keyPressTable.find(GLFW_KEY_V);
        if (it != keyPressTable.end())it->second = false;
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



