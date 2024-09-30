#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <limits>
#include <stdexcept>
#include <iostream>

// Constants
const uint32_t NULL_INDEX = std::numeric_limits<uint32_t>::max();

struct bsp_plane {
    bsp_plane(glm::vec3 normal, float distance)
        : normal(glm::normalize(normal)), distance(distance) {}

    glm::vec3 normal; // Normal vector of the plane
    float distance;   // Signed distance from the origin to the plane

    glm::vec3 getPlaneCenterPoint() const {
        return normal * distance;
    }

    glm::mat4 getMatrix() const {
        glm::vec3 planeCenter = getPlaneCenterPoint();
        glm::mat4 rotationMatrix = glm::mat4(1.0f);
        glm::vec3 right = glm::normalize(glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::vec3 up = glm::normalize(glm::cross(right, normal));

        rotationMatrix[0] = glm::vec4(right, 0.0f);
        rotationMatrix[1] = glm::vec4(up, 0.0f);
        rotationMatrix[2] = glm::vec4(-normal, 0.0f);
        rotationMatrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), planeCenter);
        glm::mat4 transform = translationMatrix * rotationMatrix;

        return transform;
    }
    float DistanceFromPoint(const glm::vec3& point) const {
        return glm::dot(normal, point) - distance;
    }
};

class BSPNode {
public:
    bsp_plane plane;
    uint32_t front; // Index of the front child node
    uint32_t back;  // Index of the back child node
    uint16_t texture_ID;


    BSPNode(bsp_plane plane)
        : plane(plane), front(NULL_INDEX), back(NULL_INDEX), texture_ID (1) {}
    bool isLeaf() { return front == NULL_INDEX && back == NULL_INDEX; }// We can probably assume this is good enough
    bool isSolidNode() { return isLeaf(); }
    // Insert a plane into the BSP tree
    void insert(const bsp_plane& newPlane, std::vector<BSPNode>& nodes) {
        float dist = glm::dot(newPlane.normal, plane.getPlaneCenterPoint()) - plane.distance;

        if (dist > 0) {
            // The new plane is in the front space
            if (front != NULL_INDEX) {
                nodes[front].insert(newPlane, nodes);
            }
            else {
                front = nodes.size();
                nodes.emplace_back(newPlane);
            }
        }
        else {
            // The new plane is in the back space
            if (back != NULL_INDEX) {
                nodes[back].insert(newPlane, nodes);
            }
            else {
                back = nodes.size();
                nodes.emplace_back(newPlane);
            }
        }
    }
    // Checks if a point intersects this plane
    bool isSolid(const glm::vec3& point) const {
        return pointIsBehind(point) && texture_ID != 0;
    }
    bool pointIsInFront(const glm::vec3& point) const {
        float distance = glm::dot(point, plane.normal) - plane.distance;
        return distance > 0;
    }
    bool pointIsBehind(const glm::vec3& point) const {
        float distance = glm::dot(point, plane.normal) - plane.distance;
        return distance < 0;
    }
    bool hasFront() { return front != NULL_INDEX; }
    bool hasBack() { return back != NULL_INDEX; }


};

struct HitResult {
    glm::vec3 hit_position;
    uint32_t node_index;

    HitResult() : hit_position(glm::vec3(0.0f)), node_index(NULL_INDEX) {}
    HitResult(const glm::vec3& position, uint32_t index) : hit_position(position), node_index(index) {}
    explicit operator bool() const {
        return node_index != NULL_INDEX;
    }
    void print() const {
        //std::cout << "Hit Position: (" << hit_position.x << ", " << hit_position.y << ", " << hit_position.z << "), Index: " << node_index << '\n';
    }
};

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
    Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction) {}
    Ray() : origin({ 0, 0, 0 }), direction({ 0, 0, 0 }) {}
};
struct RayhitResult
{
    glm::vec3 hit_location;
    uint32_t index; // NULL_INDEX if no hit
    RayhitResult(): hit_location(), index(NULL_INDEX) {}
    RayhitResult(glm::vec3 hit_location, uint32_t node_index) : hit_location(hit_location), index(node_index) {};
    bool hit() { return index != NULL_INDEX; }

};
class BSPTree {
public:


    BSPTree() : rootIndex(NULL_INDEX) {}
    BSPTree(const std::vector<bsp_plane>& planes) {
        if (planes.empty()) {
            throw std::invalid_argument("Cannot create BSPTree from empty plane list");
        }


        // Insert the planes into the tree
        for (const auto& plane : planes) {
            addNode(plane);
        }

    }
    // Adds a node to the BSP tree
    void addNode(const bsp_plane& newPlane) {
        if (rootIndex == NULL_INDEX) {
            rootIndex = nodes.size();
            nodes.emplace_back(newPlane);
        }
        else {
            nodes[rootIndex].insert(newPlane, nodes);
        }
    }
    BSPNode* getNode(uint32_t index)
    {
        return &(nodes[index]);
    }

    uint32_t isSolid_recursive(const glm::vec3& point, uint32_t node_idx) const {
        if (node_idx == NULL_INDEX) {
            // Reached a null index, which means this region is empty
            return NULL_INDEX;
        }

        float distance = glm::dot(point, nodes[node_idx].plane.normal) - nodes[node_idx].plane.distance;

        if (distance < 0) {
            // The point is in the back subspace
            if (nodes[node_idx].back != NULL_INDEX) {
                // Continue checking recursively in the back subtree
                return isSolid_recursive(point, nodes[node_idx].back);
            }
            else {
                // If there's no back subtree, this region is solid
                return node_idx;
            }
        }
        else {
            // The point is in the front subspace
            if (nodes[node_idx].front != NULL_INDEX) {
                // Continue checking recursively in the front subtree
                return isSolid_recursive(point, nodes[node_idx].front);
            }
            else {
                // If there's no front subtree, this region is empty
                return NULL_INDEX;
            }
        }
    }
    uint32_t isSolid_recursive(const glm::vec3& point) const {
        return isSolid_recursive(point, rootIndex);
    }
    // Calculate the intersection point between a ray and a plane
    RayhitResult intersectRayWithPlane(const Ray& ray, uint32_t node_index) const {

        float denom = glm::dot(ray.direction, nodes[node_index].plane.normal);
        if (fabs(denom) > 1e-6) {
            float t = (nodes[node_index].plane.distance - glm::dot(ray.origin, nodes[node_index].plane.normal)) / denom;
            if (t >= 0) {
                return RayhitResult(ray.origin + t * ray.direction, node_index);
            }
        }
        return RayhitResult();
    }

    bool isPointIntersectingPlane(const glm::vec3& point, uint32_t node_index, float tolerance = 1e-6f) const {
        // Calculate the signed distance from the point to the plane
        float distance = glm::dot(point, nodes[node_index].plane.normal) - nodes[node_index].plane.distance;

        // Check if the absolute distance is within the tolerance
        return fabs(distance) <= tolerance;
    }


    uint32_t getNodeFromPoint(glm::vec3 point, uint32_t root_index)
    {
        BSPNode root = nodes[root_index];

        float distance = root.plane.DistanceFromPoint(point);

        // If the point is behind the plane
        if (distance < 0)
        {
            if (root.hasBack())
            {
                return getNodeFromPoint(point, root.back);
            }
            return root_index; // Leaf node or no back node, return current node index
        }
        // If the point is in front of or on the plane
        else
        {
            if (root.hasFront())
            {
                return getNodeFromPoint(point, root.front);
            }
            return root_index; // Leaf node or no front node, return current node index
        }
    }
    uint32_t getNodeFromPoint(glm::vec3 point)
    {
        return getNodeFromPoint(point, rootIndex);
    }

    RayhitResult RaycastBSP(const Ray& ray, uint32_t nodeIndex) {
        if (nodeIndex == NULL_INDEX) {
            return RayhitResult(); // No hit
        }

        BSPNode& node = nodes[nodeIndex];

        // If this is a leaf node, determine if it is solid
        if (node.isLeaf()) {
            if (node.isSolidNode()) {
                return RayhitResult(ray.origin, nodeIndex);
            }
            else {
                return RayhitResult();
            }
        }

        // Calculate the distance to the plane
        float denom = glm::dot(ray.direction, node.plane.normal);
        if (fabs(denom) < 1e-6f) {
            // Ray is parallel to the plane
            if (node.plane.DistanceFromPoint(ray.origin) > 0) {
                return RaycastBSP(ray, node.front);
            }
            else {
                return RaycastBSP(ray, node.back);
            }
        }

        // Calculate intersection point
        float t = (node.plane.distance - glm::dot(ray.origin, node.plane.normal)) / denom;
        if (t < 0) {
            // Intersection is behind the ray's origin
            if (node.plane.DistanceFromPoint(ray.origin) > 0) {
                return RaycastBSP(ray, node.front);
            }
            else {
                return RaycastBSP(ray, node.back);
            }
        }

        // Intersection is in front of or at the ray's origin
        glm::vec3 hitLocation = ray.origin + t * ray.direction;
        RayhitResult result;

        if (node.plane.DistanceFromPoint(ray.origin) > 0) {
            result = RaycastBSP(ray, node.front);
            if (result.hit()) return result;

            return RaycastBSP({ hitLocation, ray.direction }, node.back);
        }
        else {
            result = RaycastBSP(ray, node.back);
            if (result.hit()) return result;

            return RaycastBSP({ hitLocation, ray.direction }, node.front);
        }
    }
    /*
    RayhitResult RaycastBSP(const Ray& ray, uint32_t nodeIndex) {
        if (nodeIndex == NULL_INDEX) {
            return RayhitResult(); // No hit
        }

        const BSPNode& node = nodes[nodeIndex];

        // If this is a leaf node, determine if it is solid
        if (node.back == NULL_INDEX && node.front == NULL_INDEX) {
            return RayhitResult(ray.origin, nodeIndex);
        }

        // Calculate the distance to the plane
        float denom = glm::dot(ray.direction, node.plane.normal);
        float t = (node.plane.distance - glm::dot(ray.origin, node.plane.normal)) / denom;

        // Determine the side of the plane the ray originates from
        bool startInFront = node.plane.DistanceFromPoint(ray.origin) > 0;

        // If the ray is parallel to the plane, continue on the side where the ray starts
        if (fabs(denom) < 1e-6f) {
            if (startInFront) {
                return RaycastBSP(ray, node.front);
            }
            else {
                return RaycastBSP(ray, node.back);
            }
        }

        // If t < 0, the intersection is behind the ray's origin, so only traverse the initial node side
        if (t < 0) {
            if (startInFront) {
                return RaycastBSP(ray, node.front);
            }
            else {
                return RaycastBSP(ray, node.back);
            }
        }

        // If t >= 0, there's an intersection, so first traverse the side the ray starts on
        if (startInFront) {
            RayhitResult result = RaycastBSP(ray, node.front);
            if (result.hit()) return result;

            // If no hit on the front side, check the back side after the intersection
            glm::vec3 hitLocation = ray.origin + t * ray.direction;
            return RaycastBSP({ hitLocation, ray.direction }, node.back);
        }
        else {
            RayhitResult result = RaycastBSP(ray, node.back);
            if (result.hit()) return result;

            // If no hit on the back side, check the front side after the intersection
            glm::vec3 hitLocation = ray.origin + t * ray.direction;
            return RaycastBSP({ hitLocation, ray.direction }, node.front);
        }
    }
    */
    /*RayhitResult RaycastBSP(const Ray& ray, uint32_t nodeIndex) {
        if (nodeIndex == NULL_INDEX) {
            return RayhitResult(); // No hit
        }

        BSPNode& node = nodes[nodeIndex];

        // If this is a leaf node, determine if it is solid
        if (node.isLeaf()) {
            if (node.isSolidNode()) {
                // Ray has hit a solid space
                return RayhitResult(ray.origin, nodeIndex);
            }
            else {
                // Ray has passed through an empty space
                return RayhitResult();
            }
        }

        // Calculate the distance to the plane
        float denom = glm::dot(ray.direction, node.plane.normal);
        float t = (node.plane.distance - glm::dot(ray.origin, node.plane.normal)) / denom;

        // Determine the side of the plane the ray originates from
        bool startInFront = node.plane.DistanceFromPoint(ray.origin) > 0;

        // If the ray is parallel to the plane, continue on the side where the ray starts
        if (fabs(denom) < 1e-6f) {
            if (startInFront) {
                return RaycastBSP(ray, node.front);
            }
            else {
                return RaycastBSP(ray, node.back);
            }
        }

        // If t < 0, the intersection is behind the ray's origin, so only traverse the initial node side
        if (t < 0) {
            if (startInFront) {
                return RaycastBSP(ray, node.front);
            }
            else {
                return RaycastBSP(ray, node.back);
            }
        }

        // If t >= 0, there's an intersection, so first traverse the side the ray starts on
        if (startInFront) {
            RayhitResult result = RaycastBSP(ray, node.front);
            if (result.hit()) return result;

            // If no hit on the front side, check the back side after the intersection
            glm::vec3 hitLocation = ray.origin + t * ray.direction;
            return RaycastBSP({ hitLocation, ray.direction }, node.back);
        }
        else {
            RayhitResult result = RaycastBSP(ray, node.back);
            if (result.hit()) return result;

            // If no hit on the back side, check the front side after the intersection
            glm::vec3 hitLocation = ray.origin + t * ray.direction;
            return RaycastBSP({ hitLocation, ray.direction }, node.front);
        }
    }
    */
    // shitty, basic raycasting

    /*
    RayhitResult raycast(const Ray& ray, uint32_t node_index) const {
        RayhitResult result;
        Ray rayCopy = ray;
        uint32_t hit_node_index = isSolid_recursive(rayCopy.origin);
        while (hit_node_index == NULL_INDEX)
        {
            rayCopy.origin += rayCopy.direction;
            hit_node_index = isSolid_recursive(rayCopy.origin);
        }
        rayCopy.origin -= rayCopy.direction;
        return  intersectRayWithPlane(rayCopy, hit_node_index);

    }
    */


    std::vector<BSPNode> nodes;
    uint32_t rootIndex; // Index of the root node
};

// Function to create a BSP tree from a list of planes