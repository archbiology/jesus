#pragma once

#include "constraint.hpp"
#include <regex>
#include <stdexcept>
#include <string>

class MatchesRegexConstraint : public IConstraint
{
    std::regex pattern;

public:
    MatchesRegexConstraint(const std::string &regex_pattern) : pattern(regex_pattern)
    {
        validate_regex(regex_pattern);
    }

    void validate_regex(const std::string &regex_pattern)
    {
        try
        {
            pattern = std::regex(regex_pattern);
        }
        catch (const std::regex_error &e)
        {
            throw std::runtime_error("Invalid regex pattern \"" + regex_pattern + "\": " + e.what());
        }
    }

    void validate(const Value &value) const override
    {
        if (!value.IS_STRING)
            throw std::runtime_error("Regex constraint can only be applied to text values");

        const std::string &text = value.toString();

        if (!std::regex_match(text, pattern))
            throw std::runtime_error("Value \"" + text + "\" does not match expected pattern.");
    }
};
