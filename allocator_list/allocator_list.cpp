#include "allocator_list.h"

template<typename T>
std::string to_str(T obj)
{
    std::stringstream stream;
    stream << obj;
    return stream.str();
}

std::string get_bytess(void* obj) {
    std::string res;
    unsigned char *bytes = reinterpret_cast<unsigned char*>(obj);
    size_t size_object = *reinterpret_cast<size_t*>(reinterpret_cast<size_t*>(obj) - sizeof(size_t));

    for (int i = 0; i < size_object; i++)
    {
        res += to_str(static_cast<int>(bytes[i])) + " ";
    }
    return res;
}

void* allocator_list::get_first_free_block() const
{
    return *reinterpret_cast<void**>(reinterpret_cast<size_t*>(_memory) + 1);
}

void* allocator_list::get_next_block(void *obj) const noexcept
{
    return *reinterpret_cast<void**>(reinterpret_cast<size_t*>(obj) + 1);
}

size_t allocator_list::get_size_block(void *obj) const
{
    return *reinterpret_cast<size_t*>(obj);
}

allocator_list::allocator_list(size_t const& size, logger * my_logger, allocator_list::METHODS method, memory* allocator)
    : _log(my_logger), _method(method)
    {
    if(allocator == nullptr)
    {
        _memory = ::operator new(size);
    } else
    {
        _memory = allocator->allocate(size);
    }
    size_t *size_memory = reinterpret_cast<size_t*>(_memory);
    *size_memory = size;

    void **next_block = reinterpret_cast<void**>(size_memory + 1);
    *next_block = reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(_memory) + sizeof(size_t) + sizeof(void*) + sizeof(memory*) + sizeof(logger*));

    memory **allocator_space = reinterpret_cast<memory**>(next_block + 1);
    *allocator_space = allocator;

    logger **logger_space = reinterpret_cast<logger**>(allocator_space + 1);
    *logger_space = my_logger;

    size_t *size_free_space = reinterpret_cast<size_t*>(*next_block);
    *size_free_space = size - sizeof(size_t) - sizeof(memory*) - sizeof(void*) - sizeof(size_t) - sizeof(void*) - sizeof(logger*);

    void **next_free_block = reinterpret_cast<void**>(size_free_space + 1);
    *next_free_block = nullptr;
    //_log->log("allocator has been created", logger::severity::debug);
    //_log->log("size of memory which was allocated: " + to_str(*size_memory), logger::severity::debug);
}



void *allocator_list::get_first(size_t const &size, void **prev_free_block, void **next_free_block) const
{
    void *current_free_block = get_first_free_block();
    *prev_free_block = *next_free_block = nullptr;

    while (current_free_block != nullptr)
    {
        if (get_size_block(current_free_block) >= size)
        {
            *next_free_block = get_next_block(current_free_block);
            return current_free_block;
        }

        *prev_free_block = current_free_block;
        current_free_block = get_next_block(current_free_block);
    }
    return nullptr;
}

void *allocator_list::get_best(size_t const &size, void **prev_free_block, void **next_free_block) const
{
    void *current_free_block = get_first_free_block();
    void *best_block = current_free_block;
    *prev_free_block = *next_free_block = nullptr;

    while (current_free_block != nullptr)
    {
        if (get_size_block(best_block) >= size && get_size_block(current_free_block) > get_size_block(best_block))
        {
            best_block = current_free_block;
        }
        if(get_next_block(current_free_block) != nullptr) {
            *prev_free_block = current_free_block;
        }
        current_free_block = get_next_block(current_free_block);
    }
    *next_free_block = get_next_block(best_block);
    return best_block;
}

void *allocator_list::get_worst(size_t const &size, void **prev_free_block, void **next_free_block) const
{
    void *current_free_block = get_first_free_block();
    void *worst_block = current_free_block;
    *prev_free_block = *next_free_block =nullptr;

    while (current_free_block != nullptr)
    {
        if (get_size_block(worst_block) >= size && get_size_block(current_free_block) < get_size_block(worst_block))
        {
            worst_block = current_free_block;
        }
        if(get_next_block(current_free_block) != nullptr)
        {
            *prev_free_block = current_free_block;
        }
        current_free_block = get_next_block(current_free_block);
    }
    *next_free_block = get_next_block(worst_block);
    return worst_block;
}

void *allocator_list::get_memory(size_t size, void**prev, void** next) const {
    if (_method == memory::METHODS::BEST) {
        return get_best(size, prev, next);
    } else if (_method == memory::METHODS::FIRST) {
        return  get_first(size, prev, next);
    } else if (_method == memory::METHODS::WORST) {
        return  get_worst(size, prev, next);
    }
}

void allocator_list::connect_blocks() const {
    void *current_free_block = get_first_free_block();
    while (current_free_block != nullptr) {
        void * next_free_block = get_next_block(current_free_block);
        if (*reinterpret_cast<size_t *>(current_free_block) ==
            reinterpret_cast<char *>(next_free_block) - reinterpret_cast<char *>(current_free_block) - sizeof(size_t) -
            sizeof(void *)) {
            *reinterpret_cast<void **>(reinterpret_cast<size_t *>(current_free_block) + 1) = get_next_block(
                    next_free_block);
            *reinterpret_cast<size_t *>(current_free_block) =
                    get_size_block(current_free_block) + get_size_block(next_free_block) + sizeof(size_t) +
                    sizeof(void *);
        } else {
            current_free_block = next_free_block;
        }
    }
}

void allocator_list::deallocate(void * deallocated_block) const
{
    void *current_free_block = get_first_free_block();
    void *prev_free_block = nullptr;

    while (current_free_block != nullptr && current_free_block < deallocated_block)
    {
        prev_free_block = current_free_block;
        current_free_block = get_next_block(current_free_block);
    }
    if (prev_free_block == nullptr)
    {
        *reinterpret_cast<void**>(reinterpret_cast<size_t*>(_memory) + 1) = reinterpret_cast<size_t*>(deallocated_block) - sizeof(size_t);
    }
    else
    {
        *reinterpret_cast<void**>(reinterpret_cast<size_t*>(prev_free_block) + 1) = reinterpret_cast<size_t*>(deallocated_block) - sizeof(size_t);
    }
    //_log->log("Deallocated BLOCK: "+ to_str(reinterpret_cast<size_t*>(deallocated_block) - sizeof(size_t))+ " bytes= " + get_bytes(deallocated_block), logger::severity::debug);
    *reinterpret_cast<void**>(deallocated_block) = current_free_block;
    *reinterpret_cast<size_t*>(reinterpret_cast<size_t*>(deallocated_block) - sizeof(size_t)) -= sizeof(void*);
    connect_blocks();
}

void * allocator_list::allocate(size_t size) const
{
    size_t *size_of_memory = reinterpret_cast<size_t*>(_memory);
    void *prev_free_block = nullptr;
    void *next_free_block = nullptr;
    void *next_block = nullptr;
    size_t size_free_block = 0;
    void *current_free_block = get_memory(size, &prev_free_block, &next_free_block);

    if (current_free_block != nullptr)
    {
        size_free_block = get_size_block(current_free_block);
        size_t extra_block_size = 0;

        if (prev_free_block == nullptr)
        {
            if (size_free_block + sizeof(void*) - size < sizeof(size_t) + sizeof(void*))
            {
                extra_block_size = size_free_block + sizeof(void*) - size;
                *reinterpret_cast<void**>(size_of_memory + 1) = next_free_block;
            }
            else
            {
                next_block = reinterpret_cast<size_t*>(current_free_block) + sizeof(size_t) + size;
                *reinterpret_cast<void**>(size_of_memory + 1) = next_block;
                *reinterpret_cast<size_t*>(next_block) = get_size_block(current_free_block) - size - sizeof(size_t);
                *reinterpret_cast<void**>(reinterpret_cast<size_t*>(next_block) + 1) = next_free_block;
            }
        }
        else
        {
            if (size_free_block + sizeof(void*) - size < sizeof(size_t) + sizeof(void*))
            {
                extra_block_size = size_free_block + sizeof(void*) - size;
                *reinterpret_cast<void**>(reinterpret_cast<size_t*>(prev_free_block) + 1) = next_free_block;
            }
            else
            {
                next_block = reinterpret_cast<size_t*>(current_free_block) + size + sizeof(size_t);
                *reinterpret_cast<void**>(reinterpret_cast<size_t*>(prev_free_block) + 1) = reinterpret_cast<size_t*>(current_free_block) + size + sizeof(size_t);
                *reinterpret_cast<size_t*>(next_block) = get_size_block(current_free_block) - size - sizeof(size_t);
                *reinterpret_cast<void**>(reinterpret_cast<size_t*>(next_block) + 1) = next_free_block;
            }
        }

        *reinterpret_cast<size_t*>(current_free_block) = size + extra_block_size;

        //_log->log("Allocated block: "+  to_str(current_free_block) + " with size = " + to_str(size), logger::severity::debug);
        return reinterpret_cast<size_t*>(current_free_block) + sizeof(size_t);
    }
    throw std::exception();
//    return nullptr;
}

allocator_list::~allocator_list()
{
    memory *alc = *reinterpret_cast<memory**>(reinterpret_cast<unsigned char*>(_memory) + sizeof(size_t) + sizeof(void*));

    if(alc != nullptr){
        alc->deallocate(_memory);
    }
    else{
        ::operator delete(_memory);
    }
    //_log->log("Allocator has been removed bytes", logger::severity::debug);
}
