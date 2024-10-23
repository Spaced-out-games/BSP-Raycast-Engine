#pragma once
#include "ent_camera.h"
#include "ent_controller.h"
#include "level_editor_controller.h"

struct Client
{
    level_editor_controller controller;
    ent_camera* camera = nullptr;

    // Default constructor
    Client()
    {
        camera = new ent_camera();
        controller.mControlledEntity = camera;
    }

    // Implement a custom copy constructor if necessary
    Client(const Client& other)
    {
        // If controller is copyable
        controller = other.controller;

        // Deep copy the camera if needed (assuming it's a raw pointer)
        if (other.camera) {
            camera = new ent_camera(*other.camera);
        }
        else {
            camera = nullptr;
        }

        // Also update controller's controlled entity
        controller.mControlledEntity = camera;
    }

    // Implement a move constructor to allow transfer of ownership
    Client(Client&& other) noexcept
        : controller(std::move(other.controller)), camera(other.camera)
    {
        other.camera = nullptr; // Nullify the other camera pointer to prevent double deletion
        controller.mControlledEntity = camera;
    }

    // Implement a move assignment operator
    Client& operator=(Client&& other) noexcept
    {
        if (this != &other) {
            // Transfer ownership of the camera
            delete camera; // Delete current camera if necessary
            camera = other.camera;
            other.camera = nullptr;

            // Move controller
            controller = std::move(other.controller);

            // Update the controlled entity
            controller.mControlledEntity = camera;
        }
        return *this;
    }

    // Clean up camera resource
    ~Client()
    {
        delete camera;
    }

    const char* get_name() { return "Client"; }
};