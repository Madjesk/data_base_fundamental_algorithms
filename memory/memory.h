#ifndef MEMORY_H
#define MEMORY_H
#include <iostream>

class memory
{

public:
    enum class METHODS {
        BEST,
        WORST,
        FIRST
    };

    void* operator +=(size_t const&);

    void operator -=(void *obj);

    void operator =(memory const&) = delete;

    [[nodiscard]] virtual void *allocate(size_t size) const = 0;

    virtual void deallocate(void * deallocated_block) const = 0;

    memory() = default;

    memory(memory const&) = delete;

    virtual ~memory();
    //todo перемещение дописать
};
#endif