#pragma once
#include "stmt.hpp"
#include <string>
#include <vector>
#include <memory>
#include "../../types/creation_type.hpp"

REGISTER_FOR_UML(
    CreateClassStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"})
        .fieldsList({"name", "module_name", "parent_class", "body"}));

/**
 * @brief Represents the 'creation' of a new class in the Jesus language.
 *
 * In the Jesus language, `let there be` is the equivalent of a "class" in other
 * programming languages. It defines a new user-defined type, initially with
 * an empty body, which can later contain attributes and methods.
 *
 * Usage:
 * @code
 *   let there be Person: amen
 * @endcode
 *
 * "God saw all that He had made, and it was very good..." — Genesis 1:31
 */
class CreateClassStmt : public Stmt
{
public:
    std::string name;
    std::string module_name;
    const std::shared_ptr<CreationType> parent_class;
    std::vector<std::shared_ptr<Stmt>> body;

    CreateClassStmt(const std::string &name, const std::string &module_name,
                    const std::shared_ptr<CreationType> &parent_class,
                    const std::vector<std::shared_ptr<Stmt>> &body)
        : name(name), module_name(module_name),
        parent_class(parent_class),
        body(body) {}

    void accept(StmtVisitor &visitor) const override;
};
