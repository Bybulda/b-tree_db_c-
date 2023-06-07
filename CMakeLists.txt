cmake_minimum_required(VERSION 3.10)

project(MAI_LABS VERSION 1.0)

add_executable(database  db_project/main.cpp B-tree/b_tree.h  db_project/database.h ./logger/logger_builder.cpp ./logger/logger_builder_concrete.cpp
        ./logger/logger.cpp ./logger/logger_concrete.cpp memory/memory.cpp memory/memory_concrete.cpp usefull_files/memory_holder.cpp
        usefull_files/logger_holder.cpp
        )
