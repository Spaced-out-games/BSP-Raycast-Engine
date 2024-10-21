#pragma once
#include "glm_master.h"
#include "uBuffer.h" // Assuming uBuffer is a class for managing uniform buffer objects


class Camera {
public:
    Camera(vec3 pos = vec3(0.0f, 0.0f, 0.0f),
        vec3 target = vec3(0.0f, 0.0f, -1.0f),
        vec3 upDirection = vec3(0.0f, 1.0f, 0.0f),
        float fieldOfView = 70.0f,
        float aspectRatio = 16.0f / 9.0f,
        float nearClip = 0.1f,
        float farClip = 1000.0f)
        : position(pos), target(target), up(upDirection), fov(fieldOfView),
        aspectRatio(aspectRatio), near(nearClip), far(farClip),
        ubo() // Initialize uBuffer with 128 bytes and default usage
    {
        updateViewMatrix();
    }
    vec3 get_position_in_front(float length)
    {
        return position + (getForwardVector() * length);
    }
    mat4 getViewMatrix() const {
        return viewMatrix;
    }

    mat4 getPerspectiveMatrix() const {
        return glm::perspective(glm::radians(fov), aspectRatio, near, far);
    }

    void updateUBO() {
        mat4 viewMatrix = getViewMatrix();
        mat4 projectionMatrix = getPerspectiveMatrix();
        ubo.updateData(0, sizeof(mat4), glm::value_ptr(viewMatrix));
        ubo.updateData(sizeof(mat4), sizeof(mat4), glm::value_ptr(projectionMatrix));
        
    }

    void bindUBO(GLuint program) const {
        GLuint uboIndex = glGetUniformBlockIndex(program, "CameraUBO");
        glUniformBlockBinding(program, uboIndex, 0);
        ubo.bindBase(0);
    }

    void setPosition(vec3 newPosition) {
        position = newPosition;
        updateViewMatrix();
    }

    vec3 getPosition() const { return position; }

    void setTarget(vec3 newTarget) {
        target = newTarget;
        updateViewMatrix();
    }

    vec3 getTarget() const { return target; }

    void setUpDirection(vec3 newUpDirection) {
        up = newUpDirection;
        updateViewMatrix();
    }

    vec3 getUpDirection() const { return up; }

    vec3 getForwardVector() const {
        return glm::normalize(target - position);
    }

    vec3 getRightVector() const {
        vec3 forward = getForwardVector();
        return glm::normalize(glm::cross(up, forward));
    }

    void lookAt(vec3 target) {
        setTarget(target);
    }

    void rotate(float pitch, float yaw) {
        vec3 forward = glm::normalize(target - position);

        // Compute the right vector
        vec3 right = glm::normalize(glm::cross(up, forward));

        // Create rotation matrices for pitch and yaw
        mat4 pitchRotation = glm::rotate(mat4(1.0f), glm::radians(pitch), right);
        mat4 yawRotation = glm::rotate(mat4(1.0f), glm::radians(yaw), up);

        // Apply pitch and yaw rotations to the forward vector
        vec4 newForward = yawRotation * pitchRotation * vec4(forward, 0.0f);
        target = position + glm::normalize(vec3(newForward));

        updateViewMatrix();
    }

    void moveForward(float distance) {
        vec3 forward = getForwardVector();
        position += forward * distance;
        target += forward * distance;
        updateViewMatrix();
    }

    void moveRight(float distance) {
        vec3 right = getRightVector();
        position += right * distance;
        target += right * distance;
        updateViewMatrix();
    }

    void moveUp(float distance) {
        position += up * distance;
        target += up * distance;
        updateViewMatrix();
    }

    uBuffer<>& getUBO() { return ubo; }

private:
    vec3 position;   // Camera position in world space
    vec3 target;     // Point the camera is looking at
    vec3 up;         // Up vector (typically the world up direction)
    float fov;            // Field of view in degrees
    float aspectRatio;    // Aspect ratio of the camera (width / height)
    float near;           // Near clipping plane distance
    float far;            // Far clipping plane distance
    mat4 viewMatrix; // View matrix
    uBuffer<> ubo;        // Uniform buffer object for storing view and projection matrices

    void updateViewMatrix() {
        viewMatrix = glm::lookAt(position, target, up);
    }
};
