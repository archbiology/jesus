#pragma once
#include "stmt.hpp"
#include <string>
#include <memory>
#include "../../types/creation_type.hpp"

REGISTER_FOR_UML(
    CreateEnumStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"})
        .fieldsList({"name", "module_name", "enumType"}));

class CreateEnumStmt : public Stmt
{
  public:
    std::string name;
    std::string module_name;
    const std::shared_ptr<CreationType> enumType;

    CreateEnumStmt(
        const std::string &name, const std::string &module_name, const std::shared_ptr<CreationType> enumType)
        : name(name), module_name(module_name), enumType(std::move(enumType))
    {
    }

    void accept(StmtVisitor &visitor) const override;

    std::string toString() const override
    {
        return "CreateEnumStmt(name: '" + name + "')";
    }
};
