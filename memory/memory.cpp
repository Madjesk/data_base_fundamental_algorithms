#include "memory.h"

void * memory::operator+=(const size_t &size) {
    return allocate(size);
}

void memory::operator-=(void *obj) {
    deallocate(obj);
}

memory::~memory()
{
}