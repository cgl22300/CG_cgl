//
// Created by 分子轨道mo法 on 25-4-4.
//

#include "Actor.h"


// World transform setters
void Actor::SetWorldLocation(const float &x, const float &y, const float &z) {
    WorldLocation = vec3(x, y, z);
}

void Actor::SetWorldRotation(const float &Yaw, const float &Pitch, const float &Roll) {
    WorldRotation = vec3(Yaw, Pitch, Roll);
}

void Actor::SetWorldScale(const float &x, const float &y, const float &z) {
    WorldScale = vec3(x, y, z);
}

// Relative transform setters
void Actor::SetRelativeLocation(const float &x, const float &y, const float &z) {
    RelativeLocation = vec3(x, y, z);
}

void Actor::SetRelativeRotation(const float &Yaw, const float &Pitch, const float &Roll) {
    RelativeRotation = vec3(Yaw, Pitch, Roll);
}

void Actor::SetRelativeScale(const float &x, const float &y, const float &z) {
    RelativeScale = vec3(x, y, z);
}

void Actor::AttachTo(Actor *parent) {
    Parent = parent;
}

vec3 Actor::GetWorldLocation() {
    if (!Parent)return WorldLocation;

    auto ParentLocation = Parent->GetWorldLocation();

    return ParentLocation + RelativeLocation;
}

vec3 Actor::GetWorldRotation() {
    if (!Parent)return WorldRotation;


    auto ParentRotation = Parent->WorldRotation;


    return ParentRotation + RelativeRotation;
}

vec3 Actor::GetWorldScale() {
    if (!Parent)return WorldScale;

    auto ParentScale = Parent->WorldScale;
    vec3 Scale;
    Scale.x = RelativeScale.x * ParentScale.x;
    Scale.y = RelativeScale.y * ParentScale.y;
    Scale.z = RelativeScale.z * ParentScale.z;

    return Scale;
}

Actor::Actor() : Parent(nullptr) {
    WorldLocation = vec3(0.f, 0.f, 0.f);
    WorldRotation = vec3(0.f, 0.f, 0.f);
    WorldScale = vec3(1.f, 1.f, 1.f);
    //相对于父级的位置
    RelativeLocation = vec3(0.f, 0.f, 0.f);
    RelativeRotation = vec3(0.f, 0.f, 0.f);
    RelativeScale = vec3(1.f, 1.f, 1.f);
}

mat4 Actor::GetModelMatrix4f() {
    mat4 Identity = glm::mat4(1.0f);

    vec3 position = GetWorldLocation();  // 位置
    vec3 rotation = GetWorldRotation();  // 欧拉角，单位为度
    vec3 scale = GetWorldScale();        // 缩放

    mat4 TransMat = translate(Identity, position);
    mat4 RotationMat = rotate(Identity, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    RotationMat = rotate(RotationMat, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    RotationMat = rotate(RotationMat, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    mat4 ScaleMat = glm::scale(Identity, scale);

    return TransMat * RotationMat * ScaleMat;
}

void Actor::Draw(mat4 ViewMat4, mat4 ProjectionMat4, Shader &shader) {

    if (!ActorMesh) {
        std::cout << "模型未初始化\n";
        ActorInner::DrawWrongBox();
        return;
    }
    shader.use();

    auto modelMatrix4f = this->GetModelMatrix4f();
    auto transMatrix = ProjectionMat4 * ViewMat4 * modelMatrix4f;

    shader.setMat4("transMatrix", transMatrix);
    shader.setMat4("model", modelMatrix4f);

    ActorMesh->Draw(shader);

}

Actor::Actor(const std::shared_ptr<Model> &mesh) : Actor() {
    ActorMesh = mesh;
}

void Actor::SetMesh(const std::shared_ptr<Model> &mesh) {
    ActorMesh = mesh;
}

void Actor::SetWorldLocation(const vec3 &worldLocation) {
    WorldLocation = worldLocation;
}

void Actor::SetWorldRotation(const vec3 &worldRotation) {
    WorldRotation = worldRotation;
}

void Actor::SetWorldScale(const vec3 &worldScale) {
    WorldScale = worldScale;
}

void Actor::SetRelativeLocation(const vec3 &relativeLocation) {
    RelativeLocation = relativeLocation;
}

void Actor::SetRelativeRotation(const vec3 &relativeRotation) {
    RelativeRotation = relativeRotation;
}

void Actor::SetRelativeScale(const vec3 &relativeScale) {
    RelativeScale = relativeScale;
}

vec3 Actor::GetForwardDirection() {
    vec3 ForwardDirection = vec3(0.f, 1.f, 0.f); // OpenGL中默认Forward方向是 -Z
    vec3 rotation = GetWorldRotation();

    mat4 RotationMat = glm::mat4(1.0f);
    RotationMat = glm::rotate(RotationMat, glm::radians(rotation.x), glm::vec3(1, 0, 0)); // pitch
    RotationMat = glm::rotate(RotationMat, glm::radians(rotation.y), glm::vec3(0, 1, 0)); // yaw
    RotationMat = glm::rotate(RotationMat, glm::radians(rotation.z), glm::vec3(0, 0, 1)); // roll

    vec3 worldForward = vec3(RotationMat * vec4(ForwardDirection, 0.0f)); // 用0表示方向向量，不参与位移
    return glm::normalize(worldForward); // 可选：归一化
}

vec3 Actor::GetUpDirection() {
    vec3 ForwardDirection = vec3(0.f, 0.f, -1.f); // OpenGL中默认Forward方向是 -Z
    vec3 rotation = GetWorldRotation();

    mat4 RotationMat = glm::mat4(1.0f);
    RotationMat = glm::rotate(RotationMat, glm::radians(rotation.x), glm::vec3(1, 0, 0)); // pitch
    RotationMat = glm::rotate(RotationMat, glm::radians(rotation.y), glm::vec3(0, 1, 0)); // yaw
    RotationMat = glm::rotate(RotationMat, glm::radians(rotation.z), glm::vec3(0, 0, 1)); // roll

    vec3 worldForward = vec3(RotationMat * vec4(ForwardDirection, 0.0f)); // 用0表示方向向量，不参与位移
    return glm::normalize(worldForward); // 可选：归一化
}



