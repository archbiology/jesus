#include "parser.hpp"
#include "ast/let_there_be_node.hpp"
#include "ast/identifier_node.hpp"
#include "ast/value_node.hpp"

std::unique_ptr<ASTNode> parse(const std::vector<Token> &tokens)
{
    const int tokens_count = tokens.size();
    // std::cout<<"Toksn: " << tokens_count << "\n";

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

        // if (tokens_count >= 6) {

        //     std::cout<<"tokens[4].type: '" << tokens[4].type <<"'\n";
        //     std::cout<<"tokens[4].value: '" << tokens[4].value <<"'\n";
        //     std::cout<<"tokens[5].type: '" << tokens[5].type <<"'\n";
        //     std::cout<<"tokens[5].value: '" << tokens[5].value <<"'\n";
        //     std::cout<<"tokens[6].type: '" << tokens[6].type <<"'\n";
        //     std::cout<<"tokens[6].value: '" << tokens[6].value <<"'\n";
        // }

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
