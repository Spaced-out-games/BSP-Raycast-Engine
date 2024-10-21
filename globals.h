#pragma once
#include "glm_master.h"
#include <GL/glew.h>


struct Globals {
    float t; // Time
    float dt; // Delta time
    vec2 window_dimensions; // Window dimensions

    // Private constructor to prevent instantiation
    Globals() : t(0.0f), dt(0.0f), window_dimensions(1920.0f, 1080.0f) {}


    uint8_t r_rendermode;

    // Updates game state, ege, updating the render mode per r_rendermode
    void tick() {};

    void set_render_mode(uint8_t mode)
    {
        r_rendermode = mode;

        switch (r_rendermode)
        {
        case 0: // Fill mode
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case 1: // Wireframe mode
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        default: // Default behavior (could be fill or no change)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        }
        //ent_brush::shader.setUniform("glPrimitiveType", mode);
    }
    void init()
    {
        set_render_mode(0);
    }

};

struct gl_globals {
    mat4 view;
    mat4 projection;
    int r_rendermode = 0;
    // total Time elapsed
    float t;
    // deltaTime
    float dt;
    // Gravity
    float g;
};

static Globals globals;