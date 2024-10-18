#pragma once
#include "server_definition.h"
#include "Camera.h"
#include "ent_controller.h"
#include "Application.h"
//FYI, steal from level_editor for this

extern server_t& server;

// how about we make a Client simply an Application, Controller, Camera, and Clientinfo?

typedef struct brush_editor_Client
{
    int selected_brush_index = 0;
    int selected_vertex_index = 0;
};




template <class state_t = brush_editor_Client>
struct Client: public ent {
public:

    
    // automatically refer to the server's deltaTime. Would be better to discern Client-side dt and server-side dt
    ent_camera* camera = &(server.add_entity<ent_camera>());
    ent_controller* controller = &(server.add_entity<ent_controller>());
    float& t = server.get_time();
    float& dt = server.get_delta_time();

    uint8_t input_mode; // Used to be input_to_imgui

    bool clip = false;
    glm::vec2 window_dimensions;

    void init() override
    {
        // Create the camera and controller, and link them

        // Let the controller know who to control. Needs generalized better. Maybe add a ent_movable, that is just an ent + glm::mat4, then  abstract camera from that? Add virtual methods like getPos, setPos, move...basically all the functions of Camera except streeamlined
        //controller->camera = *camera;
        controller->init();


    };
    void draw() override {};
    void tick() override {};



    Client() : camera(), controller() {
        keyboard_ptr = SDL_GetKeyboardState(nullptr);
    }

protected:
    const Uint8* keyboard_ptr = nullptr;
};
