#pragma once

#include <string>
#include <vector>
#include <memory>

class Stmt;

struct HttpRoute
{
    std::string protocol; // protocol: GET/POST
    std::string path;
    std::vector<std::shared_ptr<Stmt>> body;
};
