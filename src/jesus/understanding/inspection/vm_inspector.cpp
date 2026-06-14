#include "vm_inspector.hpp"
#include "ast/stmt/inspect_stmt.hpp"
#include "interpreter/interpreter.hpp"

void VmInspector::inspect(Interpreter &, const InspectStmt &)
{
    std::cout << "VM inspection not yet implemented.\n";
}
