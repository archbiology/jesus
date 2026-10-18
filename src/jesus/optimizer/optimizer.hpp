#pragma once

#include <vector>
#include "ast/stmt/stmt.hpp"

class Optimizer
{
  public:
    /**
     * @brief @param retainClasses keeps class definitions when set.
     *
     * It is turned on only for imported modules, whose classes the importing
     * module still references; the main module leaves it off so its unused
     * classes are removed.
     */
    Optimizer(bool retainClasses = false) : retainClasses(retainClasses) {}

    void optimize(std::vector<std::unique_ptr<Stmt>> &program);

  private:
    bool retainClasses;
};
