#pragma once

#include <string>
#include <variant>
#include <iostream>

/**
 * @brief Represents any value in the interpreted language:
 * string, number, boolean, or null (as monostate).
 *
 * `std::monostate` is used to represent "null" or "no value"
 *
 * "then the Lord God formed the man of dust from the ground and breathed into his nostrils the breath of life, and the man became a living creature."
 * — Genesis 2:7
 */
using Dust = std::variant<std::string, double, int, bool, std::monostate>;

struct make_string_functor
{
    std::string operator()(const std::string &x) const { return "(text) " + x; }
    std::string operator()(double x) const { return "(double) " + std::to_string(x); }
    std::string operator()(int x) const { return "(int) " + std::to_string(x); }
    std::string operator()(bool x) const { return "(logic) " + std::to_string(x); }
    std::string operator()(std::monostate x) const { return "null"; }
};

class Value
{
public:
    bool IS_FORMLESS = false;
    bool IS_INT = false;
    bool IS_DOUBLE = false;
    bool IS_NUMBER = false;
    bool IS_BOOLEAN = false;
    bool IS_STRING = false;

    bool AS_BOOLEAN = false;

    Value() : value(std::monostate{}), IS_FORMLESS(true), AS_BOOLEAN(false) {}
    explicit Value(std::monostate) : value(std::monostate{}), IS_FORMLESS(true), AS_BOOLEAN(false) {}
    explicit Value(int v) : value(v), IS_INT(true), IS_NUMBER(true), AS_BOOLEAN(v != 0) {}
    explicit Value(double v) : value(v), IS_DOUBLE(true), IS_NUMBER(true), AS_BOOLEAN(v != 0.0) {}
    explicit Value(bool v) : value(v), IS_BOOLEAN(true), AS_BOOLEAN(v) {}
    explicit Value(const std::string &v) : value(v), IS_STRING(true), AS_BOOLEAN(v != "") {}
    explicit Value(const char *v) : value(std::string(v)), IS_STRING(true), AS_BOOLEAN(std::string(v) != "") {}

    /**
     * @brief Create a NONE/NULL/monostate Value.
     *
     * "Now the earth was formless and empty, darkness was over the surface of the deep, and the Spirit of God was hovering over the waters."
     * — Genesis 1:2
     *
     * @return Value
     */
    static Value formless()
    {
        return Value(std::monostate{});
    }

    double toNumber() const
    {
        if (IS_INT)
        {
            return std::get<int>(value);
        }

        if (IS_DOUBLE) {
            return std::get<double>(value);
        }

        throw std::runtime_error("Value is not a number");
    }

    bool toBoolean() const
    {
        if (IS_FORMLESS)
            return false;

        if (IS_BOOLEAN)
            return std::get<bool>(value);

        if (IS_NUMBER)
            return std::get<double>(value) != 0;

        if (IS_STRING)
            return std::get<std::string>(value) != "";

        return false;
    }

    std::string toString() const
    {
        return std::visit(make_string_functor(), value);
    }

    /**
     * @brief Output stream overload for Value
     *
     * Uses the value's `toString()` method to produce a readable description.
     * E.g.: std::cout<<value;
     *
     * "Therefore, if anyone is in Christ, he is a new creation. The old has passed away; behold, the new has come." — 2 Corinthians 5:17
     */
    std::ostream &operator<<(std::ostream &os)
    {
        return os << this->toString();
    }

    friend Value operator+(const Value &left, const Value &right)
    {
        if (left.IS_NUMBER && right.IS_NUMBER)
        {
            if (left.IS_DOUBLE || right.IS_DOUBLE)
            {
                // Promote to double if either is double
                return Value(left.toNumber() + right.toNumber());
            }

            return Value(std::get<int>(left.value) + std::get<int>(right.value));
        }

        if (left.IS_STRING || right.IS_STRING)
        {
            return Value(left.toString() + right.toString());
        }

        throw std::runtime_error("Operator '+' not supported for given types");
    }

    friend Value operator-(const Value &left, const Value &right)
    {
        if (left.IS_NUMBER && right.IS_NUMBER)
        {
            if (left.IS_DOUBLE || right.IS_DOUBLE)
            {
                return Value(left.toNumber() - right.toNumber());
            }

            return Value(std::get<int>(left.value) - std::get<int>(right.value));
        }

        throw std::runtime_error("Operator '-' not supported for given types");
    }

    friend Value operator*(const Value &left, const Value &right)
    {
        if (left.IS_NUMBER && right.IS_NUMBER)
        {
            if (left.IS_DOUBLE || right.IS_DOUBLE)
            {
                return Value(left.toNumber() * right.toNumber());
            }
            return Value(std::get<int>(left.value) * std::get<int>(right.value));
        }

        // TODO: Allow STR * NUMBER, like in Python

        throw std::runtime_error("Operator '*' not supported for given types");
    }

    friend Value operator/(const Value &left, const Value &right)
    {
        if (left.IS_NUMBER && right.IS_NUMBER)
        {
            double divisor = right.toNumber();
            if (divisor == 0.0)
            {
                throw std::runtime_error("Division by zero");
            }

            return Value(left.toNumber() / divisor);
        }

        throw std::runtime_error("Operator '/' not supported for given types");
    }

    friend bool operator==(const Value &left, const Value &right)
    {
        if (left.IS_FORMLESS && right.IS_FORMLESS)
            return true;

        if (left.IS_NUMBER && right.IS_NUMBER)
        {
            double leftN = left.toNumber();
            double rightN = right.toNumber();

            bool result = leftN == rightN;

            return result;
        }

        if (left.IS_BOOLEAN && right.IS_BOOLEAN)
        {
            return std::get<bool>(left.value) == std::get<bool>(right.value);
        }

        if (left.IS_STRING && right.IS_STRING)
        {
            return std::get<std::string>(left.value) == std::get<std::string>(right.value);
        }

        return false; // different types are never equal
    }

    friend bool operator<(const Value &left, const Value &right)
    {
        if (left.IS_NUMBER && right.IS_NUMBER)
            return left.toNumber() < right.toNumber();

        if (left.IS_STRING && right.IS_STRING)
            return std::get<std::string>(left.value) < std::get<std::string>(right.value);

        throw std::runtime_error("Cannot compare values of different or unsupported types");
    }

    friend bool operator!=(const Value &left, const Value &right) { return !(left == right); }
    friend bool operator>(const Value &left, const Value &right) { return right < left; }
    friend bool operator<=(const Value &left, const Value &right) { return !(right < left); }
    friend bool operator>=(const Value &left, const Value &right) { return !(left < right); }

private:
    Dust value;
};
