// Definitely not stolen from Quake ; )
#pragma once
#include <iostream>
#include <memory>
#include <random>
#include <array>
#include <vector>
#include <glm/glm.hpp>
#include "debug_utils.h"
#include "mesh_types.h"



// Pointer to a node in the BSP. Positive values act as indices; negative indicates leaf contents.
using node_ptr_t = int32_t;



// Bit mask as to whether the node is a leaf or not
constexpr node_ptr_t BSP_LEAF_NODE = -1;
// For when a a collision query hits nothing
constexpr node_ptr_t BSP_NO_HIT = -1;
// Bit mask as to whether this leaf node has a collision hull or not
constexpr node_ptr_t BSP_HULL_NODE = 0b10000000000000000000000000000000;
//Whether or not to draw the leaf
constexpr node_ptr_t BSP_DRAW_NODE = 0b01000000000000000000000000000000;
// Bit mask to receive the content of the node
constexpr node_ptr_t BSP_NODE_CONTENT = 0b00000111111111111111111111111111;



// Needs to be a negative value
using node_content_t = node_ptr_t;

// Pointer to a plane in the BSP. All indices are positive
using plane_ptr_t = uint32_t;

#include "shapes.h"

// Defines a partitioning plane in 3D space
typedef struct bsp_plane
{
    // Creates a BSP plane from a normal vector and a distance from the origin
    bsp_plane(glm::vec3 normal, float distance) : normal(glm::normalize(normal)), distance(distance) {}

    bsp_plane(glm::vec3 normal, float distance, glm::vec3 offset, glm::vec3 scale)
        : normal(glm::normalize(normal)), distance(distance), scale(scale), offset(offset) {}

    // New constructor that takes a polygon
    bsp_plane(const polygon& poly) {
        // Calculate the normal from the polygon's vertices
        normal = poly.getNormal();

        // Calculate d using the plane equation: dot(normal, point) + d = 0
        // We use the first vertex of the polygon to find the distance
        distance = -glm::dot(normal, poly.vertices[0]);
    }

    // The direction of the front of the plane
    glm::vec3 normal;

    // Signed distance from the origin
    float distance;

    #ifdef _DEBUG
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
    glm::vec3 offset = { 0,0,0 };
    #endif

    // Utility function for determining which side a point is on versus a plane. 0: behind, 1: in front
    bool get_partition_side(glm::vec3 point) {
        return (glm::dot(normal, point) - distance) > 0;
    }

    // Utility to convert the normal / distance pair into a position in 3D space
    glm::vec3 get_position() {
        return normal * distance;
    }

    // Utility function to get the model matrix. Useful for debugging
    glm::mat4 get_model_matrix() {
        glm::vec3 planeCenter = get_position() + offset;
        glm::mat4 rotationMatrix = glm::mat4(1.0f);
        glm::vec3 right = glm::normalize(glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::vec3 up = glm::normalize(glm::cross(right, normal));

        rotationMatrix[0] = glm::vec4(right, 0.0f);
        rotationMatrix[1] = glm::vec4(up, 0.0f);
        rotationMatrix[2] = glm::vec4(-normal, 0.0f);
        rotationMatrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), planeCenter);
        glm::mat4 transform = translationMatrix * rotationMatrix;

        return glm::scale(transform, glm::vec3(distance, distance, 1) * scale);
    }
};


// Defines a node in a BSP tree
typedef struct bsp_node
{
	// Default constructor
	bsp_node() : plane(BSP_LEAF_NODE), front(BSP_LEAF_NODE), back(BSP_LEAF_NODE) {};
	bsp_node(plane_ptr_t plane) : plane(plane), front(BSP_LEAF_NODE | BSP_HULL_NODE), back(BSP_LEAF_NODE | BSP_HULL_NODE) {};

	// Creates a node with two child nodes
	bsp_node(plane_ptr_t plane, node_ptr_t front, node_ptr_t back) : plane(plane), front(front), back(back) {};
	// Creates a leaf node
	bsp_node(plane_ptr_t plane, node_content_t contents) : plane(plane)
	{
		// If in debug mode, assert that the contents are negative
		#ifdef _DEBUG
		runtime_assert(contents > 0, "ERROR: invalid leaf contents. Use a negative value. ");
		#endif
		front = contents;
		back = BSP_LEAF_NODE | BSP_HULL_NODE;
	}


	// Gets the front node
	node_ptr_t get_front()
	{
		return front;
	}
	// Gets the back node
	node_ptr_t get_back()
	{
		return back;
	}
	// Gets the contents.
	node_ptr_t get_contents()
	{
		return front;
	}
	// gets the plane pointer
	plane_ptr_t get_plane()
	{
		return plane;
	}
	// Whether or not the node is solid and a leaf
	bool isSolid()
	{
		return (front < 0) && (front & BSP_HULL_NODE);
	}
	// Returns 1 if the node is a leaf
	bool isLeaf()
	{
		return (front < 0);
	}

	//private: // commented until I do some more boilerplate
	// A pointer to a partitioning plane
	plane_ptr_t plane;
	// Pointer to the front plane (or contents)
	node_ptr_t front;
	// Pointer to the back plane
	node_ptr_t back;
};
typedef struct hit_result
{
	glm::vec3 location;
	glm::vec3 normal;
	node_ptr_t node;
};

typedef struct bsp_tree {
    // Constructors
    bsp_tree() {}
    bsp_tree(std::vector<bsp_plane> planes) : planes(planes) {}
    bsp_tree(std::vector<polygon> polygons) {
        for (const auto& poly : polygons) {
            bsp_plane plane(poly);
            planes.push_back(plane);
        }
        build();  // Build the tree after planes are set up
    }

    // Node and plane accessors
    bsp_node& get_node(node_ptr_t node_index) { return nodes[node_index]; }
    bsp_plane& get_plane(plane_ptr_t plane_index) { return planes[plane_index]; }
    bsp_plane& get_node_plane(node_ptr_t node_index) { return planes[nodes[node_index].get_plane()]; }
    bsp_node& get_node_front(node_ptr_t node_index) { return nodes[nodes[node_index].get_front()]; }
    bsp_node& get_node_front(bsp_node& parent) { return nodes[parent.get_front()]; }
    bsp_node& get_node_back(node_ptr_t node_index) { return nodes[nodes[node_index].get_back()]; }
    bsp_node& get_node_back(bsp_node& parent) { return nodes[parent.get_back()]; }

    // Node state queries
    bool isSolid(node_ptr_t node_index) { return nodes[node_index].isSolid(); }
    bool isSolid(bsp_node& node) { return node.isSolid(); }
    bool isLeaf(node_ptr_t node_index) { return nodes[node_index].isLeaf(); }
    bool isLeaf(bsp_node& node) { return node.isLeaf(); }

    // Raycasting and solid check
    bool isSolid(glm::vec3& point) {
        return isSolid_recursive(point, root_index);
    }

    bool isSolid_recursive(glm::vec3& point, node_ptr_t node_index) {
        bool side = planes[nodes[node_index].plane].get_partition_side(point);
        if (side) {
            if (nodes[node_index].front >= 0) {
                return isSolid_recursive(point, nodes[node_index].front);
            }
            else {
                return !side;
            }
        }
        else {
            if (nodes[node_index].back >= 0) {
                return isSolid_recursive(point, nodes[node_index].back);
            }
            else {
                return !side;
            }
        }
    }

    node_content_t getSolid_recursive(glm::vec3& point, node_ptr_t node_index) {
        bsp_node& node = nodes[node_index];
        bsp_plane& plane = planes[node.plane];

        bool side = plane.get_partition_side(point);

        if (side) {
            if (node.front >= 0) {
                return getSolid_recursive(point, node.front);
            }
            else {
                return -1;
            }
        }
        else {
            if (node.back >= 0) {
                return getSolid_recursive(point, node.back);
            }
            else {
                return node_index;
            }
        }
    }

    // Tree validity check
    bool isValid() {
        return root_index != BSP_LEAF_NODE;
    }

    // Root access
    node_ptr_t get_root() {
        return root_index;
    }

    // Tree initialization
    void init(std::vector<bsp_plane> new_planes) {
        planes = new_planes;
    }

    // Tree building
    void build() {
        if (!nodes.empty()) {
            nodes.clear();
        }

        if (planes.empty()) {
            throw std::invalid_argument("Cannot create BSPTree from empty plane list");
        }

        // Insert the planes into the tree
        for (plane_ptr_t i = 0; i < planes.size(); i++) {
            addNode(i);
        }
    }

    void insert(node_ptr_t node_index, plane_ptr_t plane_index) {
        bsp_node& node = get_node(node_index);
        bsp_plane& plane = get_plane(plane_index);
        bsp_plane& node_plane = get_node_plane(node_index);

        float dist = glm::dot(plane.normal, node_plane.get_position()) - node_plane.distance;
        if (dist > 0) {
            if (node.front >= 0) {
                insert(node.front, plane_index);
            }
            else {
                node.front = nodes.size();
                nodes.emplace_back(plane_index);
            }
        }
        else {
            if (node.back >= 0) {
                insert(node.back, plane_index);
            }
            else {
                node.back = nodes.size();
                nodes.emplace_back(plane_index);
            }
        }
    }

    void addNode(plane_ptr_t plane_index) {
        if (root_index == BSP_LEAF_NODE) {
            root_index = nodes.size();
            nodes.emplace_back(plane_index);
        }
        else {
            insert(root_index, plane_index);
        }
    }

    // Visualization
    void visualize(tex_dynamic_mesh_3D& plane) {
        if (planes.empty()) return;

        plane.prepare_for_draw();
        plane.draw();
        for (plane_ptr_t i = 0; i < planes.size(); ++i) {
            if (i < planes.size()) {
                plane.set_transform(get_plane(i).get_model_matrix());
                plane.draw();
            }
        }
    }

    // Tree printing
    void print(node_ptr_t node_index, int depth = 0, bool isLeft = true) {
        for (int i = 0; i < depth; i++) {
            std::cout << "    ";
        }

        if (node_index != BSP_LEAF_NODE) {
            if (nodes[node_index].isLeaf()) {
                std::cout << (isLeft ? "Left Leaf: " : "Right Leaf: ") << node_index << '\n';
            }
            else {
                std::cout << (isLeft ? "Left " : "Right ") << "Node (" << node_index << ")\n";
                print(nodes[node_index].front, depth + 1, true);
                print(nodes[node_index].back, depth + 1, false);
            }
        }
    }

    // Ray-plane intersection
    float get_time_of_intersection(Ray ray, node_ptr_t node, const glm::vec3& intersection) {
        if (node == BSP_LEAF_NODE || node >= nodes.size()) {
            return false;
        }

        bsp_node& bspNode = nodes[node];
        bsp_plane& plane = planes[bspNode.get_plane()];

        float denom = glm::dot(plane.normal, ray.direction);
        if (glm::abs(denom) < 1e-6f) {
            return false;
        }

        float t = (plane.distance - glm::dot(plane.normal, ray.origin)) / denom;
        return t;
    }

    bool get_point_of_intersection(Ray ray, node_ptr_t node, glm::vec3& intersection) {
        if (node == BSP_LEAF_NODE || node >= nodes.size()) {
            return false;
        }

        bsp_node& bspNode = nodes[node];
        bsp_plane& plane = planes[bspNode.get_plane()];

        float denom = glm::dot(plane.normal, ray.direction);
        if (glm::abs(denom) < 1e-6f) {
            return false;
        }

        float t = (plane.distance - glm::dot(plane.normal, ray.origin)) / denom;
        if (t < 0.0f) {
            return false;
        }

        intersection = ray.origin + t * ray.direction;
        return true;
    }

    bool raycast(Line line, node_ptr_t node_index, glm::vec3& intersection) {
        bsp_node& node = nodes[node_index];
        bsp_plane& plane = planes[nodes[node_index].plane];

        if (node_index != BSP_LEAF_NODE) {
            intersection = line.start;
            return true;
        }
        else if (!node.isSolid() && node.isLeaf()) {
            return false;
        }

        float t1 = glm::dot(planes[node.plane].normal, line.start);
        return false; // Further raycast implementation needed
    }

    // Member variables
    std::vector<bsp_node> nodes;
    std::vector<bsp_plane> planes;
    node_ptr_t root_index = BSP_LEAF_NODE;

} bsp_tree;