#include "memory_guard.h"

void *memory_guard::guard_allocate(size_t size_for_allocate) const
{
    memory *alloc= get_memory();

    if(alloc == nullptr)
    {
       return ::operator new(size_for_allocate);
    }
    else
    {
      return  alloc->allocate(size_for_allocate);
    }
}

void memory_guard::guard_deallocate(void * obj) const
{
    memory *alloc= get_memory();

    if(alloc == nullptr)
    {
        ::operator delete(obj);
    }
    else
    {
        alloc->deallocate(obj);
    }
}

