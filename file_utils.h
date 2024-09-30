#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>

inline std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

#include <fstream>
#include <stdexcept>
#include <vector>
#include <type_traits>

template <typename T>
class Serializer {
public:
    Serializer() = default;

    // Open a file for writing
    void openWrite(const std::string& filename) {
        ofs.open(filename, std::ios::binary);
        if (!ofs) {
            throw std::runtime_error("Failed to open file for writing");
        }
    }

    // Open a file for reading
    void openRead(const std::string& filename) {
        ifs.open(filename, std::ios::binary);
        if (!ifs) {
            throw std::runtime_error("Failed to open file for reading");
        }
    }

    // Close the file stream for writing
    void closeWrite() {
        if (ofs.is_open()) {
            ofs.close();
        }
    }

    // Close the file stream for reading
    void closeRead() {
        if (ifs.is_open()) {
            ifs.close();
        }
    }

    // Save function to serialize an object to a file
    void save(const T& data) {
        static_assert(std::is_trivially_copyable<T>::value, "Type must be trivially copyable for binary serialization.");
        if (!ofs.is_open()) {
            throw std::runtime_error("Output file stream is not open");
        }
        ofs.write(reinterpret_cast<const char*>(&data), sizeof(T));
    }

    // Load function to deserialize an object from a file
    T load() {
        static_assert(std::is_trivially_copyable<T>::value, "Type must be trivially copyable for binary serialization.");
        if (!ifs.is_open()) {
            throw std::runtime_error("Input file stream is not open");
        }
        T data;
        ifs.read(reinterpret_cast<char*>(&data), sizeof(T));
        return data;
    }

    // Save function to serialize a vector of objects to a file
    void save(const std::vector<T>& data) {
        static_assert(std::is_trivially_copyable<T>::value, "Type must be trivially copyable for binary serialization.");
        if (!ofs.is_open()) {
            throw std::runtime_error("Output file stream is not open");
        }
        size_t size = data.size();
        ofs.write(reinterpret_cast<const char*>(&size), sizeof(size));
        ofs.write(reinterpret_cast<const char*>(data.data()), size * sizeof(T));
    }

    // Load function to deserialize a vector of objects from a file
    std::vector<T> load_vector() {
        static_assert(std::is_trivially_copyable<T>::value, "Type must be trivially copyable for binary serialization.");
        if (!ifs.is_open()) {
            throw std::runtime_error("Input file stream is not open");
        }
        size_t size;
        ifs.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::vector<T> data(size);
        ifs.read(reinterpret_cast<char*>(data.data()), size * sizeof(T));
        return data;
    }

private:
    std::ofstream ofs;
    std::ifstream ifs;
};


#endif