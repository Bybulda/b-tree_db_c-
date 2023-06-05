//
// Created by smirn on 05.06.2023.
//

#ifndef MAI_LABS_DATABASE_H
#define MAI_LABS_DATABASE_H
#include "../B-tree/b_tree.h"
#include "../associative_container/associative_container.h"
#include "../usefull_files/logger_holder.h"
#include "../logger/logger.h"
#include "../logger/logger_builder_concrete.h"
#include "../logger/logger_builder.h"
#include "../memory/memory_concrete.h"
#include "../usefull_files/memory_holder.h"
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

enum class player_status{
    REGULAR,
    PREMIUM,
    MODERATOR,
    ADMIN
};

struct user_info{
    size_t user_id;
    std::string game_zone;
    player_status status;
    size_t steel;
    size_t gold;
    std::string reg_date;
    size_t time_min;
};

class DataBase:
        private memory_holder,
        private logger_holder
        {
        private:
            enum class commands{
                INSERT_INFO,
                SET_INFO,
                FIND_INFO,
                FIND_IN_INFO,
                DELETE_INFO,
                INSERT_INFO,
                SET_INFO,
                FIND_INFO,
                FIND_IN_INFO,
                DELETE_INFO,
            };

            associative_container<std::string, b_tree<std::string, b_tree<std::string, user_info,
            compare<std::string>>, compare<std::string>>>* pull;

        public:
            DataBase(const std::string& filename);
        private:
            void parse_file(const std::string& filename);
        };
#endif //MAI_LABS_DATABASE_H
