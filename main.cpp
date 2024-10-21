#pragma once
#define SDL_MAIN_HANDLED
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include <GL/glew.h>
#include <iostream>
#include "Application.h"

#include <math.h>
#include "globals.h"

#include "level_editor.h"




// Texture and other resources should probably have their constructors deferred to a resource_manager that returns a pointer to the type, in which case, entities bind before the next draw call.
// This can build off of the planned ent_manager / server system that is on the horizon...






int main(int argc, char* argv[])
{


    //cube_demo app(argc, argv); // Initializes SDL window & context
    level_editor_app app(argc, argv);
    // Initialize windowContent with the Application instance
    //windowContent content;




    // Run the main loop
    app.run();

    return 0;
}
//
