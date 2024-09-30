#pragma once
#include <glm/glm.hpp>

typedef struct Ray
{
	glm::vec3 origin;
	glm::vec3 direction; // Perhaps this can be compressed to only store the first 16 bits (sign, exponent, some of mantissa);
	Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction) {};
};
typedef struct Line
{
	glm::vec3 start;
	glm::vec3 end; // Perhaps this can be compressed to only store the first 16 bits (sign, exponent, some of mantissa);
	Line(glm::vec3 start, glm::vec3 end) : start(start), end(end) {};
};
class Polygon {
public:
    std::vector<glm::vec3> vertices;

    // Constructor to initialize a polygon with given vertices
    Polygon(const std::vector<glm::vec3>& verts) : vertices(verts) {}

    // Method to calculate and return the normal of the polygon
    glm::vec3 getNormal() const {
        if (vertices.size() < 3) {
            std::cerr << "Polygon must have at least 3 vertices to compute a normal.\n";
            return glm::vec3(0.0f); // Return a zero vector if not enough vertices
        }

        // Assume vertices are defined in a counterclockwise order
        glm::vec3 edge1 = vertices[1] - vertices[0];
        glm::vec3 edge2 = vertices[2] - vertices[0];

        // Compute the cross product of edge1 and edge2
        glm::vec3 normal = glm::cross(edge1, edge2);

        // Normalize the normal vector
        normal = glm::normalize(normal);

        return normal;
    }
};


typedef struct Triangle
{
	glm::vec3 vertices[3];
    glm::vec3 get_normal()
    {
        glm::vec3 v0 = vertices[0];
        glm::vec3 v1 = vertices[1];
        glm::vec3 v2 = vertices[2];

        // Calculate normal
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        return normal;
    }
};
