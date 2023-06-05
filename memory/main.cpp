#include "memory.h"
#include "memory_concrete.h"
#include <iostream>



int main(){
    memory *t = new memory_concrete();
    logger_builder* build = new logger_builder_concrete();
    logger* logg = build->add_stream("logs.log", logger::severity::trace)->construct();
    logger* &lg = logg;
    t->set_logger(lg);
    int* p = reinterpret_cast<int*>(t->allocate(sizeof(int) * 8));
    p[0] = 10;
    p[1] = 20;
    t->deallocate(p);
    char* d = reinterpret_cast<char*>(*t += sizeof(char)*3);
    d[0] = 'h';
    d[1] = 'u';
    d[2] = 'i';
    *t -= d;
    delete build;
    delete logg;
    delete t;
    return 0;
}