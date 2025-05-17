#pragma once
#include <string>
#include <variant>


/**
 * @brief Represents any value in the interpreted language:
 * string, number, boolean, or null (as monostate).
 *
 * `std::monostate` is used to represent "null" or "no value"
 *
 * "The words of the Lord are pure words, like silver refined in a furnace..."
 * — Psalm 12:6
 */
using Value = std::variant<std::string, double, int, bool, std::monostate>;


struct make_string_functor
{
    std::string operator()(const std::string &x) const { return "(text) " + x; }
    std::string operator()(double x) const { return "(double) " + std::to_string(x); }
    std::string operator()(int x) const { return "(int) " + std::to_string(x); }
    std::string operator()(bool x) const { return "(logic) " + std::to_string(x); }
    std::string operator()(std::monostate x) const { return "null"; }
};