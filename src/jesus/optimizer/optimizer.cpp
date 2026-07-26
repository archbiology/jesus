#include "optimizer.hpp"
#include "constant_folder.hpp"

void Optimizer::optimize(std::vector<std::unique_ptr<Stmt>> &program)
{
    ConstantFolder().run(program);
}
