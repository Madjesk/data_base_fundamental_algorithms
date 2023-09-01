#include "allocator_descriptor.h"

template<typename T>
std::string to_str(T obj)
{
    std::stringstream stream;
    stream << obj;
    return stream.str();
}

std::string get_bytes(void* obj) {
    std::string res;
    unsigned char *bytes = reinterpret_cast<unsigned char*>(obj);
    size_t size_object = *reinterpret_cast<size_t*>(reinterpret_cast<size_t*>(obj) - sizeof(size_t));

    for (int i = 0; i < size_object; i++)
    {
        res += to_str(static_cast<int>(bytes[i])) + " ";
    }
    return res;
}

allocator_descriptor::allocator_descriptor(size_t const& size, logger *my_logger, memory::METHODS method_allocation, memory *alloc)
{
    _method = method_allocation;
    if(alloc == nullptr)
    {
        _memory = ::operator new(size + sizeof(size_t) + sizeof(void*) + sizeof(memory*) + sizeof(logger*));
    }
    else
    {
        _memory = alloc->allocate(size);
    }

    auto* memory_size = reinterpret_cast<size_t*>(_memory);
    *memory_size = size;

    void** next_block = reinterpret_cast<void**>(memory_size + 1);
    next_block = nullptr;

    auto **logger_space = reinterpret_cast<logger**>(reinterpret_cast<unsigned char*>(_memory) + sizeof(size_t) + sizeof(void*));
    *logger_space = my_logger;

    auto **outer_allocator_space = reinterpret_cast<memory**>(logger_space + 1);
    *outer_allocator_space = alloc;


}

memory* allocator_descriptor::get_allocator() const noexcept
{
    //тоже работает
//    auto **logger_space = reinterpret_cast<logger**>(reinterpret_cast<unsigned char*>(_memory) + sizeof(size_t) + sizeof(void*));
//    memory* alc =  *reinterpret_cast<memory**>(logger_space + 1);
    return *reinterpret_cast<memory**>(reinterpret_cast<unsigned char*>(_memory) + sizeof(size_t) + sizeof(void*) + sizeof(logger*));
}

allocator_descriptor::~allocator_descriptor()
{
    memory* alc =  get_allocator();

    if(alc != nullptr)
    {
        alc->deallocate(_memory);
    }
    else
    {
        ::operator delete(_memory);
    }
}



void* allocator_descriptor::allocate(size_t size) const
{
    void *block = nullptr;
        switch (_method)
        {
            case memory::METHODS::FIRST:
                block = get_first_block(size);

            case memory::METHODS::BEST:
                block = get_best_block(size);

            case memory::METHODS::WORST:
                block = get_worst_block(size);

            default: break;
        }
    return block;
}

void *allocator_descriptor::get_next_block(void *memory_block) const noexcept
{
    return *reinterpret_cast<void**>(reinterpret_cast<size_t*>(memory_block) + 1);
}


size_t allocator_descriptor::get_block_size(void* memory_block) const noexcept
{
    return *reinterpret_cast<size_t const*>(memory_block);
}

void* allocator_descriptor::get_first_block(size_t &size) const {
    void *curr = get_next_block(_memory);
    void *block = nullptr;
    if (curr == nullptr) //нет доступных блоков памяти(выделенных),// проверяем можно ли выделить новый блок размером size
    {
        // проверяем можно ли выделить новый блок размером size, 2 void* на след и пред блок
        if((size + sizeof(size_t) + sizeof(void*) + sizeof(void*)) <= get_block_size(_memory))
        {
            //получаем указатель на блок памяти, который будет выделен
            block = reinterpret_cast<unsigned char*>(_memory) + sizeof(size_t) + sizeof(void*) + sizeof(logger*)+ sizeof(memory*);
            auto* block_size = reinterpret_cast<size_t*>(block);
            *block_size = size;

            //ставим указатели на следующий и предыдущий блок
            *reinterpret_cast<void**>(block_size + 1) = nullptr;
            *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(block_size + 1) + sizeof(void*)) = nullptr;

            //записали адрес блока который выделилил(первый свободный)
            *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(_memory) + sizeof(size_t)) = block;
            //возвращаем указатель на начало выделенного блока памяти которую можно юзать
            return reinterpret_cast<unsigned char*>(block) + sizeof(size_t) + sizeof(void*) + sizeof(void*);
        }
    }
    else
    {
        //ищем свободный блок
        while(curr != nullptr)
        {
            //следущий блок памяти в списке свободных блоков
            void* next = get_next_block(curr);
            auto * left = reinterpret_cast<unsigned char*>(curr) + sizeof(size_t) + sizeof(void*) + sizeof(void*) + get_block_size(curr);

            //если curr оказался последним свободным блоком
            if(next == nullptr)
            {
                //указатель на конец всей доступной памяти
                auto *right = reinterpret_cast<unsigned char*>(_memory) + sizeof(logger*) + sizeof(void*) + sizeof(size_t) + sizeof(memory*) + get_block_size(_memory);
                size_t distance = right - left;
                //если хватает места чтобы вставить блок
                if(size + sizeof(size_t) + sizeof(void*) + sizeof(void*) <= distance)
                {
                    block = left;
                    *reinterpret_cast<size_t*>(block) = size;
                    //следущий для block
                    *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(block) + sizeof(size_t)) = nullptr;
                    //предыдущий для block
                    *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(block) + sizeof(size_t) + sizeof(void*)) = curr;
                    //следующий для curr
                    *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(curr) + sizeof(size_t)) = block;
                    return reinterpret_cast<unsigned char*>(block) + sizeof(size_t) + sizeof(void*) + sizeof(void*);
                }
            }
            else //если ещё есть свободные блоки
            {
                //указатель на следующий блок после curr
                auto* right = reinterpret_cast<unsigned char*>(get_next_block(curr));
                size_t distance = right - left;
                if(size + sizeof(size_t) + sizeof(void*) + sizeof(void*) <= distance){
                    block = left;
                    *reinterpret_cast<size_t*>(block) = size;
                    //cледующий для curr
                    *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(curr) + sizeof(size_t)) = block;
                    //предыдущий для block
                    *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(block) + sizeof(size_t) + sizeof(void*)) = curr;
                    //следующий для block
                    *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(block) + sizeof(size_t)) = next;
                    //пред для next
                    *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(next) + sizeof(size_t) + sizeof(void*)) = block;

                    return reinterpret_cast<unsigned char*>(block) + sizeof(size_t) + (sizeof(void*) << 1);
                }
            }
            curr = next;
        }
    }
    return nullptr;
}

void allocator_descriptor::deallocate(void * block_for_deallocate) const {
    if (block_for_deallocate == nullptr){
        return;
    }
    memory* alc = get_allocator();

    if (alc == nullptr)
    {
        void* block = reinterpret_cast<unsigned char*>(const_cast<void*>(block_for_deallocate)) - sizeof(void*) - sizeof(void*) - sizeof(size_t);
        auto* prev = *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(block) + sizeof(size_t) + sizeof(void*));
        auto* next = *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(block) + sizeof(size_t));

        if (next != nullptr)
        {
            *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(next) + sizeof(size_t) + sizeof(void*)) = prev;
        }
        else
        {
            *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(_memory) + sizeof(size_t)) = next;
        }
    }
    else{
        alc->deallocate(block_for_deallocate);

    }
}


void* allocator_descriptor::get_best_block(size_t &size) const
{
    void* curr = get_next_block(_memory);
    void* block = nullptr;
    void* prev = nullptr;
    void* next1 = nullptr;

    if(curr == nullptr){
        if (size + sizeof(size_t) + sizeof(void*) + sizeof(void*) <= get_block_size(_memory)){
            block = (reinterpret_cast<unsigned char*>(_memory) + sizeof(size_t) + sizeof(void*) + sizeof(logger*) + sizeof(memory*));
            auto* block_size = reinterpret_cast<size_t*>(block);
            *block_size = size;

            *reinterpret_cast<void**>(block_size + 1) = nullptr;
            *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(block_size + 1) + sizeof(void*)) = nullptr;
            *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(_memory) + sizeof(size_t)) = block;
            return reinterpret_cast<unsigned char*>(block) + sizeof(size_t) + sizeof(void*) + sizeof(void*);
        }
    }else{
        size_t best_size = 2147483647;
        while(curr != nullptr){
            void* next = get_next_block(curr);
            auto * left = reinterpret_cast<unsigned char*>(curr) + sizeof(size_t) + sizeof(void*) + sizeof(void*) + get_block_size(curr);
//            auto * right = next == nullptr ? reinterpret_cast<unsigned char*>(_memory) + (sizeof(logger*) + sizeof(memory*) + sizeof(void*) + sizeof(size_t)) + get_block_size(_memory) : reinterpret_cast<unsigned char*>(next);
            auto* right = next == nullptr ?
                          reinterpret_cast<unsigned char*>(_memory) + (sizeof(logger*) + sizeof(memory*) + sizeof(void*) + sizeof(size_t)) + get_block_size(_memory) :
                          reinterpret_cast<unsigned char*>(next);
            size_t distance = right - left;
            if(size + sizeof(size_t) + sizeof(void*) +sizeof(void*) <= distance && distance < best_size){
                best_size = distance;
                block = left;
                prev = curr;
                next1 = next;
            }
            curr = next;
        }
        if (block != nullptr){
            *reinterpret_cast<size_t*>(block) = size;
            *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(block) + sizeof(size_t)) = next1;
            *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(block) + sizeof(size_t) + sizeof(void*)) = prev;
            *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(prev) + sizeof(size_t)) = block;
            if (next1 != nullptr){
                *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(next1) + sizeof(size_t) + sizeof(void*)) = block;
            }
            return reinterpret_cast<unsigned char*>(block) + sizeof(size_t) + sizeof(void*) + sizeof(void*);
        }
    }
    return nullptr;
}

void* allocator_descriptor::get_worst_block(size_t &size) const
{
    void* curr = get_next_block(_memory), *block = nullptr, *previous = nullptr, *next_one = nullptr;
    if(curr == nullptr){
        if (size + sizeof(size_t) + (sizeof(void*) << 1) <= get_block_size(_memory)){
//            block = *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(_memory) + sizeof(size_t) + sizeof(void*) + sizeof(logger*) + sizeof(memory*));
block = (reinterpret_cast<unsigned char*>(_memory) + sizeof(size_t) + sizeof(void*) + sizeof(logger*) + sizeof(memory*));
auto* now = reinterpret_cast<size_t*>(block);
            *now = size;

            *reinterpret_cast<void**>(now + 1) = nullptr;
            *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(now + 1) + sizeof(void*)) = nullptr;
            *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(_memory) + sizeof(size_t)) = block;
            return reinterpret_cast<unsigned char*>(block) + sizeof(size_t) + (sizeof(void*) << 1);
        }
    }else{
        size_t best_size = 0;
        while(curr != nullptr){
            void* next = get_next_block(curr);
            auto * left = reinterpret_cast<unsigned char*>(curr) + sizeof(size_t) + (sizeof(void*) << 1) + get_block_size(curr);
            auto * right = next == nullptr ? reinterpret_cast<unsigned char*>(_memory) + sizeof(logger*) + sizeof(memory*) + sizeof(void*) + sizeof(size_t) + get_block_size(_memory) : reinterpret_cast<unsigned char*>(next);
            size_t dist = right - left;
            if(size + sizeof(size_t) + (sizeof(void*) << 1) <= dist && dist > best_size){
                best_size = dist;
                block = left;
                previous = curr;
                next_one = next;
            }
            curr = next;
        }
        if (block != nullptr){
            *reinterpret_cast<size_t*>(block) = size;
            *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(block) + sizeof(size_t)) = next_one;
            *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(block) + sizeof(size_t) + sizeof(void*)) = previous;
            *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(previous) + sizeof(size_t)) = block;
            if (next_one != nullptr){
                *reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(next_one) + sizeof(size_t) + sizeof(void*)) = block;
            }
            return reinterpret_cast<unsigned char*>(block) + sizeof(size_t) + (sizeof(void*) << 1);
        }
    }
    return nullptr;
}

