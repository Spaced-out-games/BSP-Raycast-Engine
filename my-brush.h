#include <glm/glm.hpp>
#include "shader.h"
#include "Texture.h"
#include "shapes.h"
#include <array>
#include <unordered_set>




using point_cloud = std::vector<glm::vec3>;


inline glm::vec3 generate_normal(std::array<glm::vec3, 3> vertices) // Pass by value
{
    // Assume vertices are defined in a counterclockwise order
    glm::vec3 edge1 = vertices[1] - vertices[0];
    glm::vec3 edge2 = vertices[2] - vertices[0];

    // Compute the cross product of edge1 and edge2
    glm::vec3 normal = glm::cross(edge1, edge2);

    // Normalize the normal vector
    normal = glm::normalize(normal);

    return normal;
}


typedef struct brush_triangle
{
    std::array<glm::vec3, 3> vertices;
    uint16_t material_ID;
    brush_triangle(glm::vec3 p1 = {0,0,0}, glm::vec3 p2 = { 0,0,0 }, glm::vec3 p3 = { 0,0,0 }, uint16_t matID = 0): vertices{p1,p2,p3}, material_ID(matID) {}

	glm::vec3 get_normal()
    {
        return generate_normal(vertices);
    }
    uint16_t get_material() { return material_ID; }

};


typedef struct brush_triangle_reference
{
	uint16_t point_indices[3];
    uint16_t material_ID;
    brush_triangle_reference(uint16_t p1_idx = 0, uint16_t p2_idx = 0, uint16_t p3_idx = 0, uint16_t matID = 0) : point_indices{ p1_idx,p2_idx,p3_idx }, material_ID(matID) {}

    glm::vec3 get_normal(point_cloud& points) const
    {
        return generate_normal({
            points[point_indices[0]],
            points[point_indices[1]],
            points[point_indices[2]],

        })
    }
    uint16_t get_material() { return material_ID; }
};


// Define the brush_edge structure
struct brush_edge {
    std::array<glm::vec3, 2> vertices;
    brush_edge(std::array<glm::vec3, 2> verts) : vertices(verts) {}
    brush_edge(glm::vec3 p1, glm::vec3 p2) : vertices{p1, p2} {}

    // Hash function for brush_edge
    struct hash {
        std::size_t operator()(const brush_edge& edge) const {
            // Order the indices to ensure edges with swapped vertices are considered equal
            uint32_t v1 = std::hash<float>()(edge.vertices[0].x) ^
                std::hash<float>()(edge.vertices[0].y) ^
                std::hash<float>()(edge.vertices[0].z);
            uint32_t v2 = std::hash<float>()(edge.vertices[1].x) ^
                std::hash<float>()(edge.vertices[1].y) ^
                std::hash<float>()(edge.vertices[1].z);
            return std::hash<uint32_t>()((v1 << 16) | v2);  // Pack the two hashes into one integer for hashing
        }
    };

    // Equality comparison for brush_edge
    struct equal {
        bool operator()(const brush_edge& lhs, const brush_edge& rhs) const {
            return (lhs.vertices[0] == rhs.vertices[0] && lhs.vertices[1] == rhs.vertices[1]) ||
                (lhs.vertices[0] == rhs.vertices[1] && lhs.vertices[1] == rhs.vertices[0]);
        }
    };
};

struct brush_edge_reference {
    std::array<uint16_t, 2> indices;

    brush_edge_reference(uint16_t p1, uint16_t p2) {
        indices[0] = p1;
        indices[1] = p2;
    }

    // Hashing function
    struct hash {
        std::size_t operator()(const brush_edge_reference& edge) const {
            uint32_t v1 = std::hash<uint16_t>()(edge.indices[0]);
            uint32_t v2 = std::hash<uint16_t>()(edge.indices[1]);

            // Combine two 16-bit hashes into a 32-bit value, assuming the order of indices doesn't matter.
            // Using XOR to make the hash commutative (i.e., hash(1, 2) == hash(2, 1))
            return std::hash<uint32_t>()((v1 << 16) ^ v2);
        }
    };

    // Equality comparison for brush_edge_reference
    struct equal {
        bool operator()(const brush_edge_reference& lhs, const brush_edge_reference& rhs) const {
            // Edges are equal if they contain the same two vertices, in any order.
            return (lhs.indices[0] == rhs.indices[0] && lhs.indices[1] == rhs.indices[1]) ||
                (lhs.indices[0] == rhs.indices[1] && lhs.indices[1] == rhs.indices[0]);
        }
    };
};




typedef struct brush_model
{
    GLuint VAO = 0;
    GLuint VBO = 0;  // Point cloud
    GLuint wireframe_EBO = 0;
    GLuint solid_EBO = 0;

    // Method to prepare for wireframe rendering
    void prepare_for_wireframe_draw()
    {
        // Bind VAO
        glBindVertexArray(VAO);

        // Bind the wireframe EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireframe_EBO);
    }

    // Method to prepare for solid rendering
    void prepare_for_solid_draw()
    {
        // Bind VAO
        glBindVertexArray(VAO);

        // Bind the solid EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solid_EBO);
    }

    // Method to draw the solid geometry
    void draw_solid()
    {
        prepare_for_solid_draw(); // Ensure solid mode is prepared

        // Issue draw call for triangles
        glDrawElements(GL_TRIANGLES, get_solid_triangle_count() * 3, GL_UNSIGNED_SHORT, 0);

        // Unbind VAO
        glBindVertexArray(0);
    }

    // Method to draw the wireframe
    void draw_wireframe()
    {
        prepare_for_wireframe_draw();  // Ensure wireframe mode is prepared

        // Issue draw call for lines
        glDrawElements(GL_LINES, get_wireframe_edge_count() * 2, GL_UNSIGNED_SHORT, 0);

        // Unbind VAO
        glBindVertexArray(0);
    }

    // Helper to get the number of triangles for solid rendering
    GLsizei get_solid_triangle_count() const
    {
        // Assumes each triangle has 3 vertices, and the EBO has size corresponding to this
        return solid_EBO_size / sizeof(uint16_t) / 3;  // Adjust `solid_EBO_size` accordingly
    }

    // Helper to get the number of edges for wireframe rendering
    GLsizei get_wireframe_edge_count() const
    {
        // Assumes each edge has 2 vertices, and the EBO has size corresponding to this
        return wireframe_EBO_size / sizeof(uint16_t) / 2;  // Adjust `wireframe_EBO_size` accordingly
    }

    // Destroy resources
    void destroy()
    {
        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
        }
        if (VBO != 0) {
            glDeleteBuffers(1, &VBO);
        }
        if (solid_EBO != 0) {
            glDeleteBuffers(1, &solid_EBO);
        }
        if (wireframe_EBO != 0) {
            glDeleteBuffers(1, &wireframe_EBO);
        }
    }

    // Size of the EBOs (used to calculate draw calls)
    GLsizei solid_EBO_size = 0;
    GLsizei wireframe_EBO_size = 0;
};



struct TriangleData {
    glm::vec3 vertices[3]; // Vertices of the triangle
    glm::vec3 normal;      // Normal of the triangle
    uint16_t materialID;   // Material ID of the triangle

    // Constructor to initialize triangle data
    TriangleData(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
        const glm::vec3& normal, uint16_t materialID)
        : normal(normal), materialID(materialID) {
        vertices[0] = v0;
        vertices[1] = v1;
        vertices[2] = v2;
    }

    TriangleData(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,, uint16_t materialID)
        : materialID(materialID) {
        vertices[0] = v0;
        vertices[1] = v1;
        vertices[2] = v2;
        normal = get_normal(a, b, c);
    }
};

typedef struct brush
{   
public:
    enum class EditMode {
        None,
        Vertex,
        Triangle,
        Wireframe,
        Material
    };



    // Toggles wireframe mode on and off
    static bool r_wireframe;// = false;
    static Texture<GL_TEXTURE_2D, GL_RGB, GL_RGB, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR> texture;
    static Shader shader;
    // Default constructor (doesn't initialize OpenGL state)
    brush() : edit_mode(EditMode::None)
    {
        // Initialize other members if needed, but don't initialize OpenGL state
    }

    // Copy function to create a copy of this brush
    brush copy() const {
        brush new_brush; // Create a new brush object

        // Copy member variables
        new_brush.points = this->points;
        new_brush.triangles = this->triangles;

        // Create a new model for the copied brush
        new_brush.model = new_brush.make_model(); // Generates buffers for triangles

        return new_brush; // Return the newly created brush
    }

    // Constructor to create a cuboid given start and stop points
    // Constructor to create a cuboid given start and stop points
    brush(const glm::vec3& start, const glm::vec3& stop)
    {
        // Define the corners of the cuboid
        glm::vec3 front_bottom_left = start;
        glm::vec3 front_bottom_right = { stop.x, start.y, start.z };
        glm::vec3 front_top_left = { start.x, stop.y, start.z };
        glm::vec3 front_top_right = { stop.x, stop.y, start.z };
        glm::vec3 back_bottom_left = { start.x, start.y, stop.z };
        glm::vec3 back_bottom_right = { stop.x, start.y, stop.z };
        glm::vec3 back_top_left = { start.x, stop.y, stop.z };
        glm::vec3 back_top_right = { stop.x, stop.y, stop.z };

        // Vector to hold points in the point cloud
        std::vector<glm::vec3> points = {
            front_bottom_left,
            front_bottom_right,
            front_top_left,
            front_top_right,
            back_bottom_left,
            back_bottom_right,
            back_top_left,
            back_top_right
        };

        // Define the triangles with indices into the point cloud
        std::vector<brush_triangle_reference> cuboid_triangles = {
            // Front face
            {0, 1, 3}, {0, 3, 2},
            // Back face
            {4, 5, 7}, {4, 7, 6},
            // Left face
            {0, 4, 6}, {0, 6, 2},
            // Right face
            {1, 3, 7}, {1, 7, 5},
            // Top face
            {2, 3, 7}, {2, 7, 6},
            // Bottom face
            {0, 1, 5}, {0, 5, 4}
        };

        // Initialize triangles with the cuboid triangles
        triangles.reserve(cuboid_triangles.size());
        for (const auto& triangle_ref : cuboid_triangles) {
            triangles.push_back(triangle_ref);
        }

        // Store the points in the brush instance
        this->points = std::move(points);

        // Create the model using the triangles
        model = make_model();
    }


    // Constructor to initialize brush with a vector of triangles
    brush(const std::vector<brush_triangle_reference>& input_triangles, point_cloud points)
        : triangles(input_triangles), points(points) // Initialize triangles with input
    {
        // Create the model using the provided triangles
        model = make_model();
    }


    // Move constructor
    brush(brush&& other) noexcept
        : 
        model(std::move(other.model)),
        points(std::move(other.points)),
        triangles(std::move(other.triangles))
    {

    }

    // Deleted copy constructor
    brush(const brush&) 
    {
    }// = delete;

    // Cleans up OpenGL state information
    ~brush() {
        // Cleanup OpenGL resources
        glDeleteVertexArrays(1, &model.VAO);
        glDeleteBuffers(1, &model.triangle_EBO);
        glDeleteBuffers(1, &model.wireframe_EBO);
    }



    // Draw function to render the model based on r_wireframe
    void draw()
    {
        glBindVertexArray(model.VAO);  // Bind the VAO of the model

        if (r_wireframe)
        {
            // If wireframe mode is enabled, bind the wireframe EBO
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.wireframe_EBO);
            // Set polygon mode to line to draw wireframe
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            // Draw elements (wireframe edges)
            glDrawElements(GL_LINES, unique_edges_count * 2, GL_UNSIGNED_SHORT, 0);
        }
        else
        {
            // If wireframe mode is disabled, bind the triangle EBO
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.triangle_EBO);
            // Set polygon mode back to fill (normal rendering)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            // Draw elements (triangles)
            glDrawElements(GL_TRIANGLES, triangles.size() * 3, GL_UNSIGNED_SHORT, 0);
        }

        glBindVertexArray(0); // Unbind VAO after drawing
    }
    // Start editing vertex VBO
    float* start_geometry_edit()
    {
        if (edit_mode != EditMode::None)
        {
            glBindBuffer(GL_ARRAY_BUFFER, model.vertex_VBO);

            float* mapped_buffer = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
            if (mapped_buffer)
            {
                edit_mode = EditMode::Vertex;
                return mapped_buffer;
            }
        }
        return nullptr;
    }

    // Stop editing vertex VBO
    void stop_geometry_edit()
    {
        if (edit_mode == EditMode::Vertex)
        {
            glBindBuffer(GL_ARRAY_BUFFER, model.vertex_VBO);
            glUnmapBuffer(GL_ARRAY_BUFFER);
            edit_mode = EditMode::None;
        }
    }

    // Start editing triangle index EBO
    uint16_t* start_triangle_index_edit()
    {
        if (edit_mode == EditMode::None)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.triangle_EBO);

            uint16_t* mapped_buffer = (uint16_t*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
            if (mapped_buffer)
            {
                edit_mode == EditMode::Triangle;
                return mapped_buffer;
            }
            else
            {
                std::cerr << "Failed to open triangle EBO for edit.";
                return nullptr;
            }
        }
        return nullptr;
    }

    // Stop editing triangle index EBO
    void stop_triangle_index_edit()
    {
        if (edit_mode== EditMode::Triangle)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.triangle_EBO);
            glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
            edit_mode = EditMode::None;
        }
    }

    // Start editing wireframe index EBO
    uint16_t* start_wireframe_index_edit()
    {
        if (edit_mode == EditMode::None)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.wireframe_EBO);

            uint16_t* mapped_buffer = (uint16_t*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
            if (mapped_buffer)
            {
                edit_mode = EditMode::Wireframe;
                return mapped_buffer;
            }
            else
            {
                std::cerr << "Failed to open wireframe EBO for edit.";
                return nullptr;
            }
        }
        return nullptr;
    }

    // Stop editing wireframe index EBO
    void stop_wireframe_index_edit()
    {
        if (edit_mode ==EditMode::Wireframe)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.wireframe_EBO);
            glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
            edit_mode = EditMode::None;
        }
    }

    // Start editing material VBO
    uint16_t* start_material_edit()
    {
        if (edit_mode == EditMode::None)
        {
            glBindBuffer(GL_ARRAY_BUFFER, model.material_VBO);

            uint16_t* mapped_buffer = (uint16_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
            if (mapped_buffer)
            {
                edit_mode = EditMode::Material;
                return mapped_buffer;
            }
            else
            {
                std::cerr << "Failed to open material VBO for edit.";
                return nullptr;
            }
        }
        return nullptr;
    }

    // Stop editing material VBO
    void stop_material_edit()
    {
        if (edit_mode ==EditMode::Material)
        {
            glBindBuffer(GL_ARRAY_BUFFER, model.material_VBO);
            glUnmapBuffer(GL_ARRAY_BUFFER);
            edit_mode == EditMode::None;
        }
    }
    bool is_editing() const {
        return edit_mode != EditMode::None;
    }



    void clear() { triangles.clear(); points.clear(); unique_edges_count = 0; }
    void free_model()
    {
        model.destroy();
    }
    private:

        EditMode edit_mode;
        

        brush_model model;
        point_cloud points;
        std::vector<brush_triangle_reference> triangles;
        GLsizei unique_edges_count = 0;
        brush_triangle get_triangle(brush_triangle_reference reference) {    return (points[reference.point_indices[0]], points[reference.point_indices[1]], points[reference.point_indices[2]]);    }
        brush_triangle get_triangle(uint16_t p1_idx, uint16_t p2_idx, uint16_t p3_idx)  {    return (points[p1_idx], points[p2_idx], points[p3_idx]);   }
        brush_triangle get_triangle(size_t brush_triangle_index)    {    return get_triangle(triangles[brush_triangle_index]);  }






};

Shader brush::shader;
Texture<GL_TEXTURE_2D, GL_RGB, GL_RGB, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR> brush::texture;
bool brush::r_wireframe = false;

brush_model make_brush_model(TriangleData* triangle_data, size_t triangle_count)
{
    // Create a new brush model
    brush_model B;

    // Step 1: Create unique points, normals, and material IDs
    std::unordered_map<glm::vec3, uint16_t, glm::vec3> unique_points_map;
    std::unordered_map<glm::vec3, uint16_t, glm::vec3> unique_normals_map;
    point_cloud unique_points;
    point_cloud unique_normals;

    // Step 2: Prepare triangle indices
    std::vector<brush_triangle_reference> triangle_indices;

    for (size_t i = 0; i < triangle_count; ++i) {
        const TriangleData& data = triangle_data[i];
        uint16_t new_indices[3];

        for (int j = 0; j < 3; ++j) {
            glm::vec3 point = data.positions[j];
            if (unique_points_map.find(point) == unique_points_map.end()) {
                // New unique point
                unique_points_map[point] = unique_points.size();
                unique_points.push_back(point);
            }
            new_indices[j] = unique_points_map[point];

            glm::vec3 normal = data.normals[j];
            if (unique_normals_map.find(normal) == unique_normals_map.end()) {
                // New unique normal
                unique_normals_map[normal] = unique_normals.size();
                unique_normals.push_back(normal);
            }
        }

        triangle_indices.emplace_back(new_indices[0], new_indices[1], new_indices[2], data.material_ID);
    }

    // Step 3: Remove duplicate triangle references
    std::set<std::array<uint16_t, 3>> unique_triangles;
    std::vector<brush_triangle_reference> dedup_triangles;
    for (auto& tri_ref : triangle_indices) {
        std::array<uint16_t, 3> sorted_indices = { tri_ref.point_indices[0], tri_ref.point_indices[1], tri_ref.point_indices[2] };
        std::sort(sorted_indices.begin(), sorted_indices.end());
        if (unique_triangles.find(sorted_indices) == unique_triangles.end()) {
            unique_triangles.insert(sorted_indices);
            dedup_triangles.push_back(tri_ref);
        }
    }
    triangle_indices = std::move(dedup_triangles);

    // Step 4: Using these data, generate the VAO, VBO, and EBOs
    glGenVertexArrays(1, &B.VAO);
    glBindVertexArray(B.VAO);

    // Create and populate the VBO for positions
    glGenBuffers(1, &B.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, B.VBO);
    glBufferData(GL_ARRAY_BUFFER, unique_points.size() * sizeof(glm::vec3), unique_points.data(), GL_STATIC_DRAW);

    // Enable vertex attribute for positions
    glEnableVertexAttribArray(0); // Attribute 0 for positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0); // Positions (3 floats)

    // Create and populate the VBO for normals
    glGenBuffers(1, &B.normal_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, B.normal_VBO);
    glBufferData(GL_ARRAY_BUFFER, unique_normals.size() * sizeof(glm::vec3), unique_normals.data(), GL_STATIC_DRAW);

    // Enable vertex attribute for normals
    glEnableVertexAttribArray(1); // Attribute 1 for normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0); // Normals (3 floats)

    // Create and populate the VBO for material IDs
    std::vector<uint16_t> material_ids;
    for (const auto& tri_ref : triangle_indices) {
        material_ids.push_back(tri_ref.material_ID);
    }

    glGenBuffers(1, &B.material_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, B.material_VBO);
    glBufferData(GL_ARRAY_BUFFER, material_ids.size() * sizeof(uint16_t), material_ids.data(), GL_STATIC_DRAW);

    // Enable vertex attribute for material IDs
    glEnableVertexAttribArray(2); // Attribute 2 for material IDs
    glVertexAttribPointer(2, 1, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(uint16_t), (void*)0); // Material IDs

    // Generate the solid EBO (for solid rendering)
    std::vector<uint16_t> solid_indices;
    for (const auto& tri_ref : triangle_indices) {
        solid_indices.push_back(tri_ref.point_indices[0]);
        solid_indices.push_back(tri_ref.point_indices[1]);
        solid_indices.push_back(tri_ref.point_indices[2]);
    }
    glGenBuffers(1, &B.solid_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, B.solid_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, solid_indices.size() * sizeof(uint16_t), solid_indices.data(), GL_STATIC_DRAW);

    // Generate the wireframe EBO (for wireframe rendering)
    std::set<std::pair<uint16_t, uint16_t>> unique_edges;
    for (const auto& tri_ref : triangle_indices) {
        std::array<uint16_t, 3> indices = { tri_ref.point_indices[0], tri_ref.point_indices[1], tri_ref.point_indices[2] };
        for (int j = 0; j < 3; ++j) {
            uint16_t p1 = indices[j];
            uint16_t p2 = indices[(j + 1) % 3];  // Wrap around to form a triangle
            if (p1 > p2) std::swap(p1, p2);      // Order the pair to avoid duplicates
            unique_edges.insert({ p1, p2 });
        }
    }

    // Generate wireframe EBO
    std::vector<uint16_t> wireframe_indices;
    for (const auto& edge : unique_edges) {
        wireframe_indices.push_back(edge.first);
        wireframe_indices.push_back(edge.second);
    }
    glGenBuffers(1, &B.wireframe_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, B.wireframe_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, wireframe_indices.size() * sizeof(uint16_t), wireframe_indices.data(), GL_STATIC_DRAW);

    // Unbind the VAO
    glBindVertexArray(0);

    return B;  // Return the generated brush model
}



void brush_setup()
{
    brush::shader.initFromFiles("resources/shaders/brush_vertex.glsl", "resources/shaders/brush_frag.glsl");
    brush::shader.use();
    //GLfloat maxAniso;
    //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
    brush::texture.loadFromFile("resources/images/atlas.png");

}