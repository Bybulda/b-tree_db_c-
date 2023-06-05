#ifndef SANDBOX_CPP_MEMORY_HOLDER_H
#define SANDBOX_CPP_MEMORY_HOLDER_H

#include "../memory/memory.h"

class memory_holder
{

public:

    virtual ~memory_holder() noexcept = default;

public:

    void *allocate_with_guard(
        size_t size) const;

    void deallocate_with_guard(
        void *block_pointer) const;

protected:

    virtual memory *get_memory() const noexcept = 0;

};

#endif //SANDBOX_CPP_MEMORY_HOLDER_H
