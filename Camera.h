#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp> // For PI
#include "uBuffer.h" // Assuming uBuffer is a class for managing uniform buffer objects
class Camera {
public:
    Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3 upDirection = glm::vec3(0.0f, 1.0f, 0.0f),
        float fieldOfView = 70.0f,
        float aspectRatio = 16.0f / 9.0f,
        float mNearClip = 0.1f,
        float mFarClip = 1000.0f)
        : position(pos), target(target), up(upDirection), fov(fieldOfView),
        aspectRatio(aspectRatio), mNear(mNearClip), mFar(mFarClip),
        ubo() // Initialize uBuffer with 128 bytes and default usage
    {
        updateViewMatrix();
    }
    glm::vec3 get_position_in_front(float length)
    {
        return position + (getForwardVector() * length);
    }
    glm::mat4 getViewMatrix() const {
        return viewMatrix;
    }
    glm::vec3 get_position() { return position; }


    glm::mat4 getPerspectiveMatrix() const {
        return glm::perspective(glm::radians(fov), aspectRatio, mNear, mFar);
    }

    void updateUBO() {
        glm::mat4 viewMatrix = getViewMatrix();
        glm::mat4 projectionMatrix = getPerspectiveMatrix();
        ubo.updateData(0, sizeof(glm::mat4), glm::value_ptr(viewMatrix));
        ubo.updateData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionMatrix));
        
    }

    void bindUBO(GLuint program) const {
        GLuint uboIndex = glGetUniformBlockIndex(program, "CameraUBO");
        glUniformBlockBinding(program, uboIndex, 0);
        ubo.bindBase(0);
    }

    void setPosition(glm::vec3 newPosition) {
        position = newPosition;
        updateViewMatrix();
    }

    glm::vec3 getPosition() const { return position; }

    void setTarget(glm::vec3 newTarget) {
        target = newTarget;
        updateViewMatrix();
    }

    glm::vec3 getTarget() const { return target; }

    void setUpDirection(glm::vec3 newUpDirection) {
        up = newUpDirection;
        updateViewMatrix();
    }

    glm::vec3 getUpDirection() const { return up; }

    glm::vec3 getForwardVector() const {
        return glm::normalize(target - position);
    }

    glm::vec3 getRightVector() const {
        glm::vec3 forward = getForwardVector();
        return glm::normalize(glm::cross(up, forward));
    }

    void lookAt(glm::vec3 target) {
        setTarget(target);
    }

    void rotate(float pitch, float yaw) {
        glm::vec3 forward = glm::normalize(target - position);

        // Compute the right vector
        glm::vec3 right = glm::normalize(glm::cross(up, forward));

        // Create rotation matrices for pitch and yaw
        glm::mat4 pitchRotation = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), right);
        glm::mat4 yawRotation = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), up);

        // Apply pitch and yaw rotations to the forward vector
        glm::vec4 newForward = yawRotation * pitchRotation * glm::vec4(forward, 0.0f);
        target = position + glm::normalize(glm::vec3(newForward));

        updateViewMatrix();
    }

    void moveForward(float distance) {
        glm::vec3 forward = getForwardVector();
        position += forward * distance;
        target += forward * distance;
        updateViewMatrix();
    }

    void moveRight(float distance) {
        glm::vec3 right = getRightVector();
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
    glm::vec3 position;   // Camera position in world space
    glm::vec3 target;     // Point the camera is looking at
    glm::vec3 up;         // Up vector (typically the world up direction)
    float fov;            // Field of view in degrees
    float aspectRatio;    // Aspect ratio of the camera (width / height)
    float mNear;           // mNear clipping plane distance
    float mFar;            // mFar clipping plane distance
    glm::mat4 viewMatrix; // View matrix
    uBuffer<> ubo;        // Uniform buffer object for storing view and projection matrices

    void updateViewMatrix() {
        viewMatrix = glm::lookAt(position, target, up);
    }
};

#include "entity.h" // Make sure this is included for the 'ent' class

class ent_camera : public ent, public Camera // Inherit from both ent and Camera
{
public:
    glm::vec3 last_cam_pos;
    glm::vec3 delta_cam_pos;
    ent_camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3 upDirection = glm::vec3(0.0f, 1.0f, 0.0f),
        float fieldOfView = 70.0f,
        float aspectRatio = 16.0f / 9.0f,
        float mNearClip = 0.1f,
        float mFarClip = 1000.0f)
        : Camera(pos, target, upDirection, fieldOfView, aspectRatio, mNearClip, mFarClip)
    {
    }

    Camera& get_camera() { return *this; } // Return reference to Camera
    ent& get_ent() { return *this; } // Return reference to ent

    enum : uint16_t // Corrected enum definition
    {
        GET_POSITION = 4,
        GET_FORWARD_VECTOR = 5,
        GET_LEFT_VECTOR = 6,
        GET_UP_VECTOR = 7,
        LOOK_AT = 8,
        ROTATE = 9,
    };

    virtual void exec(ent_request stream) override
    {
        small_size_t offset = 0;
        switch (stream.command_ID)
        {
        case NULL_OPERATION:
            break; // No-op packet
        case INIT:
            init();
            stream.command_ID = 0xffff; // Command executed successfully
            break;
        case TICK:
            tick();
            stream.command_ID = 0xffff; // Command executed successfully
            break;
        case DRAW:
            draw();
            stream.command_ID = 0xffff; // Command executed successfully
            break;
        case GET_POSITION:
            stream.write_next<glm::vec3>(getPosition(), offset);
            stream.command_ID = 0xffff; // Command executed successfully
            break;
        case GET_FORWARD_VECTOR:
        {
            glm::vec3 forwardVector = getForwardVector();
            stream.write_next<glm::vec3>(forwardVector, offset);
            stream.command_ID = 0xffff; // Command executed successfully
            break;
        }
        case GET_LEFT_VECTOR:
        {
            glm::vec3 leftVector = glm::normalize(glm::cross(getUpDirection(), getForwardVector()));
            stream.write_next<glm::vec3>(leftVector, offset);
            stream.command_ID = 0xffff; // Command executed successfully
            break;
        }
        case GET_UP_VECTOR:
        {
            stream.write_next<glm::vec3>(getUpDirection(), offset);
            stream.command_ID = 0xffff; // Command executed successfully
            break;
        }
        case LOOK_AT:
        {
            glm::vec3 newTarget = stream.read_next<glm::vec3>(offset); // Assuming you have a method to read the target position from the stream
            lookAt(newTarget);
            stream.command_ID = 0xffff; // Command executed successfully
            break;
        }
        case ROTATE:
        {
            float pitch = stream.read_next<float>(offset); // Assuming you have a method to read the pitch
            float yaw = stream.read_next<float>(offset);   // Assuming you have a method to read the yaw
            rotate(pitch, yaw);
            stream.command_ID = 0xffff; // Command executed successfully
            break;
        }
        default:
            stream.command_ID = 0xfffe; // Unknown command error code
            break;
        }
    }

};
