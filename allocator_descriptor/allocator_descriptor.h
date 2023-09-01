#ifndef ALLOCATOR_DESCRIPTOR_H
#define ALLOCATOR_DESCRIPTOR_H
#include "memory.h"
#include <iostream>
#include <sstream>
#include "../logger/logger.h"

class allocator_descriptor: public memory
{
private:
    logger* _log;
    void* _memory;

    memory::METHODS _method;
public:


    explicit allocator_descriptor(size_t const& size, logger *my_logger = nullptr, memory::METHODS method = memory::METHODS::FIRST, memory *alloc = nullptr);

    void *allocate(size_t size) const override;

    void deallocate(void * target_to_dealloc) const override;

    ~allocator_descriptor();

    void* get_best_block(size_t &size) const;
    void* get_first_block(size_t &size) const;
    void* get_worst_block(size_t &size) const;

    memory* get_allocator() const noexcept;
    void* get_next_block(void *memory_block) const noexcept;
    size_t get_block_size(void* memory_block) const noexcept;
};

#endif