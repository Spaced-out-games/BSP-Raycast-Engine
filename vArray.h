#pragma once
#include <GL/glew.h>
#include <iostream>

#include "debug_utils.h"


class vArray {
public:
    /**
    test
    */
    vArray() : ID(0) {
        
        #ifdef _DEBUG
        std::cout << "Generated vArray with ID: " << ID << std::endl;
        #endif
    }
    
    void init()
    {
        glGenVertexArrays(1, &ID);
    }

    ~vArray() {
        
    }
    void destroy()
    {
        if (ID != 0) {
            //glDeleteVertexArrays(1, &ID);
            #ifdef _DEBUG
            std::cout << "Deleted vArray with ID: " << ID << std::endl;
            #endif
        }
    }

    void bind() const {
        glBindVertexArray(ID);
        #ifdef _DEBUG
        //std::cout << "Bound vArray with ID: " << ID << std::endl;
        #endif
        CHECK_GL_ERROR();
    }

    void unbind() const {
        glBindVertexArray(0);
        #ifdef _DEBUG
        //std::cout << "Unbound vArray with ID: " << ID << std::endl;
        #endif
    }

    bool isBound() const {
        GLint currentVAO = 0;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO); // Problem seems to be here
        return (static_cast<GLuint>(currentVAO) == ID);
    }

    GLuint getID() const { return ID; }

private:
    GLuint ID;
};