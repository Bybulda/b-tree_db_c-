#include <iostream>
#include <fstream>
#include <sstream>
#include "database.h"

int main(int argc, char* argv[]){
    argc = 2;
    if (argc > 1){
        auto* db = new DataBase("commands.txt");
        db->parseFile();
        delete(db);
    }
    else{
        std::cerr << "NO FILE INCLUDED" << std::endl;
    }
}
