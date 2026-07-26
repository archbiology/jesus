#pragma once

#include <vector>
#include "ast/stmt/stmt.hpp"

class Optimizer
{
  public:
    void optimize(std::vector<std::unique_ptr<Stmt>> &program);
};
