#pragma once
#include <GL/glew.h>
#include "brush.h"
#include "mesh_types.h"
#include <list>

// Forward declaration of level_editor_controller
class level_editor_controller;

struct windowContent {
    std::vector<ent_brush> brushes;
    float t = 0.0f;
    tex_dynamic_mesh_3D cube;
    level_editor_controller* camController = nullptr;  // Pointer to level_editor_controller
    glm::vec3 last_cam_pos;
    glm::vec3 delta_cam_pos;
    glm::vec3 hit_location;
    int selected_brush_index = 0;
    int selected_vertex_index = 0;
    //Brush B;
    bool input_to_imgui = false;

    ~windowContent();  // Destructor to clean up the pointer
    void init();
};
#include "windowContent.h"
#include "level_editor_controller.h"

windowContent::~windowContent() {
    delete camController;  // Clean up the dynamically allocated controller

    // Destroy all the Brush meshes
    for (size_t i = 0; i < brushes.size(); i++)
    {
        brushes[i].~ent_brush();
    }
}

void windowContent::init() {
    camController = new level_editor_controller();  // Dynamically allocate controller
    // Initialize the controller here
}