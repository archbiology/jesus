#pragma once

#include <string>
#include <variant>
#include <iostream>
#include <vector>
#include <memory>

class Value;                // Forward declaration, since it is used on Dust.
struct make_string_functor; // Forward declaration
struct Module;              // Forward declaration
struct CreationType;        // Forward declaration
struct Instance;            // Forward declaration

/**
 * @brief Represents any value in the interpreted language:
 * string, number, boolean, or null (as monostate).
 *
 * `std::monostate` is used to represent "null" or "no value"
 *
 * "then the Lord God formed the man of dust from the ground and breathed into his nostrils the breath of life, and the man became a living creature."
 * — Genesis 2:7
 */
using Dust = std::variant<std::vector<std::shared_ptr<Value>>, std::string, double, int, bool, std::monostate, std::shared_ptr<Module>, std::shared_ptr<CreationType>, std::shared_ptr<Instance>>; // or std::unique_ptr<Value>, etc.

class Value
{
public:
    bool IS_FORMLESS = false;
    bool IS_INT = false;
    bool IS_DOUBLE = false;
    bool IS_NUMBER = false;
    bool IS_BOOLEAN = false;
    bool IS_STRING = false;
    bool IS_LIST = false;
    bool IS_MODULE = false;
    bool IS_CLASS = false;
    bool IS_INSTANCE = false;

    bool AS_BOOLEAN = false;

    Value() : value(std::monostate{}), IS_FORMLESS(true), AS_BOOLEAN(false) {}
    explicit Value(std::monostate) : value(std::monostate{}), IS_FORMLESS(true), AS_BOOLEAN(false) {}
    explicit Value(int v) : value(v), IS_INT(true), IS_NUMBER(true), AS_BOOLEAN(v != 0) {}
    explicit Value(double v) : value(v), IS_DOUBLE(true), IS_NUMBER(true), AS_BOOLEAN(v != 0.0) {}
    explicit Value(bool v) : value(v), IS_BOOLEAN(true), AS_BOOLEAN(v) {}
    explicit Value(const std::string &v) : value(v), IS_STRING(true), AS_BOOLEAN(v != "") {}
    explicit Value(const char *v) : value(std::string(v)), IS_STRING(true), AS_BOOLEAN(std::string(v) != "") {}
    explicit Value(const std::vector<std::shared_ptr<Value>> &v) : value(v), IS_LIST(true), AS_BOOLEAN(!v.empty()) {}
    explicit Value(const std::shared_ptr<Module> &v) : value(std::move(v)), IS_MODULE(true), AS_BOOLEAN(true) {}
    explicit Value(const std::shared_ptr<CreationType> &v): value(std::move(v)), IS_CLASS(true), AS_BOOLEAN(true) {}
    explicit Value(const std::shared_ptr<Instance> &v) : value(v), IS_INSTANCE(true), AS_BOOLEAN(static_cast<bool>(v)) {}

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

        if (IS_DOUBLE)
        {
            return std::get<double>(value);
        }

        throw std::runtime_error("Value is not a number");
    }

    int toInt() const
    {
        return (int)toNumber();
    }

    const std::shared_ptr<Instance> toInstance() const;

    std::string toString() const;

    const std::vector<std::shared_ptr<Value>> &asList() const
    {
        if (!IS_LIST)
            throw std::runtime_error("This value is not a list.");

        return std::get<std::vector<std::shared_ptr<Value>>>(value);
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

    friend Value operator%(const Value &left, const Value &right)
    {
        if (left.IS_NUMBER && right.IS_NUMBER)
        {
            if (left.IS_DOUBLE || right.IS_DOUBLE)
                throw std::runtime_error("'" + std::to_string(left.toNumber()) + " mod " + std::to_string(right.toNumber()) + "' invalid — 'mod' only supports integers.");

            return Value(left.toInt() % right.toInt());
        }

        // FIXME: Detect at PARSE time if numbers are 'int'
        throw std::runtime_error("Operator 'mod' not supported for given types");
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

    /**
     * @brief Value + std::string
     */
    friend std::string operator+(const Value &val, const std::string &str)
    {
        return val.toString() + str;
    }

    /**
     * @brief std::string + Value
     */
    friend std::string operator+(const std::string &str, const Value &val)
    {
        return str + val.toString();
    }

    /**
     * @brief const char* + Value
     */
    friend std::string operator+(const char *str, const Value &val)
    {
        return std::string(str) + val.toString();
    }

    /**
     * @brief Value + const char*
     */
    friend std::string operator+(const Value &val, const char *str)
    {
        return val.toString() + std::string(str);
    }

private:
    Dust value;
};
