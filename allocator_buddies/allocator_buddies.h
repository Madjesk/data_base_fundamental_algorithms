#ifndef allocator_buddies_H
#define allocator_buddies_H
#include "../memory/memory.h"
#include <iostream>
#include <sstream>
#include "../logger/logger.h"

class allocator_buddies final : public memory
{
private:
    logger* _log;
    void* _memory;
    memory::METHODS _method;
public:
    void * allocate(size_t target_size) const override;

    void deallocate(void * target_to_dealloc) const override;

    allocator_buddies& operator=(allocator_buddies const&) = delete;


    explicit allocator_buddies(
            size_t power_of_2,
            memory::METHODS method = memory::METHODS::FIRST,
            logger *logger = nullptr,
            memory *outer_allocator = nullptr
    );

    allocator_buddies(allocator_buddies const&) = delete;

    size_t get_allocated_memory_size() const;
    memory *get_memory() const;
    logger * get_logger();

    void * get_buddies(void * current_block) const;
    void * get_start_allocated_memory_address() const;
    memory * get_outer_allocator();
    void ** get_first_available_block_ptr() const;
    void * get_first_available_block() const;
    ~allocator_buddies();

    void * get_next_available_block(void * current_block) const;
    void * get_prev_available_block(void *current_block) const;
    size_t get_available_block_size(void * current_block) const;
    size_t get_occupied_block_service_block_size() const;
    size_t get_occupied_block_size(void * current_block) const;
    bool get_block_status(void * current_block) const;

    memory::METHODS get_allocation_mode() const;
};

#endif