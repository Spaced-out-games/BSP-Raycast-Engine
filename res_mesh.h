#pragma once
#include <type_traits>
#include "resource.h"
#include <GL/glew.h>
#include "glm_master.h"
/*
#define tuple_struct(struct_name, ...)          \
    struct struct_name {                        \
        __VA_ARGS__;                            \
    };                                          \
    constexpr auto as_tuple(const struct_name& s) { \
        return std::make_tuple(s.__VA_ARGS__); \
    }                                           \
    constexpr struct_name from_tuple(const auto& t) { \
        struct_name s;                          \
        std::apply([&s](auto&&... args) {      \
            ((s.__VA_ARGS__ = args), ...);     \
        }, t);                                  \
        return s;                               \
    }



tuple_struct(MyStruct,
    int x;
float y;
std::string z;
)

*/


// -----------------------------------------------------------------------------------------------


template<typename T>
struct has_data : std::integral_constant<bool, !std::is_same<T, void>::value> {};

//template <typename vertex_t = glm::vec3, typename face_t = void>
struct res_mesh : public Resource {
    GLenum draw_mode = GL_STATIC_DRAW;
    GLuint VAO = 0;

    // Conditional members based on vertex_t and face_t
    GLuint VBO = 0;
    GLuint EBO = 0;
    GLuint SSBO = 0; // SSBO for per-face attributes
    size_t index_count = 0;

    // Constructor
    template <typename vertex_t = glm::vec3, typename face_t = void>
    res_mesh(const vertex_t* vertices, const size_t num_vertices,
        const GLuint* indices, const size_t num_indices,
        const face_t* faces, const size_t num_faces) {
        init(vertices, num_vertices, indices, num_indices, faces, num_faces);
    }

    // No-op constructor
    res_mesh() {}

    // Destructor
    ~res_mesh();
    template <typename vertex_t = glm::vec3, typename face_t = void>
    void init(const vertex_t* vertices, const size_t num_vertices,
        const GLuint* indices, const size_t num_indices,
        const face_t* faces, const size_t num_faces);

    void use() override { bind(); }

    void bind();
    void unbind();

    // Automatically set up attributes for the vertex structure
    template <typename vertex_t = glm::vec3, typename face_t = void>
    void setupAttributes();


    virtual void draw(GLenum primitive_t = GL_TRIANGLES);
};

// Destructor implementation

res_mesh::~res_mesh() {
    if (VAO > 0) {
        glDeleteVertexArrays(1, &VAO);
    }
    
    if (VBO > 0) {
        glDeleteBuffers(1, &VBO);
    }

    if (SSBO > 0) {
        glDeleteBuffers(1, &SSBO);
    }

    if (EBO > 0) {
        glDeleteBuffers(1, &EBO);
    }
}

// Initialization method
template <typename vertex_t = glm::vec3, typename face_t = void>
void res_mesh::init(const vertex_t* vertices, const size_t num_vertices,
    const GLuint* indices, const size_t num_indices,
    const face_t* faces, const size_t num_faces) {
    index_count = num_indices;

    // Initialize the vertex array object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Initialize the VBO if vertex_t is not void
    if constexpr (has_data<vertex_t>::value) {
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(vertex_t), vertices, draw_mode);
        setupAttributes(); // Set up attributes after the VBO is created
    }

    // Generate the EBO if there are indices
    if constexpr (has_data<vertex_t>::value && num_indices > 0) {
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(GLuint), indices, draw_mode);
    }

    // Initialize the SSBO for per-face attributes if face_t is not void
    if constexpr (has_data<face_t>::value) {
        glGenBuffers(1, &SSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, num_faces * sizeof(face_t), faces, draw_mode);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SSBO); // Binding to index 1
    }

    unbind();
}

// Setup vertex attributes based on the vertex structure
template <typename vertex_t = glm::vec3, typename face_t = void>
void res_mesh::setupAttributes() {
    // Example: assuming vertex_t is a struct with attributes `position` (vec3) and `color` (vec3)
    constexpr std::size_t num_members = std::tuple_size_v<vertex_t>;
    configureAttributesForStruct(*this, std::make_index_sequence<num_members>{});
}

// Binding methods

void res_mesh::bind() {
    glBindVertexArray(VAO);
}


void res_mesh::unbind() {
    glBindVertexArray(0);
}

// Draw method
void res_mesh::draw(GLenum primitive_t) {
    glDrawElements(primitive_t, index_count, GL_UNSIGNED_INT, 0);
}