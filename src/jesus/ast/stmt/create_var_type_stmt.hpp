#pragma once

#include "../../types/creation_type.hpp"
#include "../../types/constraints/constraint.hpp"
#include "stmt.hpp"
#include <string>
#include <memory>

/**
 * @brief Represents the creation of a new variable type with a given value.
 * E.g:
 *  create number Positive > 0
 *  create number Negative < 0
 *  create number Percentage > 0 and <=100
 *  create number Percent between 0 and 100
 *
 * "In the beginning God created..." — Genesis 1:1
 */
class CreateVarTypeStmt : public Stmt
{
public:
    std::shared_ptr<CreationType> base_type;
    std::string module_name;
    std::string name;
    std::vector<std::shared_ptr<IConstraint>> constraints;

public:
    CreateVarTypeStmt(std::shared_ptr<CreationType> base_type, std::string name, std::string module_name,
                      std::vector<std::shared_ptr<IConstraint>> constraints)
        : base_type(std::move(base_type)),
          name(std::move(name)),
          module_name(std::move(module_name)),
          constraints(std::move(constraints)) {}

    void accept(StmtVisitor &visitor) const override;

    /**
     * @brief Returns a string representation of the statement.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    std::string toString() const override
    {

        std::string str = "CreateVarTypeStmt(" + name + ")";

        return str;
    }
};
