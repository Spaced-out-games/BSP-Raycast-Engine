#ifndef GLM_UTILS_HPP
#define GLM_UTILS_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class glm_utils {
public:
    // Set position in a matrix
    static void setPosition(glm::mat4& matrix, const glm::vec3& position);

    // Set rotation in a matrix
    static void setRotation(glm::mat4& matrix, const glm::vec3& rotation); // Euler angles in degrees

    // Set scale in a matrix
    static void setScale(glm::mat4& matrix, const glm::vec3& scale);

    // Get position from a matrix
    static glm::vec3 getPosition(const glm::mat4& matrix);

    // Get rotation from a matrix (returns Euler angles in degrees)
    static glm::vec3 getRotation(const glm::mat4& matrix);

    // Get scale from a matrix
    static glm::vec3 getScale(const glm::mat4& matrix);

    // Generate a transformation matrix from position, rotation, and scale
    static glm::mat4 createTransformMatrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
};
void glm_utils::setPosition(glm::mat4& matrix, const glm::vec3& position) {
    matrix = glm::translate(glm::mat4(1.0f), position) * matrix;
}

void glm_utils::setRotation(glm::mat4& matrix, const glm::vec3& rotation) {
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    matrix = rotationMatrix * matrix;
}

void glm_utils::setScale(glm::mat4& matrix, const glm::vec3& scale) {
    matrix = glm::scale(glm::mat4(1.0f), scale) * matrix;
}

glm::vec3 glm_utils::getPosition(const glm::mat4& matrix) {
    return glm::vec3(matrix[3]); // Extract position from the last column
}

glm::vec3 glm_utils::getRotation(const glm::mat4& matrix) {
    glm::vec3 scale;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(matrix, scale, glm::quat(), glm::vec3(), skew, perspective);
    return glm::degrees(glm::eulerAngles(glm::quat(matrix))); // Convert radians to degrees
}

glm::vec3 glm_utils::getScale(const glm::mat4& matrix) {
    glm::vec3 scale;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(matrix, scale, glm::quat(), glm::vec3(), skew, perspective);
    return scale;
}

glm::mat4 glm_utils::createTransformMatrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
    glm::mat4 matrix = glm::mat4(1.0f); // Start with the identity matrix
    matrix = glm::translate(matrix, position);
    matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    matrix = glm::scale(matrix, scale);
    return matrix;
}
#endif // GLM_UTILS_HPP