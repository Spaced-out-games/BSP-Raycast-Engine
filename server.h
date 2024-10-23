#pragma once
#include "ent_container.h"

class Client;

#define test_client_t Client<level_editor_app, level_editor_controller, ent_camera>

template <typename T, typename = void>
struct has_get_name : std::false_type {};

template <typename T>
struct has_get_name<T, std::void_t<decltype(std::declval<T>().get_name())>> : std::true_type {};


template <typename ...ent_types>
struct Server
{
    std::vector<Client> clients;
	std::tuple<ent_container<ent_types>...> entity_containers;
    std::unordered_map<std::string, std::size_t> entity_name_map;
    // Access entity container by index
    template <std::size_t Index>
    auto& get_entity_container() {
        return std::get<Index>(entity_containers);
    }

    // Access entity container by type
    template <typename T>
    auto& get_entity_container() {
        return std::get<ent_container<T>>(entity_containers);
    }

    // Access entity container by entity name
    auto& get_entity_container_by_name(const std::string& entity_name) {
        std::size_t index = entity_name_map[entity_name];
        return get_entity_container_by_index(index);
    }

    // Helper method to get container by index
    auto& get_entity_container_by_index(std::size_t index) {
        return std::apply([index](auto&... containers) -> auto& {
            return std::get<index>(std::forward_as_tuple(containers...));
        }, entity_containers);
    }


	Server() {
		// Populate the entity name map with indices using get_name()
		add_entity_type_names<ent_types...>();
	}
	//private:
		template <typename First, typename... Rest>
        void add_entity_type_names() {
            // Check if First has get_name
            if constexpr (has_get_name<First>::value) {
                // Create a temporary instance of the entity to call get_name()
                First temp_entity;
                entity_name_map[temp_entity.get_name()] = sizeof...(Rest); // Adjusted index logic
            }
            else {
                // Fallback to using typeid
                //entity_name_map["Client"] = sizeof...(Rest);
            }

            // Continue adding the rest of the types
            if constexpr (sizeof...(Rest) > 0) {
                add_entity_type_names<Rest...>();
            }
        }

};


