#include <iostream>
#include "b_tree.h"
#include "../logger/logger_builder_concrete.h"
#include "../descriptor/alloc_types.h"

template<typename T>
class t_type_holder{
    T parameter;

    public:
        explicit t_type_holder(T parameter){
            this->parameter = parameter;
        }
        virtual ~t_type_holder()=default;
        T& get(){
            return parameter;
        }
        void set(T& arg){
            parameter = arg;
        }
};

template<typename T>
class compare{
private:
    int comparer(T const & a, T const & b) const{
        if(a > b){
            return 1;
        }
        if (a < b){
            return -1;
        }
        return 0;
    }
public:
    int operator ()(T const & a, T const &  b) const{
        if(a > b){
            return 1;
        }
        if (a < b){
            return -1;
        }
        return 0;
    }
};

struct name{
    std::string name;
    std::string surname;
};

struct name_holder{
    name* info;
};


int main(){
//    t_type_holder<int> ex = t_type_holder<int>(10);
//    std::cout << ex.get();
    logger_builder* builder = new logger_builder_concrete();
    logger* loggs_tree = builder->add_stream("tree.log", logger::severity::information)->construct();
    logger* loggs_alloc = builder->add_stream("alloc.logs", logger::severity::trace)->construct();
    memory* alloc = new memory_concrete(loggs_alloc);
//    memory* allocator = new alloc_types_descriptor(10000000, nullptr, loggs);
    associative_container<int, std::string>* bTree = new b_tree<int, std::string, compare<int>>(2, alloc, loggs_tree);
    std::string ask = "34", abd = "12", cd = "9";
    int a = 10, b = 12, c = 9;
    compare<std::string> comp;
//    bTree->insert(1, "0");
    for (int i = 0; i < 300; i++) {
        bTree->insert(i, std::to_string(i));
    }
//    for (int i = 300; i < 600; ++i) {
//        bTree->set(i - 300, std::to_string(i));
//    }
    associative_container<int, std::string>* bTree_copy = new b_tree<int, std::string, compare<int>>(std::move(*dynamic_cast<b_tree<int, std::string, compare<int>>*>(bTree)));
    dynamic_cast<b_tree<int, std::string, compare<int>>*>(bTree)->try_iter();
    std::cout << std::endl;
    dynamic_cast<b_tree<int, std::string, compare<int>>*>(bTree_copy)->try_iter();
    delete(bTree);
    delete(bTree_copy);
    delete(alloc);
    delete(loggs_tree);
    delete(loggs_alloc);
    delete(builder);

}