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
using Value = std::variant<std::string, double, bool, std::monostate>;
