#pragma once
#include "ent.h"
#include <vector>
#include "iostream"



template <typename T = ent>
class ent_container {
private:
    std::vector<T> container;  // The underlying vector

public:
    // Constructors
    ent_container() = default;
    ent_container(size_t initial_size) : container(initial_size) {}

    // Wrap std::vector's push_back
    void push_back(const T& value) {
        container.push_back(value);
    }

    void push_back(T&& value) {
        container.push_back(std::move(value));
    }

    // Wrap std::vector's emplace_back
    template <typename... Args>
    void emplace_back(Args&&... args) {
        container.emplace_back(std::forward<Args>(args)...);
    }

    // Wrap std::vector's operator[]
    T& operator[](size_t index) {
        return container[index];
    }

    const T& operator[](size_t index) const {
        return container[index];
    }

    // Wrap std::vector's size method
    size_t size() const {
        return container.size();
    }

    // Wrap std::vector's capacity method
    size_t capacity() const {
        return container.capacity();
    }

    // Wrap std::vector's resize method
    void resize(size_t new_size) {
        container.resize(new_size);
    }

    // Wrap std::vector's clear method
    void clear() {
        container.clear();
    }

    // Wrap std::vector's begin and end for iterators
    typename std::vector<T>::iterator begin() {
        return container.begin();
    }

    typename std::vector<T>::const_iterator begin() const {
        return container.begin();
    }

    typename std::vector<T>::iterator end() {
        return container.end();
    }

    typename std::vector<T>::const_iterator end() const {
        return container.end();
    }

    // Add any other custom methods you'd like to extend functionality.
    void remove_at(size_t index) {
        if (index < container.size()) {
            container.erase(container.begin() + index);
        }
    }

    // Expose the underlying container if absolutely necessary
    std::vector<T>& get_container() {
        return container;
    }
};


