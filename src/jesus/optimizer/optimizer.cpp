#include "optimizer.hpp"
#include "constant_folder.hpp"
#include "constant_propagator.hpp"
#include "dead_code_eliminator.hpp"
#include "method_inliner.hpp"

void Optimizer::optimize(std::vector<std::unique_ptr<Stmt>> &program)
{
    ConstantFolder().run(program);
    ConstantPropagator().run(program);
    ConstantFolder().run(program);
    DeadCodeEliminator().run(program);

    MethodInliner().run(program);

    ConstantFolder().run(program);
    ConstantPropagator().run(program);
    ConstantFolder().run(program);
    DeadCodeEliminator().run(program);
}
