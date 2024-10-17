#pragma once
#include "server_definition.h"
#include "Camera.h"
#include "ent_controller.h"
#include "Application.h"

extern server_t& server;

// how about we make a client simply an Application, Controller, Camera, and clientinfo?

typedef struct clientinfo
{
    client player;
    float t;
    float dt;
    bool input_to_imgui;
};


struct client : public ent_camera, public ent_controller {
    void init() {};
    void draw() {};
    void tick() {};

    ControllerState<> controller_state;

    client() : ent_camera(), ent_controller(&controller_state) {
        keyboard_ptr = SDL_GetKeyboardState(nullptr);
    }

protected:
    const Uint8* keyboard_ptr = nullptr;
};
