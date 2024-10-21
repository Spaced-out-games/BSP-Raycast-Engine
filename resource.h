#include <typeinfo>
#include <iostream>

template <typename inheritor_t = void>
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
    virtual void load() = 0;
    virtual void save() = 0;
    virtual void use() = 0;                       // Pure virtual function for using the resource

    // Copy function to be overridden in derived classes
    virtual Resource* copy() const = 0;

    // Get's the name of the resource type
    virtual const char* get_name() const { return "Resource<>"; }
};