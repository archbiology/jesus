#pragma once

#include "stmt.hpp"
#include "../../spirit/heart.hpp"
#include <string>
#include <vector>
#include <memory>

REGISTER_FOR_UML(
    CreateMethodStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"})
        .fieldsList({"name", "params", "returnType", "body"}));

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
    std::shared_ptr<CreationType> returnType;
    std::vector<std::unique_ptr<Stmt>> body;
    bool isConstructor; // true if this is the special constructor "__alpha__"
    bool isDestructor;  // true if this is the special destructor "__omega__"

    /**
     * @brief Parameters prefixed with private/protected/public
     * in the constructor become instance attributes
     * Each entry is (name, access)
     */
    std::vector<std::pair<std::string, std::string>> attributeNames;

    CreateMethodStmt(const std::string &name,
                     const std::shared_ptr<Heart> &params,
                     const std::shared_ptr<CreationType> &returnType,
                     std::vector<std::unique_ptr<Stmt>> body,
                     bool isConstructor = false,
                     bool isDestructor = false,
                     const std::vector<std::pair<std::string, std::string>> &attributeNames = {})
        : name(name), params(std::move(params)), returnType(std::move(returnType)), body(std::move(body)),
          isConstructor(isConstructor), isDestructor(isDestructor),
          attributeNames(attributeNames)
    {
    }

    /**
     * @brief Returns a string representation of the statement.
     */
    std::string toString() const override
    {
        std::string str = "CreateMethodStmt('" + name + "', body: [";
        bool isFirst = true;

        for (auto &stmt : body)
        {
            if (!isFirst)
                str += ",";

            str += "\n    " + stmt->toString();

            isFirst = false;
        }

        str += "])";

        return str;
    }

    void accept(StmtVisitor &visitor) const override {};
};
