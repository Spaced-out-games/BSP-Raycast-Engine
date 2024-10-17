#pragma once
#include <stdint.h>
#include <algorithm>
#include <array>
#include <type_traits>
#include <vector>
#include <any>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>
#include <memory> // for std::unique_ptr

// Function template to get the maximum size from a parameter pack

template <typename... Ts>
constexpr size_t GET_MAX_SIZE() {
    return (std::max)({ sizeof(Ts)... });
}

// Structure for entity reference
struct ent_reference {
    std::type_index type_ID;
    uint16_t instance_index = 0;

    // Constructor with template type deduction (default to int)
    template <typename T = int>
    ent_reference(uint16_t index)
        : type_ID(typeid(T)), instance_index(index) {}  // Use initializer list for type_ID

    // Default constructor with void type
    ent_reference()
        : type_ID(typeid(void)), instance_index(0) {}  // Default to typeid(void)
};


using small_size_t = uint16_t;

// Command structure
struct ent_command {
    // Constructor initializes the stream with command data
    ent_command(uint16_t command_ID = 0, small_size_t size = 0, uint8_t* ptr = nullptr)
        : command_ID(command_ID), buffer_size(size) {
        if (ptr == nullptr) {
            data = new uint8_t[size]; // Allocate memory if no pointer is provided
        }
        else {
            data = ptr; // Use the provided pointer
        }
    }

    // No need for a destructor when using std::unique_ptr

    // Given an offset, read the next T in the buffer.
    template <typename T>
    T read_next(small_size_t& offset) {
        if (offset + sizeof(T) > buffer_size) {
            throw std::out_of_range("Attempted to read past the end of command data.");
        }

        T out = *reinterpret_cast<T*>(data + offset);
        offset += sizeof(T);
        return out;
    }

    // Given an offset, write a T into the buffer
    template <typename T>
    void write_next(const T& value, small_size_t& offset) {
        if (offset + sizeof(T) > buffer_size) {
            throw std::out_of_range("Attempted to write past the end of command data.");
        }

        *reinterpret_cast<T*>(data + offset) = value;
        offset += sizeof(T);
    }

    // Return the size of the buffer
    small_size_t size() const {
        return buffer_size;
    }

    uint16_t command_code() { return command_ID; }


    uint16_t command_ID;
    small_size_t buffer_size = 0;
    uint8_t* data;
    //std::unique_ptr<uint8_t[]> data; // Use smart pointer for better memory management
};

typedef struct sv_command
{
    ent_reference reference;
    ent_command command;

};

using ent_request = ent_command;
using ent_response = ent_command;

class ent {
public:
    ent() = default;
    virtual ~ent() = default;

    virtual void draw() {};
    virtual void tick() {};
    virtual void init() {};

    enum : uint16_t {
        NULL_OPERATION = 0,
        INIT = 1,
        TICK = 2,
        DRAW = 3
    };

    virtual void exec(ent_request stream) {
        switch (stream.command_ID) {
        case NULL_OPERATION:
            break; // No-op packet
        case INIT:
            init();
            break;
        case TICK:
            tick();
            break;
        case DRAW:
            draw();
            break;
        default:
            break;
        }
    }
};


