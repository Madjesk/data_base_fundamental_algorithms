#include "allocator_buddies.h"
#include <cmath>
#include <sstream>


size_t get_allocator_service_block_size()
{
    return sizeof(size_t) + sizeof(memory*) + sizeof(logger*) + sizeof(memory::METHODS) + sizeof(void *);
}

memory * allocator_buddies::get_outer_allocator()
{
    return *reinterpret_cast<memory**>(reinterpret_cast<unsigned char*>(_memory) + sizeof(size_t));
}

logger * allocator_buddies::get_logger()
{
    return *reinterpret_cast<logger**>(reinterpret_cast<unsigned char*>(_memory) + sizeof(size_t) + sizeof(memory*));
}

memory::METHODS allocator_buddies::get_allocation_mode() const
{
    return *reinterpret_cast<memory::METHODS*>(reinterpret_cast<unsigned char*>(_memory) + sizeof(size_t) + sizeof(memory*) + sizeof(logger*));
}

void ** allocator_buddies::get_first_available_block_ptr() const
{
    return reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(_memory) + sizeof(size_t) + sizeof(memory*) + sizeof(logger*) + sizeof(memory::METHODS));
}

void * allocator_buddies::get_first_available_block() const
{
    return *get_first_available_block_ptr();
}

size_t allocator_buddies::get_occupied_block_size(void * current_block) const
{
    return 1 << (*(reinterpret_cast<unsigned char*>(current_block) - 1) >> 1);
}


size_t allocator_buddies::get_occupied_block_service_block_size() const
{
    return sizeof(unsigned char);
}


size_t allocator_buddies::get_available_block_size(void * current_block) const
{
    return 1 << (*reinterpret_cast<unsigned char*>(current_block) >> 1);
}

void * allocator_buddies::get_prev_available_block(void *current_block) const
{
    return *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(current_block) + sizeof(unsigned char));
}

void * allocator_buddies::get_next_available_block(void * current_block) const
{
    return *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(current_block) + sizeof(unsigned char) + sizeof(void*));
}


bool allocator_buddies::get_block_status(void * current_block) const
{
    return *reinterpret_cast<unsigned char*>(current_block) & 1;
}

void * allocator_buddies::get_start_allocated_memory_address() const
{
    return reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(_memory) + get_allocator_service_block_size());
}

size_t allocator_buddies::get_allocated_memory_size() const
{
    return *reinterpret_cast<size_t*>(_memory);
}

void * allocator_buddies::get_buddies(void * current_block) const
{
    size_t block_size = get_available_block_size(current_block);

    if (block_size == get_allocated_memory_size())
    {
        return nullptr;
    }

    auto * const start_allocated_memory = get_start_allocated_memory_address();

    size_t relative_address = reinterpret_cast<unsigned char*>(current_block) - reinterpret_cast<unsigned char*>(start_allocated_memory);
    size_t relative_address_buddies = relative_address ^ block_size;

    return reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(start_allocated_memory) + relative_address_buddies);
}

allocator_buddies::allocator_buddies(size_t power_of_2,memory::METHODS mode, logger *log, memory *outer_allocator)
{


    size_t request_size = 1 << power_of_2;
    size_t target_size = request_size + get_allocator_service_block_size();

    if (outer_allocator == nullptr)
    {
        _memory = ::operator new(target_size);
    }
    else
    {
        _memory = outer_allocator->allocate(target_size);
    }

    auto * const allocated_memory_size = reinterpret_cast<size_t*>(_memory);
    *allocated_memory_size = target_size;

    auto * const outer_allocator_ptr = reinterpret_cast<memory**>(allocated_memory_size + 1);
    *outer_allocator_ptr = outer_allocator;

    auto * const logger_ptr = reinterpret_cast<logger**>(outer_allocator_ptr + 1);
    *logger_ptr = log;

    auto * const allocation_mode_ptr = reinterpret_cast<memory::METHODS*>(logger_ptr + 1);
    *allocation_mode_ptr = mode;

    auto * const first_available_block_ptr = reinterpret_cast<void**>(allocation_mode_ptr + 1);
    *first_available_block_ptr = reinterpret_cast<void*>(first_available_block_ptr + 1);

    auto * const first_available_block_size_and_status = reinterpret_cast<unsigned char*>(*first_available_block_ptr);
    *first_available_block_size_and_status = (static_cast<unsigned char>(power_of_2) << 1) | 0;

    auto * const previous_available_block = reinterpret_cast<void**>(first_available_block_size_and_status + 1);
    *previous_available_block = nullptr;

    auto * const next_available_block = reinterpret_cast<void**>(previous_available_block + 1);
    *next_available_block = nullptr;
}


void * allocator_buddies::allocate(size_t request_size) const{
    if (request_size < 2 * sizeof(void*))
    {

        request_size = 2 * sizeof(void*);
    }
    void *target_block = nullptr;
    auto current_block = get_first_available_block();
    auto occupied_block_service_block_size = get_occupied_block_service_block_size();
    auto allocation_mode = get_allocation_mode();

    while (current_block != nullptr)
    {
        auto current_block_size = get_available_block_size(current_block);
        auto next_block = get_next_available_block(current_block);

        if (current_block_size >= request_size + occupied_block_service_block_size)
        {
            if (
                    allocation_mode == memory::METHODS::FIRST ||
                    allocation_mode == memory::METHODS::BEST && (target_block == nullptr || current_block_size < get_available_block_size(target_block)) ||
                    allocation_mode == memory::METHODS::WORST && (target_block == nullptr || current_block_size > get_available_block_size(target_block))
                    )
            {
                target_block = current_block;
            }

            if (allocation_mode == memory::METHODS::FIRST)
            {
                break;
            }
        }

        current_block = next_block;
    }

    if (target_block == nullptr)
    {
        std::string message = "No available memory";
    }

    size_t target_block_size = get_available_block_size(target_block);

    while ((target_block_size >> 1) >= (request_size + occupied_block_service_block_size))
    {
        auto * const left_buddy = reinterpret_cast<unsigned char*>(target_block);

        auto * const right_buddy = left_buddy + (target_block_size >>= 1);

        *right_buddy = *left_buddy = ((*left_buddy >> 1) - 1) << 1;

        auto * const next_block = get_next_available_block(reinterpret_cast<void*>(left_buddy));

        if (next_block != nullptr)
        {
            *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(next_block) + 1) = reinterpret_cast<void*>(right_buddy);
        }
        *(reinterpret_cast<void**>(right_buddy + 1) + 1) = next_block;

        *(reinterpret_cast<void**>(left_buddy + 1) + 1) = reinterpret_cast<void*>(right_buddy);

        *reinterpret_cast<void**>(right_buddy + 1) = reinterpret_cast<void*>(left_buddy);
    }

    *reinterpret_cast<unsigned char*>(target_block) |= 1 ;

    auto * const previous_to_target_block = get_prev_available_block(target_block);
    auto * const next_to_target_block = get_next_available_block(target_block);

    if (previous_to_target_block == nullptr)
    {
        auto * const first_available_block_ptr = get_first_available_block_ptr();
        *first_available_block_ptr = next_to_target_block;

        if (next_to_target_block != nullptr)
        {
            *reinterpret_cast<void **>(reinterpret_cast<unsigned char *>(next_to_target_block) + 1) = nullptr;
        }
    }
    else
    {
        *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(previous_to_target_block) + sizeof(unsigned char) + sizeof(void*)) = next_to_target_block;

        if (next_to_target_block != nullptr)
        {
            *reinterpret_cast<void **>(reinterpret_cast<unsigned char *>(next_to_target_block) + 1) = previous_to_target_block;
        }
    }

    auto allocated_block = reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(target_block) + sizeof(unsigned char));


    return allocated_block;
}


void allocator_buddies::deallocate(void * target_to_dealloc) const {
    auto * target_block = reinterpret_cast<void*>(reinterpret_cast<unsigned char*>(target_to_dealloc) - 1);

    auto buddies = get_buddies(target_block);

    if (buddies != nullptr && (get_available_block_size(target_block) == get_available_block_size(buddies)) && (get_block_status(buddies) == false))
    {
        while (buddies != nullptr && (get_available_block_size(target_block) == get_available_block_size(buddies)) && (get_block_status(buddies) == false))
        {
            if (buddies < target_block)
            {
                if (get_block_status(target_block) == false)
                {
                    auto * const next_available_block_for_target = get_next_available_block(target_block);
                    *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(buddies) + sizeof(unsigned char) + sizeof(void*)) = next_available_block_for_target;

                    if (next_available_block_for_target != nullptr)
                    {
                        *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(next_available_block_for_target) + sizeof(unsigned char)) = buddies;
                    }
                }

                *reinterpret_cast<unsigned char*>(buddies) = ((*reinterpret_cast<unsigned char*>(buddies) >> 1) + 1) << 1;

                target_block = buddies;
            }
            else
            {
                auto * const previous_available_block_for_buddies = get_prev_available_block(buddies);
                auto * const next_available_block_for_buddies = get_next_available_block(buddies);

                if (get_block_status(target_block) == true)
                {
                    *reinterpret_cast<void **>(reinterpret_cast<unsigned char *>(target_block) + sizeof(unsigned char)) = previous_available_block_for_buddies;

                    if (previous_available_block_for_buddies == nullptr)
                    {
                        auto * const first_available_block_ptr = get_first_available_block_ptr();
                        *first_available_block_ptr = target_block;
                    }
                    else
                    {
                        *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(previous_available_block_for_buddies) + sizeof(unsigned char) + sizeof(void*)) = target_block;
                    }
                }

                *reinterpret_cast<unsigned char*>(target_block) = ((*reinterpret_cast<unsigned char*>(target_block) >> 1) + 1) << 1;

                *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(target_block) + sizeof(unsigned char) + sizeof(void*)) = next_available_block_for_buddies;

                if (next_available_block_for_buddies != nullptr)
                {
                    *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(next_available_block_for_buddies) + sizeof(unsigned char)) = target_block;
                }
            }

            buddies = get_buddies(target_block);
        }
    }
    else
    {
        void * previous_to_target_block = nullptr;
        auto next_to_target_block = get_first_available_block();

        while (next_to_target_block != nullptr && next_to_target_block < target_block)
        {
            previous_to_target_block = next_to_target_block;
            next_to_target_block = get_next_available_block(next_to_target_block);
        }

        *(reinterpret_cast<unsigned char*>(target_block)) &= ~1;
        *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(target_block) + sizeof(unsigned char)) = previous_to_target_block;
        *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(target_block) + sizeof(unsigned char) + sizeof(void*)) = next_to_target_block;

        if (previous_to_target_block == nullptr)
        {
            auto * const first_available_block_ptr = get_first_available_block_ptr();
            *first_available_block_ptr = target_block;
        }
        else
        {
            *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(previous_to_target_block) + sizeof(unsigned char) + sizeof(void*)) = target_block;
        }

        if (next_to_target_block != nullptr)
        {
            *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(next_to_target_block) + sizeof(unsigned char)) = target_block;
        }
    }

}

allocator_buddies::~allocator_buddies()
{
    const auto * const outer_allocator = get_outer_allocator();
    if (outer_allocator == nullptr)
    {
        ::operator delete(_memory);
    }
    else
    {
        outer_allocator->deallocate(_memory);
    }
}