#pragma once
#include <typeinfo>
#include <iostream>
#include <string>


class Resource {
public:
    Resource() = default;                        // Default constructor
    virtual ~Resource() = default;               // Virtual destructor

    // Delete copy constructor and copy assignment operator
    Resource(const Resource&) = delete;          // Disable copying
    Resource& operator=(const Resource&) = delete; // Disable copy assignment

    // Default move constructor and move assignment operator
    Resource(Resource&&) = default;              // Enable moving
    Resource& operator=(Resource&&) = default;   // Enable move assignment

    // Pure virtual functions to be overridden in derived classes
    virtual void load(const std::string& path) {};
    virtual void save(const std::string& path) {};
    virtual void use() {};                       // Pure virtual function for using the resource

    // Copy function to be overridden in derived classes
    virtual Resource* copy() const { return nullptr; };

    // Get's the name of the resource type
    virtual const char* get_name() const { return "Resource<>"; }
};