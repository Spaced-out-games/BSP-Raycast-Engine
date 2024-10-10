#pragma once
#include <glm/glm.hpp>
#include "Mesh.h"
#include "shader.h"
#include "Texture.h"
#include "shapes.h"
#include <array>
#include <set>

typedef struct brush_triangle
{
	glm::vec3 points[3];
	glm::vec3 get_normal();
	uint16_t material_ID;
};

typedef struct brush_face
{
	glm::vec3 normal;
	uint16_t material_ID;
	brush_face(brush_triangle& triangle)
	{
		normal = triangle.get_normal();
		material_ID = triangle.material_ID;

	}
};
typedef struct brush_triangle_reference
{
	uint16_t indices[3];
};


typedef struct Brush
{
	GLuint VAO;
	GLuint vertex_VBO;
	GLuint face_VBO;
	GLuint EBO;

	std::set <glm::vec3> points;
	std::vector <brush_face> faces;
	std::vector <brush_triangle_reference> triangles;

	void add_triangle(const brush_triangle& triangle) {
		// Add vertices to the set and store the index mapping
		uint32_t index1 = static_cast<uint32_t>(points.size());
		index1 = static_cast<uint32_t>(points.insert(triangle.points[0]).first - points.begin());

		uint32_t index2 = static_cast<uint32_t>(points.size());
		index2 = static_cast<uint32_t>(points.insert(triangle.points[1]).first - points.begin());

		uint32_t index3 = static_cast<uint32_t>(points.size());
		index3 = static_cast<uint32_t>(points.insert(triangle.points[2]).first - points.begin());

		// Store the face attributes directly using emplace_back
		faces.emplace_back(FaceAttributes{ triangle.get_normal(), triangle.material_ID });

		// Add indices for EBO
		indices.push_back(index1);
		indices.push_back(index2);
		indices.push_back(index3);
	}

	void init()
	{
		// Create the VAO
		glGenVertexArrays(1, &VAO);

		// Bind it
		glBindVertexArrays(VAO);
	}
};