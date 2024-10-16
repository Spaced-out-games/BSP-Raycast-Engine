#include <stdint.h>
#include <algorithm> // for std::max
#include <array>     // for std::array
#include <type_traits> // for type safety

// Function template to get the maximum size from a parameter pack
template <typename... Ts>
constexpr size_t GET_MAX_SIZE() {
    return (std::max)({ sizeof(Ts)... });
}

#include "Camera.h"
#include "level_editor_controller.h"
#include "Controller.h" // Assuming you have this file


// Define the ent_command structure
template <size_t size>
struct ent_command {
    uint16_t command_index = 0; // 0 indicates no-op
    std::array<uint8_t, size> command; // Command data
};
// Structure for entity reference
struct ent_reference {
    uint16_t type_ID;       // Type ID of the entity
    uint16_t instance_index; // Instance index to refer to a specific instance
};


// The engine will automatically call ent::tick(), so if you implement draw() and want it drawn every frame, include draw() in your tick() function
class ent
{
    virtual ent() = default;
    virtual ~ent() = defualt;



    virtual void draw() = 0;

    virtual void tick() = 0;

    virtual void init() = 0;

    // Whenever you define custom behavior, you have to pass 
    template <size_t size>
    virtual void exec(ent_command<size> packet)
    {
        switch (packet.command_index)
        {
        case 0:
            init();
            break;
        case 1:
            tick();
            break;
        case 2:
            draw();
            break;
        default:
            break;
        }
    }
};


class ent_camera : public Camera
{
    template <size_t size>
    virtual void exec(ent_command<size> packet) override
    {

    }
};