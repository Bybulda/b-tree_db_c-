#include <iostream>
#include <sstream>
#include "memory_concrete.h"
#include <map>


memory_concrete::~memory_concrete()= default;


memory_concrete::memory_concrete(logger* lg){
    loggerr = lg;
}

void memory_concrete::set_logger(logger* &lg) noexcept{
    loggerr = lg;
}

void memory_concrete::_log_with_guard(const std::string& str, logger::severity level)const{
    if (loggerr != nullptr){
        loggerr->log(str, level);
    }
}

template<typename T>
std::string memory_concrete::to_str(T const & object) const noexcept
{
    std::stringstream stream;
    stream << object;

    return stream.str();
}


void * memory_concrete::allocate(size_t target_size){
    void* new_mem = ::operator new(target_size + sizeof(size_t));
    
    *reinterpret_cast<size_t*>(new_mem) = target_size;
    _log_with_guard("Allocated size block: " + to_str(target_size), logger::severity::debug);
    new_mem = reinterpret_cast<void*>(reinterpret_cast<size_t*>(new_mem) + 1);
 
    return new_mem;
}


void memory_concrete::deallocate(void const * const target_to_dealloc) const{
    void *ptr = const_cast<void*>(target_to_dealloc);
    
    size_t size_deallocated = *reinterpret_cast<size_t*>(reinterpret_cast<size_t*>(ptr) - 1);
    const unsigned char* trial = static_cast<const unsigned char*>(ptr);
    size_t trace = (size_deallocated > 255) ? 255 : size_deallocated;
    std::string objects = "Object have such data in it: \n";
    for (int i = 0; i < trace - 1; i++){
        objects += " ";
        objects += to_str(static_cast<int>(trial[i]));
    }
    _log_with_guard(objects, logger::severity::debug);
    _log_with_guard("Deallocated block size: " + to_str(size_deallocated) + " of block (" + to_str(ptr) + ")", logger::severity::debug);
    ::operator delete(reinterpret_cast<void*>(reinterpret_cast<size_t*>(ptr) - 1));
}

