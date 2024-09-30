#pragma once
#define SDL_MAIN_HANDLED
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include <GL/glew.h>
#include <iostream>
#include "Application.h"

#include <math.h>
//#include "cube_demo.h"

#include "level_editor.h"











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
