#pragma once
#include "grammar_rule.hpp"
#include "combinators/or_rule.hpp"
#include "combinators/sequence_rule.hpp"

// operator overloads
inline std::shared_ptr<IGrammarRule> operator|(
    std::shared_ptr<IGrammarRule> a,
    std::shared_ptr<IGrammarRule> b)
{
    return std::make_shared<OrRule>(a, b);
}

inline std::shared_ptr<IGrammarRule> operator>>(
    std::shared_ptr<IGrammarRule> a,
    std::shared_ptr<IGrammarRule> b)
{
    return std::make_shared<SequenceRule>(a, b);
}
