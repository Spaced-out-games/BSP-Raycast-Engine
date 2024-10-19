#pragma once
#include "ent.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

// 3D entity with position and look-at vector for rotation
class ent_3d_2a : public ent {
public:
    ent_3d_2a() = default;

    glm::vec3 get_forward_vector();
    glm::vec3 get_right_vector();
    glm::vec3 get_up_vector();
    void look_at(glm::vec3& target_position);
    void rotate(glm::vec2 rotation);
    void move(glm::vec3& delta_position);
    void move_forward(float distance);
    void move_right(float distance);
    void move_up(float distance);

    void set_position(const glm::vec3& new_position);
    void set_target(const glm::vec3& new_target);

    const glm::vec3& get_position();
    const glm::vec3& get_target();
    glm::mat4 get_matrix();

//private:
    glm::vec3 position = { 0.0f, 0.0f, 0.0f };   // Entity position
    glm::vec3 target = { 0.0f, 1.0f, 0.0f };   // Look-at direction
};

glm::vec3 ent_3d_2a::get_forward_vector() {
    return glm::normalize(target - position);
}

glm::mat4 ent_3d_2a::get_matrix() {
    // Calculate the forward vector
    glm::vec3 forward = get_forward_vector();

    // Calculate the right vector
    glm::vec3 right = get_right_vector();

    // Calculate the up vector
    glm::vec3 up = get_up_vector();

    // Create a transformation matrix
    glm::mat4 model = glm::mat4(1.0f); // Identity matrix

    // Set the rotation part of the matrix
    model[0] = glm::vec4(right, 0.0f); // Right
    model[1] = glm::vec4(up, 0.0f);    // Up
    model[2] = glm::vec4(-forward, 0.0f); // Forward (inverted for right-handed system)

    // Set the translation part of the matrix
    model[3] = glm::vec4(position, 1.0f); // Position

    return model;
}

glm::vec3 ent_3d_2a::get_right_vector() {
    return glm::normalize(glm::cross(get_forward_vector(), glm::vec3(0.0f, 1.0f, 0.0f))); // World up vector
}

glm::vec3 ent_3d_2a::get_up_vector() {
    return glm::normalize(glm::cross(get_right_vector(), get_forward_vector())); // Forward vector
}

void ent_3d_2a::look_at(glm::vec3& target_position) {
    target = target_position; // Set look-at direction directly
}

void ent_3d_2a::rotate(glm::vec2 rotation) {
    float& yawAngle = rotation.y;
    float& pitchAngle = rotation.x;

    const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 forward = glm::normalize(target - position);

    // Compute the right vector
    glm::vec3 right = glm::normalize(glm::cross(up, forward));

    // Apply yaw rotation first (y-axis)
    yawAngle += rotation.y; // Assuming you have a yawAngle variable to accumulate yaw

    // Clamp yaw to prevent extreme values if desired (optional)
    if (yawAngle > 360.0f) {
        yawAngle -= 360.0f;
    }
    else if (yawAngle < -360.0f) {
        yawAngle += 360.0f;
    }

    // Apply pitch rotation (x-axis)
    pitchAngle += rotation.x; // Assuming you have a pitchAngle variable to accumulate pitch

    // Define clamping limits in degrees (convert to radians for calculations)
    const float maxPitch = 89.0f; // max pitch in degrees
    const float minPitch = -89.0f; // min pitch in degrees
    pitchAngle = glm::clamp(pitchAngle, minPitch, maxPitch);

    // Create rotation matrices for pitch and yaw
    glm::mat4 pitchRotation = glm::rotate(glm::mat4(1.0f), glm::radians(pitchAngle), right);
    glm::mat4 yawRotation = glm::rotate(glm::mat4(1.0f), glm::radians(yawAngle), up);

    // Apply pitch and yaw rotations to the forward vector
    glm::vec4 newForward = yawRotation * pitchRotation * glm::vec4(forward, 0.0f);
    target = position + glm::normalize(glm::vec3(newForward));

    // Update the view matrix if necessary
    // updateViewMatrix();
}
void ent_3d_2a::move(glm::vec3& delta_position) {
    position += delta_position;
}

void ent_3d_2a::move_forward(float distance) {
    glm::vec3 forward = get_forward_vector();
    position += forward * distance;
    target += forward * distance;
    //updateViewMatrix();
}

void ent_3d_2a::move_right(float distance) {
    glm::vec3 right = get_right_vector();
    position += right * distance;
    target += right * distance;
    //updateViewMatrix();
}

void ent_3d_2a::move_up(float distance) {
    const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f);
    position += up * distance;
    target += up * distance;
    //updateViewMatrix();
}

void ent_3d_2a::set_position(const glm::vec3& new_position) {
    position = new_position;
}

void ent_3d_2a::set_target(const glm::vec3& new_target) {
    target = new_target;
}

const glm::vec3& ent_3d_2a::get_position() {
    return position;
}

const glm::vec3& ent_3d_2a::get_target() {
    return target;
}
