#include "parser.hpp"
#include "ast/let_there_be_node.hpp"
#include "ast/identifier_node.hpp"
#include "ast/value_node.hpp"
#include "ast/reveal_node.hpp"

std::unique_ptr<ASTNode> parse(const std::vector<Token> &tokens)
{
    const int tokens_count = tokens.size();
    if (!tokens_count)
        return nullptr;

    if (tokens[0].value == "reveal")
    {
        if (tokens_count >= 2)
        {
            return std::make_unique<RevealNode>(new IdentifierNode(tokens[1].value));
        }
    }

    if (tokens_count >= 3 &&
        tokens[0].value == "let" &&
        tokens[1].value == "there" &&
        tokens[2].value == "be")
    {
        std::string identifier;
        std::string value;

        if (tokens_count >= 4)
        {
            identifier = tokens[3].value;
        }

        if (tokens_count >= 7 && tokens[4].value == "set" && tokens[5].value == "to")
        {
            value = tokens[6].value;
        }

        auto idNode = std::make_unique<IdentifierNode>(identifier);
        auto valNode = std::make_unique<ValueNode>(value);

        return std::make_unique<LetThereBeNode>(std::move(idNode), std::move(valNode));
    }

    return nullptr; // or an error node in the future
}
