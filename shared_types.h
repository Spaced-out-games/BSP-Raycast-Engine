#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H
/*
#include <cstdint>
#include <cstring> // for C's memset
#include <iostream>
#include <vector>
#include "Entity.h"

// Since bitset_512 is only BITSET_SIZE_BITS bits, we need not a full size_t to access every element of a bitset_512
#define bitset_size_t uint16_t

// A bitset is comprised of enough of these to form 64 bytes.
#define bitset_atomic_t uint64_t


// Define constants dynamically
#define BITSET_SIZE_BITS 512
#define NUM_BLOCKS (BITSET_SIZE_BITS / (sizeof(bitset_atomic_t) * 8))

enum ent_t
{
	ENTITY

};


typedef struct atomic_location // need a better name
{
    atomic_location(ent_t which_entity_ID, bitset_size_t which_block, bitset_size_t which_index) { ent_ID = which_entity_ID; block_index = which_block; atomic_index = which_index; }
	// Which entity section this component/entity belongs in.
	ent_t ent_ID;

	// Which block within this section this component/entity belongs in
    bitset_size_t block_index;

	// Which index within the block this component / entity belongs in [0-511]
    bitset_size_t atomic_index;

} atomic_location;




template <typename entity_t>
struct ent_block
{
public:
    ent_block() : entity_count(0)
    {
        // Initialize all entities to default-constructed entity_t
        for (bitset_size_t i = 0; i < BITSET_SIZE_BITS; ++i)
        {
            entities[i] = entity_t{};
        }
    }


    void insert(const entity_t& new_item)
    {
        if (entity_count < BITSET_SIZE_BITS)
        {
            // Find the first available slot
            bitset_size_t index = findSlot();
            if (index < BITSET_SIZE_BITS) // Ensure the index is within bounds
            {
                entities[index] = new_item;
                states.set(index, true);
                entity_count++;
            }
        }
    }

    void remove(bitset_size_t index)
    {
        if (index < BITSET_SIZE_BITS && states.get(index))
        {
            // Set the state to false
            states.set(index, false);
            // Clear the entity if needed
            entities[index] = entity_t{};
            entity_count--;
        }
    }

    const bitset_512& getStates() const
    {
        return states;
    }

    const entity_t& getEntity(bitset_size_t index)
    {
        if (index <= BITSET_SIZE_BITS) { return entities[index]; }
    }
    const bitset_size_t size()
    {
        return entity_count;
    }

    const bitset_size_t capacity() { return BITSET_SIZE_BITS; }

private:
    entity_t entities[BITSET_SIZE_BITS]; // Raw array for entities
    bitset_512 states;       // Bitset for tracking valid slots
    bitset_size_t entity_count; // Current number of entities

    bitset_size_t findSlot() const
    {
        for (bitset_size_t i = 0; i < BITSET_SIZE_BITS; ++i)
        {
            if (!states.get(i))
            {
                return i;
            }
        }
        return BITSET_SIZE_BITS; // Indicate no available index
    }
};

template <int entity_index, typename entity_t>
struct ent_array
{
    public:

        void insert(const entity_t& new_item)
        {
            // the index of the block
            bitset_size_t block_index = 0;

            // Iterate through each block
            for (bitset_size_t i = 0; i < blocks.size(); i++)
            {
                // If the size of this block is less than BITSET_SIZE_BITS, insert and return
                if (blocks[i].size() < BITSET_SIZE_BITS)
                {
                    blocks[i].insert(new_item);
                    return;
                }
                // otherwise, continue...
            }
            
            // If we made it this far, we couldn't find a slot in any of the blocks, so we must append a new block and insert there.

            blocks.push_back(ent_block<entity_t>);

            // Now that we have a block, add the fucking item already
            blocks[blocks.size() - 1].insert(new_item);

        }

    private:
        std::vector <ent_block<entity_t>> blocks;
};




// TODO: create ent_handle to give a means for entities to talk to one another
typedef struct ent_handle
{


    // What type of entity this handle points to. Points to which ent_array the entity is stored in within an ent_manager
    ent_t entity_type;

    // Which ent_block within the ent_array the handle points to [0-ent_manager[entity_type].size()]
    bitset_size_t block_index;

    // Which index within the ent_block the entity is stored at [0-511]
    bitset_size_t entry_index;


};

*/
#endif