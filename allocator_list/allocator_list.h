#ifndef ALLOCATOR_LIST
#define ALLOCATOR_LIST
#include "../memory/memory.h"
#include <iostream>
#include <sstream>
#include "logger/logger.h"

class allocator_list final : public memory
{
private:
    logger* _log;
    void* _memory;

    memory::METHODS _method;
public:
    void *allocate(size_t size) const override;

    void deallocate(void * deallocated_block) const override;

    allocator_list& operator=(allocator_list const&) = delete;

    explicit allocator_list(size_t const& size, logger *my_logger = nullptr, memory::METHODS method = memory::METHODS::FIRST, memory *alloc = nullptr);

    allocator_list(allocator_list const&) = delete;

    ~allocator_list();

    void* get_best(size_t const &size, void **prev_free_block, void **next_free_block) const;
    void* get_first(size_t const &size, void **prev_free_block, void **next_free_block) const;
    void* get_worst(size_t const &size, void **prev_free_block, void **next_free_block) const;

    void* get_first_free_block() const;
    void* get_next_block(void*) const noexcept;
    size_t get_size_block(void*) const;
    void* get_memory(size_t size, void**prev, void** next) const;
    void connect_blocks() const;
};

#endif