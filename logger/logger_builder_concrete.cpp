#include "logger_builder_concrete.h"
#include "logger_concrete.h"
#include <map>
#include <fstream>
#include "../include/single_include/nlohmann/json.hpp"

logger_builder *logger_builder_concrete::add_stream(std::string const &path, logger::severity severity) {
    _construction_info[path] = severity;
    return this;
}

logger *logger_builder_concrete::construct() const {
    return new logger_concrete(_construction_info);
}

logger *logger_builder_concrete::conduct(std::string const &path)
{
    std::ifstream file(path);
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    nlohmann::json j = nlohmann::json::parse(str);

    std::map<std::string, logger::severity> lev = {{"trace", logger::severity::trace}, 
    {"debug", logger::severity::debug}, 
    {"information", logger::severity::information}, 
    {"warning", logger::severity::warning}, 
    {"error", logger::severity::error}, 
    {"critical", logger::severity::critical}};

    for(auto& el : j.items()){
        this->add_stream(el.key(), lev[el.value()]);
    }
    file.close();
    return new logger_concrete(_construction_info);
}
