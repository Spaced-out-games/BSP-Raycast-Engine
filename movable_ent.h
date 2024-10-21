#pragma once
#include "ent.h"
#include "glm_master.h"

// 3D entity with position and look-at vector for rotation
class ent_3d_2a : public ent{
public:
    ent_3d_2a() = default;
    vec3 get_forward_vector();
    vec3 get_right_vector();
    vec3 get_up_vector();
    void look_at(vec3 target_position);
    void rotate(vec2 rotation);
    void move(vec3 delta_position);
    void move_forward(float distance);
    void move_right(float distance);
    void move_up(float distance);
    const char* get_name() const override
    {
        return "ent_3d_2a";
    }

    void set_position(const vec3& new_position);
    void set_target(const vec3& new_target);

    const vec3& get_position();
    const vec3& get_target();
    mat4 get_matrix();

//private:
    vec3 position = { 0.0f, 0.0f, 0.0f };   // Entity position
    vec3 target = { 0.0f, 1.0f, 0.0f };   // Look-at direction
};

vec3 ent_3d_2a::get_forward_vector() {
    return glm::normalize(target - position);
}

mat4 ent_3d_2a::get_matrix() {
    // Calculate the forward vector
    vec3 forward = get_forward_vector();

    // Calculate the right vector
    vec3 right = get_right_vector();

    // Calculate the up vector
    vec3 up = get_up_vector();

    // Create a transformation matrix
    mat4 model = mat4(1.0f); // Identity matrix

    // Set the rotation part of the matrix
    model[0] = vec4(right, 0.0f); // Right
    model[1] = vec4(up, 0.0f);    // Up
    model[2] = vec4(-forward, 0.0f); // Forward (inverted for right-handed system)

    // Set the translation part of the matrix
    model[3] = vec4(position, 1.0f); // Position

    return model;
}

vec3 ent_3d_2a::get_right_vector() {
    return glm::normalize(glm::cross(get_forward_vector(), vec3(0.0f, 1.0f, 0.0f))); // World up vector
}

vec3 ent_3d_2a::get_up_vector() {
    return glm::normalize(glm::cross(get_right_vector(), get_forward_vector())); // Forward vector
}

void ent_3d_2a::look_at(vec3 target_position) {
    target = target_position; // Set look-at direction directly
}

void ent_3d_2a::rotate(vec2 rotation) {
    float& yawAngle = rotation.y;
    float& pitchAngle = rotation.x;

    const vec3 up = vec3(0.0f, 1.0f, 0.0f);

    vec3 forward = glm::normalize(target - position);

    // Compute the right vector
    vec3 right = glm::normalize(glm::cross(up, forward));

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
    mat4 pitchRotation = glm::rotate(mat4(1.0f), glm::radians(pitchAngle), right);
    mat4 yawRotation = glm::rotate(mat4(1.0f), glm::radians(yawAngle), up);

    // Apply pitch and yaw rotations to the forward vector
    vec4 newForward = yawRotation * pitchRotation * vec4(forward, 0.0f);
    target = position + glm::normalize(vec3(newForward));

    // Update the view matrix if necessary
    // updateViewMatrix();
}
void ent_3d_2a::move(vec3 delta_position) {
    position += delta_position;
}

void ent_3d_2a::move_forward(float distance) {
    vec3 forward = get_forward_vector();
    position += forward * distance;
    target += forward * distance;
    //updateViewMatrix();
}

void ent_3d_2a::move_right(float distance) {
    vec3 right = get_right_vector();
    position += right * distance;
    target += right * distance;
    //updateViewMatrix();
}

void ent_3d_2a::move_up(float distance) {
    const vec3& up = vec3(0.0f, 1.0f, 0.0f);
    position += up * distance;
    target += up * distance;
    //updateViewMatrix();
}

void ent_3d_2a::set_position(const vec3& new_position) {
    position = new_position;
}

void ent_3d_2a::set_target(const vec3& new_target) {
    target = new_target;
}

const vec3& ent_3d_2a::get_position() {
    return position;
}

const vec3& ent_3d_2a::get_target() {
    return target;
}
