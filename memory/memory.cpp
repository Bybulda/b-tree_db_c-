#include "memory.h"


void *memory::operator+=(size_t const& size){
    return allocate(size);
}

void memory::operator-=(void const * const object){
    deallocate(object);
}