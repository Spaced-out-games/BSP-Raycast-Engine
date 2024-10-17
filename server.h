#pragma once
#include <iostream>
#include <tuple>
#include <typeindex>
#include <vector>
#include <unordered_map>
#include <string>
#include <type_traits>
#include <any>
#include <stdexcept>
//#include "server_definition.h"
//#include "client.h"

// Entity container class for managing entity lists and references
// Entity container class for managing entity lists and references
template<typename T>
class ent_container {
public:
    // Add an entity by constructing it in place
    template<typename... Args>
    void add(Args&&... args) {
        items.emplace_back(std::forward<Args>(args)...); // Construct in place with perfect forwarding
    }

    // Retrieve an entity by index
    T& get(size_t index) {
        if (index >= items.size()) {
            throw std::out_of_range("Index out of bounds");
        }
        return items[index];
    }

    // Get the number of entities in the container
    size_t size() const {
        return items.size();
    }

    // Call tick() on all entities that have a tick() method
    void tick() {
        for (auto& item : items) {
            call_tick(item);
        }
    }

    // Call draw() on all entities that have a draw() method
    void draw() {
        for (auto& item : items) {
            call_draw(item);
        }
    }

    // Call init() on all entities that have an init() method
    void init() {
        for (auto& item : items) {
            call_init(item);
        }
    }

private:
    std::vector<T> items; // Store entities directly

    // Helper to check if T has a tick() method
    template<typename U>
    static auto has_tick(int) -> decltype(std::declval<U>().tick(), std::true_type()) {
        return std::true_type();
    }

    template<typename>
    static std::false_type has_tick(...) {
        return std::false_type();
    }

    // Helper to check if T has a draw() method
    template<typename U>
    static auto has_draw(int) -> decltype(std::declval<U>().draw(), std::true_type()) {
        return std::true_type();
    }

    template<typename>
    static std::false_type has_draw(...) {
        return std::false_type();
    }

    // Helper to check if T has an init() method
    template<typename U>
    static auto has_init(int) -> decltype(std::declval<U>().init(), std::true_type()) {
        return std::true_type();
    }

    template<typename>
    static std::false_type has_init(...) {
        return std::false_type();
    }

    // Call tick on the entity if it has a tick method
    void call_tick(T& item) {
        if constexpr (decltype(has_tick<T>(0))::value) {
            item.tick(); // Call tick if it exists
        }
        else {
            std::cerr << "Warning: Entity does not have a tick method.\n";
        }
    }

    // Call draw on the entity if it has a draw method
    void call_draw(T& item) {
        if constexpr (decltype(has_draw<T>(0))::value) {
            item.draw(); // Call draw if it exists
        }
        else {
            std::cerr << "Warning: Entity does not have a draw method.\n";
        }
    }

    // Call init on the entity if it has an init method
    void call_init(T& item) {
        if constexpr (decltype(has_init<T>(0))::value) {
            item.init(); // Call init if it exists
        }
        else {
            std::cerr << "Warning: Entity does not have an init method.\n";
        }
    }
};





















// Server class to manage different types of entities
template<typename header_t = char, typename... Ts>
class Server {
public:
    float dt;
    float t;
    header_t custom_state;

    // Singleton instance retrieval
    static Server& instance() {
        static Server instance; // Static instance created once, thread-safe in C++11 and later
        return instance;
    }

    Server(const Server&) = delete; // Prevent copy construction
    Server& operator=(const Server&) = delete; // Prevent copy assignment

    // Method to add an entity and return an ent_reference
    template<typename T, typename... Args>
    ent_reference add_entity(Args&&... args) {
        auto& container = get_container<T>();
        container.add(std::forward<Args>(args)...);

        // Create an ent_reference with the type and the index of the newly added entity
        return ent_reference{ static_cast<uint16_t>(container.size() - 1) }; // Size - 1 for zero-based index
    }

    // Retrieve the container for a specific type T
    template<typename T>
    ent_container<T>& get_container() {
        return std::get<ent_container<T>>(entity_containers);
    }

    // Retrieve the container based on the entity reference
    template<typename T>
    T& get(const ent_reference& reference) {
        auto& container = get_container<T>();
        return container.get(reference.instance_index); // Return the entity of type T
    }

    // Run a command on a specific entity based on the given sv_command
    void run_command(const sv_command& cmd) {
        // Extract the type and run the command
        // For now, we assume the command applies to one of the entity types in the tuple
        // You'll need to add proper handling based on command code
        std::cerr << "Command not implemented yet!\n";
    }

private:
    Server() = default; // Private constructor to enforce singleton

    // Using std::tuple to store ent_containers for each type
    std::tuple<ent_container<Ts>...> entity_containers;
};


//#include "server_definition.h"

static server_t& server = server_t::instance();