#pragma once
#include "glm_master.h"


typedef struct Ray
{
	vec3 origin;
	vec3 direction; // Perhaps this can be compressed to only store the first 16 bits (sign, exponent, some of mantissa);
	Ray(vec3 origin, vec3 direction) : origin(origin), direction(direction) {};
};
typedef struct Line
{
	vec3 start;
	vec3 end; // Perhaps this can be compressed to only store the first 16 bits (sign, exponent, some of mantissa);
	Line(vec3 start, vec3 end) : start(start), end(end) {};
};
class Polygon {
public:
    std::vector<vec3> vertices;

    // Constructor to initialize a polygon with given vertices
    Polygon(const std::vector<vec3>& verts) : vertices(verts) {}

    // Method to calculate and return the normal of the polygon
    vec3 getNormal() const {
        if (vertices.size() < 3) {
            std::cerr << "Polygon must have at least 3 vertices to compute a normal.\n";
            return vec3(0.0f); // Return a zero vector if not enough vertices
        }

        // Assume vertices are defined in a counterclockwise order
        vec3 edge1 = vertices[1] - vertices[0];
        vec3 edge2 = vertices[2] - vertices[0];

        // Compute the cross product of edge1 and edge2
        vec3 normal = glm::cross(edge1, edge2);

        // Normalize the normal vector
        normal = glm::normalize(normal);

        return normal;
    }
};


typedef struct Triangle
{
	vec3 vertices[3];
    vec3 get_normal()
    {
        vec3 v0 = vertices[0];
        vec3 v1 = vertices[1];
        vec3 v2 = vertices[2];

        // Calculate normal
        vec3 edge1 = v1 - v0;
        vec3 edge2 = v2 - v0;
        vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        return normal;
    }
};
