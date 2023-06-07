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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <utility>

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

struct user_info{
    size_t user_id;
    std::string game_zone;
    std::string status;
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
//            enum class commands{
//                CREATE_PULL,
//                CREATE_SCHEMA,
//                CREATE_COLLECTION,
//                INSERT_DATA,
//                SET_DATA,
//                GET_DATA,
//                GET_FROM_TO,
//                DELETE_PULL,
//                DELETE_SCHEMA,
//                DELETE_COLLECTION,
//                DELETE_DATA
//            };

            b_tree<std::string, b_tree<std::string, b_tree<std::string, b_tree<int, user_info*, compare<int>>*,
            compare<std::string>>*, compare<std::string>>*, compare<std::string>>* pool_collector;
            std::string filename_;
            memory* allocator;
            logger* logg;

        private:
            b_tree<std::string, b_tree<std::string, b_tree<int, user_info*, compare<int>>*,
            compare<std::string>>*, compare<std::string>>* pool_init(){
                auto mem = allocate_with_guard(sizeof(b_tree<std::string, b_tree<std::string, b_tree<int, user_info*, compare<int>>*,
                        compare<std::string>>*, compare<std::string>>));
                auto new_mem = reinterpret_cast<b_tree<std::string, b_tree<std::string, b_tree<int, user_info*, compare<int>>*,
                        compare<std::string>>*, compare<std::string>>*>(mem);
                new (new_mem) b_tree<std::string, b_tree<std::string, b_tree<int, user_info*, compare<int>>*,
                        compare<std::string>>*, compare<std::string>>(3);
                return new_mem;
            }

            b_tree<std::string, b_tree<int, user_info*, compare<int>>*,
            compare<std::string>>* schema_init(){
                auto mem = allocate_with_guard(sizeof(b_tree<std::string, b_tree<int, user_info*, compare<int>>*,
                        compare<std::string>>));
                auto new_mem = reinterpret_cast<b_tree<std::string, b_tree<int, user_info*, compare<int>>*,
                        compare<std::string>>*>(mem);
                new (new_mem) b_tree<std::string, b_tree<int, user_info*, compare<int>>*,
                        compare<std::string>>(3);
                return new_mem;
            }

            b_tree<int, user_info*, compare<int>>* collection_init(){
                auto mem = allocate_with_guard(sizeof(b_tree<int, user_info, compare<int>>));
                auto new_mem = reinterpret_cast<b_tree<int, user_info*, compare<int>>*>(mem);
                new (new_mem) b_tree<int, user_info*, compare<int>>(2);
                return new_mem;
            }

            user_info* init_info(size_t id, const std::string& gameZone, const std::string& status,
                                 size_t stell, size_t gold, const std::string& regDate, size_t timeInGame){
                auto mem = allocate_with_guard(sizeof(user_info));
                auto new_mem = reinterpret_cast<user_info*>(mem);
                new (new_mem) user_info{id, gameZone, status,
                stell, gold, regDate, timeInGame};
                return new_mem;
            }

            std::string fix_date(std::string& input){
                std::stringstream in(input);
                int day, month, year;
                char ignore;
                if (in >> day >> ignore >> month >> ignore >> year)
                {
                    std::stringstream out;
                    out << std::setfill('0') << std::setw(2) << day << '/' << month << '/' <<  std::setw(4) << year;
                    return out.str();
                }
                return std::string();
            }

        public:
            explicit DataBase(std::string filename, logger* logs = nullptr, memory* alloc = nullptr): filename_(std::move(filename)), logg(logs), allocator(alloc), pool_collector(
                    nullptr){
                auto mem = allocate_with_guard(sizeof(b_tree<std::string, b_tree<std::string, b_tree<std::string, b_tree<int, user_info*, compare<int>>*,
                        compare<std::string>>*, compare<std::string>>*, compare<std::string>>));
                auto new_mem = reinterpret_cast<b_tree<std::string, b_tree<std::string, b_tree<std::string, b_tree<int, user_info*, compare<int>>*,
                        compare<std::string>>*, compare<std::string>>*, compare<std::string>>*>(mem);
                new (new_mem) b_tree<std::string, b_tree<std::string, b_tree<std::string, b_tree<int, user_info*, compare<int>>*,
                        compare<std::string>>*, compare<std::string>>*, compare<std::string>>(4);
                pool_collector = new_mem;
            }
            ~DataBase() override{
                deallocate_with_guard(pool_collector);
            }

            void parseFile() {
                std::ifstream file(filename_);
                std::string line;
                std::vector<user_info*> insert_info;

                while (std::getline(file, line)) {
                    std::istringstream iss(line);
                    std::string command;
                    iss >> command;

                    if (command == "CREATE_POOL") {
                        std::string poolName;
                        iss >> poolName;
                        insertPool(poolName);
                    } else if (command == "DELETE_POOL") {
                        std::string poolName;
                        iss >> poolName;
                        deletePool(poolName);
                    } else if (command == "CREATE_SCHEMA") {
                        std::string schemaName, poolName;
                        iss >> schemaName >> poolName;
                        insertSchema(schemaName, poolName);
                    } else if (command == "DELETE_SCHEMA") {
                        std::string schemaName, poolName;
                        iss >> schemaName >> poolName;
                        deleteSchema(schemaName, poolName);
                    } else if (command == "CREATE_COLLECTION") {
                        std::string collectionName, poolName, schemaName;
                        iss >> collectionName >> schemaName >> poolName;
                        insertCollection(collectionName, schemaName, poolName);
                    } else if (command == "DELETE_COLLECTION") {
                        std::string collectionName, poolName, schemaName;
                        iss >> collectionName >> schemaName >> poolName;
                        deleteCollection(collectionName, schemaName, poolName);
                    } else if (command == "INSERT_DATA") {
                        std::string poolName, schemaName, collectionName, gameZone, status, regDate;
                        int id, stell, gold, timeInGame;
                        iss >> collectionName >> schemaName >> poolName >> id >> gameZone >> status
                            >> stell >> gold >> regDate >> timeInGame;
                        insertInfo(poolName, schemaName, collectionName, id, gameZone, status, stell,
                                   gold, regDate, timeInGame);
                    } else if (command == "DELETE_DATA") {
                        std::string poolName, schemaName, collectionName;
                        int id;
                        iss >> collectionName >> schemaName >> poolName >> id;
                        deleteInfo(poolName, schemaName, collectionName, id);
                    } else if (command == "GET_DATA") {
                        std::string poolName, schemaName, collectionName;
                        int id;
                        iss >> collectionName >> schemaName >> poolName >> id;
                        auto curr_info =  getInfo(poolName, schemaName, collectionName, id);
                        if (curr_info != nullptr){
                            std::cout << curr_info->user_id <<  " " << curr_info->status << " " << " " << curr_info->game_zone << " " << std::endl;
                        }
                        else{
                            std::cout << "NO SUCH DATA" << std::endl;
                        }
                    } else if (command == "SET_DATA") {
                        std::string poolName, schemaName, collectionName, gameZone, status, regDate;
                        int id, stell, gold, timeInGame;
                        iss >> collectionName >> schemaName >> poolName >> id >> gameZone >> status
                            >> stell >> gold >> regDate >> timeInGame;
                        setInfo(poolName, schemaName, collectionName, id, gameZone, status, stell,
                                gold, regDate, timeInGame);
                    } else if (command == "FIND_DIAP") {
                        std::string poolName, schemaName, collectionName;
                        int start, end;
                        iss >> collectionName >> schemaName >> poolName >> start >> end;
                        auto lst = findDiap(poolName, schemaName, collectionName, start, end);
                        if (lst.empty()){
                            std::cout << "DID NOT FOUND VALUES IN DIAP " << start << " TO " << end << std::endl;
                        }
                        else{
                            std::cout << "INFO IN DIAP " << std::endl;
                            auto iter = lst.begin();
                            while(iter != lst.end()){
                                std::cout << (*iter)->user_id << " " << (*iter)->game_zone << " "
                                        << (*iter)->status << " " << (*iter)->steel << " "
                                        << (*iter)->gold << " " << (*iter)->reg_date << " "
                                        << (*iter)->time_min << " " << std::endl;
                                iter++;
                            }
                        }
                    }
                }
                file.close();
            }

        private:
            void insertPool(const std::string& poolName) {
                std::cout << "CREATE_POOL: " << poolName << std::endl;
                try {
                    pool_collector->get(poolName);
                }
                catch (std::runtime_error& e) {
                    auto schema = pool_init();
                    pool_collector->insert(poolName, std::move(schema));
                }
            }

            void deletePool(const std::string& poolName) {
                std::cout << "DELETE_POOL: " << poolName << std::endl;
                try {
                    pool_collector->get(poolName);
                    pool_collector->remove(poolName);
                }
                catch (std::runtime_error& e) {
                    std::cout << "NO SUCH POOL CAN NOT REMOVE" << std::endl;
                }
            }

            void insertSchema(const std::string& schemaName, const std::string& poolName) {
                std::cout << "CREATE_SCHEMA: " << schemaName << std::endl;
                try {
                    auto curr_pool = pool_collector->get(poolName);
                    auto schema_ = schema_init();
                    curr_pool->insert(schemaName, std::move(schema_));
                }
                catch (std::runtime_error& e) {
                    std::cout << "NO SUCH POOL, CAN NOT INSERT SCHEMA" << std::endl;
                }
            }

            void deleteSchema(const std::string& schemaName, const std::string& poolName) {
                std::cout << "DELETE_SCHEMA: " << schemaName << std::endl;
                try {
                    auto curr_pool = pool_collector->get(poolName);
                    auto curr_schem = curr_pool->get(schemaName);
                    curr_pool->remove(schemaName);
                }
                catch (std::runtime_error& e) {
                    std::cout << "NO SUCH POOL OR SCHEME, CAN NOT DELETE SCHEMA" << std::endl;
                }
            }

            void insertCollection(const std::string& collectionName, const std::string& schemaName, const std::string& poolName) {
                std::cout << "CREATE_COLLECTION: " << collectionName << std::endl;
                try {
                    auto curr_pool = pool_collector->get(poolName);
                    auto curr_schem = curr_pool->get(schemaName);
                    try {
                        curr_schem->get(collectionName);
                        std::cout << "NO SUCH POOL OR SCHEME, CAN NOT INSERT COLLECTION" << std::endl;
                    }
                    catch (std::runtime_error& e) {
                        auto new_coll = collection_init();
                        curr_schem->insert(collectionName, std::move(new_coll));
                    }
                }
                catch (std::runtime_error& e) {
                    std::cout << "NO SUCH POOL OR SCHEME, CAN NOT INSERT COLLECTION" << std::endl;
                }
            }

            void deleteCollection(const std::string& collectionName, const std::string& schemaName, const std::string& poolName) {
                std::cout << "DELETE_COLLECTION: " << collectionName << std::endl;
                try {
                    auto curr_pool = pool_collector->get(poolName);
                    auto curr_schem = curr_pool->get(schemaName);
                    curr_schem->get(collectionName);
                    curr_schem->remove(collectionName);
                }
                catch (std::runtime_error& e) {
                    std::cout << "NO SUCH POOL OR SCHEME, CAN NOT INSERT COLLECTION" << std::endl;
                }
            }

            void insertInfo(const std::string& poolName, const std::string& schemaName, const std::string& collectionName,
                            size_t id, const std::string& gameZone, const std::string& status,
                            size_t stell, size_t gold, const std::string& regDate, size_t timeInGame) {
                std::cout << "INSERT_INFO: " << poolName << ", " << schemaName << ", " << collectionName << ", "
                          << ", " << id << ", " << gameZone << ", " << status << ", " << stell << ", "
                          << gold << ", " << regDate << ", " << timeInGame << std::endl;
                try {
                    auto curr_pool = pool_collector->get(poolName);
                    auto curr_schem = curr_pool->get(schemaName);
                    auto cr_coll = curr_schem->get(collectionName);
                    try {
                        cr_coll->get(id);
                        std::cout << "THE CONTAINER IS IN COLLECTION, CAN NOT INSERT" << std::endl;
                    }
                    catch (std::runtime_error& e) {
                        auto get_us_info = init_info(id, gameZone, status, stell, gold, regDate, timeInGame);
                        cr_coll->insert(id, std::move(get_us_info));
                        std::cout << "INSERTED USER WITH ID " << id << " TO POOL "
                        << poolName << " IN SCHEMA " << schemaName << " IN COLLECTION " << collectionName << std::endl;
                    }
                }
                catch (std::runtime_error& e) {
                    std::cout << "NO SUCH POOL OR SCHEME, CAN NOT INSERT COLLECTION" << std::endl;
                }
            }

            void deleteInfo(const std::string& poolName, const std::string& schemaName, const std::string& collectionName,
                            int id) {
                std::cout << "DELETE_INFO: " << poolName << ", " << schemaName << ", " << collectionName << ", "
                          << id << std::endl;
                try {
                    auto curr_pool = pool_collector->get(poolName);
                    auto curr_schem = curr_pool->get(schemaName);
                    auto curr_col = curr_schem->get(collectionName);
                    auto cur_data = curr_col->get(id);
                    curr_col->remove(id);
                }
                catch (std::runtime_error& e) {
                    std::cout << "NO SUCH POOL OR SCHEME OR COLLECTION OR DATA IS ALREADY SET, CAN NOT INSERT DATA" << std::endl;
                }
            }

            user_info* getInfo(const std::string& poolName, const std::string& schemaName, const std::string& collectionName,
                         int id) {
                std::cout << "GET_INFO: " << poolName << ", " << schemaName << ", " << collectionName << ", " << std::endl;
                try {
                    auto curr_pool = pool_collector->get(poolName);
                    auto curr_schem = curr_pool->get(schemaName);
                    auto curr_col = curr_schem->get(collectionName);
                    auto cur_data = curr_col->get(id);
                    std::cout << "DATA WAS ABTAINED FROM POOL " << poolName << " IN SCHEMA " << schemaName
                    << " FROM COLLECTION " << collectionName << " BY ID " << id << std::endl;
                    return reinterpret_cast<user_info *>(cur_data);
                }
                catch (std::runtime_error& e) {
                    std::cout << "NO SUCH POOL OR SCHEME OR COLLECTION OR DATA IS ALREADY SET, CAN NOT INSERT DATA" << std::endl;
                    return nullptr;
                }
            }

            void setInfo(const std::string& poolName, const std::string& schemaName, const std::string& collectionName,
                         size_t id, const std::string& gameZone, const std::string& status,
                         size_t stell, size_t gold, const std::string& regDate, size_t timeInGame) {
                std::cout << "SET_INFO: " << poolName << ", " << schemaName << ", " << collectionName << ", "
                          << ", " << id << ", " << gameZone << ", " << status << ", " << stell << ", "
                          << gold << ", " << regDate << ", " << timeInGame << std::endl;
                try {
                    auto curr_pool = pool_collector->get(poolName);
                    auto curr_schem = curr_pool->get(schemaName);
                    auto curr_col = curr_schem->get(collectionName);
                    auto cur_data = curr_col->get(id);
                    auto new_info = init_info(id, gameZone, status, stell, gold, regDate, timeInGame);
                    curr_col->set(id, std::move(new_info));
                    std::cout << "DATA WAS SET TO POOL " << poolName << " IN SCHEMA " << schemaName
                              << " FROM COLLECTION " << collectionName << " BY ID " << id << std::endl;
                }
                catch (std::runtime_error& e) {
                    std::cout << "NO SUCH POOL OR SCHEME OR COLLECTION OR DATA IS ALREADY SET, CAN NOT INSERT DATA" << std::endl;
                }
            }

            std::list<user_info*> findDiap(const std::string& poolName, const std::string& schemaName, const std::string& collectionName,
                          int start, int end) {
                std::cout << "FIND_DIAP: " << poolName << ", " << schemaName << ", " << collectionName << ", "
                          << start << ", " << end << std::endl;
                try {
                    auto curr_pool = pool_collector->get(poolName);
                    auto curr_schem = curr_pool->get(schemaName);
                    auto curr_col = curr_schem->get(collectionName);
                    auto re_list = curr_col->find_diap(start, end);
                    std::cout << "DATA WAS ABTAINED FROM POOL " << poolName << " IN SCHEMA " << schemaName
                              << " FROM COLLECTION " << collectionName << " BY ID " << start << " AND " << end << std::endl;
                    return re_list;
                }
                catch (std::runtime_error& e) {
                    std::cout << "NO SUCH POOL OR SCHEME OR COLLECTION OR DATA IS ALREADY SET, CAN NOT INSERT DATA" << std::endl;
                    throw std::runtime_error("ERROR");
                }
            }

            memory *get_memory() const noexcept override{
                    return allocator;
            }
            logger *get_logger() const noexcept override{
                return logg;
            }
        };
#endif //MAI_LABS_DATABASE_H
