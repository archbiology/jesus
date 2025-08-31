#pragma once

#include "stmt.hpp"
#include "../../spirit/heart.hpp"
#include <string>
#include <vector>
#include <memory>

/**
 * @brief Represents a method declaration inside a class.
 *
 * Example in Jesus Language:
 *
 * calling decreaseAge(number newAge):
 *     my age = newAge
 * amen
 *
 * A method holds a name, typed parameters, and a body of statements.
 * Parameters are like seeds planted at invocation — they bring values into
 * the method’s local scope.
 *
 * "Commit to the LORD whatever you do, and he will establish your plans."
 * — Proverbs 16:3
 */
class CreateMethodStmt : public Stmt
{
public:
    struct Param
    {
        std::string type;
        std::string name;

        Param(const std::string &type, const std::string &name)
            : type(type), name(name) {}
    };

    std::string name;
    std::shared_ptr<Heart> params;
    std::vector<std::shared_ptr<Stmt>> body;
    bool isGenesis; ///< true if this is the special constructor "genesis"

    CreateMethodStmt(const std::string &name,
                     //  const std::vector<Param> &params,
                     const std::shared_ptr<Heart> &params,
                     const std::vector<std::shared_ptr<Stmt>> &body,
                     bool isGenesis = false)
        : name(name), params(std::move(params)), body(body), isGenesis(isGenesis)
    {
    }

    /**
     * @brief Returns a string representation of the statement.
     */
    std::string toString() const override
    {
        return "CreateMethodStmt( " + name + " (\n" + params->toString() + "))";
    }

    void accept(StmtVisitor &visitor) const override {};
};
