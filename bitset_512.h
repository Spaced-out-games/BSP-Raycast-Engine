#ifndef BITSET_512_H
#define BITSET_512_H

#include <cstdint>
#include <cstring>
#include <iostream>

// Since bitset_512 is only BITSET_SIZE_BITS bits, we need not a full size_t to access every element of a bitset_512
#define bitset_size_t uint16_t

// A bitset is comprised of enough of these to form 64 bytes.
#define bitset_atomic_t uint64_t

// Define constants dynamically
#define BITSET_SIZE_BITS 512
#define NUM_BLOCKS (BITSET_SIZE_BITS / (sizeof(bitset_atomic_t) * 8))

// A bitset of BITSET_SIZE_BITS states.
class bitset_512
{
public:
    bitset_512()
    {
        std::memset(states, 0, sizeof(states));
    }

    void set(bitset_size_t index, bool state)
    {
        bitset_size_t array_index = index / (sizeof(bitset_atomic_t) * 8);
        bitset_size_t bit_index = index % (sizeof(bitset_atomic_t) * 8);

        bitset_atomic_t mask = bitset_atomic_t(1) << bit_index;
        if (state)
            states[array_index] |= mask;
        else
            states[array_index] &= ~mask;
    }

    bool get(bitset_size_t index) const
    {
        bitset_size_t array_index = index / (sizeof(bitset_atomic_t) * 8);
        bitset_size_t bit_index = index % (sizeof(bitset_atomic_t) * 8);

        bitset_atomic_t mask = bitset_atomic_t(1) << bit_index;
        return (states[array_index] & mask) != 0;
    }

    void zeros()
    {
        std::memset(states, 0, sizeof(states));
    }

    void ones()
    {
        std::memset(states, 0xff, sizeof(states));
    }

    int find_first_empty_slot() const {
        for (int i = 0; i < NUM_BLOCKS; ++i) {
            uint64_t block = states[i];
            uint64_t not_full_block = ~block;

            if (not_full_block) {
                unsigned long bit_index;
                if (_BitScanForward64(&bit_index, not_full_block)) {
                    return i * (sizeof(bitset_atomic_t) * 8) + bit_index;
                }
            }
        }

        return -1;
    }

    int find_last_set_bit() const {
        for (int i = NUM_BLOCKS - 1; i >= 0; --i) {
            uint64_t block = states[i];

            if (block != 0) {
                unsigned long bit_index;
                #if defined(_MSC_VER) // Visual Studio
                _BitScanReverse64(&bit_index, block);
                #else // GCC/Clang
                bit_index = 63 - __builtin_clzll(block);
                #endif
                return i * (sizeof(bitset_atomic_t) * 8) + bit_index;
            }
        }

        return -1;
    }

private:
    bitset_atomic_t states[8]; // BITSET_SIZE_BITS bits / 64 bits per uint64_t = 8 uint64_t elements
};

// Output the bitset to an ostream
inline std::ostream& operator<<(std::ostream& os, const bitset_512& bs)
{
    for (bitset_size_t i = 0; i < BITSET_SIZE_BITS; ++i)
    {
        os << (bs.get(i) ? '1' : '0');
        if (i % 64 == 63) os << '\n'; // Print a new line every 64 bits
    }
    return os;
}

#endif // BITSET_512_H