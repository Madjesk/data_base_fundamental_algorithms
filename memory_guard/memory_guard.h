#include "../memory/memory.h"

class memory_guard
{
public:
    void *guard_allocate(size_t size_for_allocate) const;
    void guard_deallocate(void *obj) const;
    virtual memory *get_memory() const = 0;
    virtual ~memory_guard() = default;
};