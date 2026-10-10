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
class IMethod;              // Forward declaration

using ListValue = std::vector<std::shared_ptr<Value>>;
using DictValue = std::vector<std::pair<std::shared_ptr<Value>, std::shared_ptr<Value>>>;

/**
 * @brief Represents any value in the interpreted language:
 * string, number, boolean, or null (as monostate).
 *
 * `std::monostate` is used to represent "null" or "no value"
 *
 * "then the Lord God formed the man of dust from the ground and breathed into his nostrils the breath of life, and the man became a living creature."
 * — Genesis 2:7
 */
using Dust = std::variant<
    bool,
    int,
    double,
    std::string,
    std::monostate,
    std::shared_ptr<Module>,
    std::shared_ptr<Instance>,
    std::shared_ptr<IMethod>,
    std::shared_ptr<CreationType>,
    std::shared_ptr<ListValue>,
    std::shared_ptr<DictValue>>;

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
    bool IS_DICT = false;
    bool IS_MODULE = false;
    bool IS_CLASS = false;
    bool IS_INSTANCE = false;
    bool IS_METHOD = false;

    bool AS_BOOLEAN = false;

    /**
     * @brief Cached numeric value, filled in when the Value is constructed.
     *
     * Reading the number is the hottest operation in the VM's arithmetic,
     * and reaching it through std::variant (std::get) is measurably the
     * dominant cost. Values are effectively immutable once constructed,
     * so the numeric payload can be cached here and copied for free.
     */
    double number = 0.0;

    /**
     * Cached integer payload for IS_INT values. Like `number`, it is stored
     * outside the variant so integer copies never touch the variant.
     */
    int intValue = 0;

    Value() : IS_FORMLESS(true), AS_BOOLEAN(false) {}
    explicit Value(std::monostate) : IS_FORMLESS(true), AS_BOOLEAN(false) {}
    explicit Value(int v) : IS_INT(true), IS_NUMBER(true), AS_BOOLEAN(v != 0), number(static_cast<double>(v)), intValue(v) {}
    explicit Value(double v) : IS_DOUBLE(true), IS_NUMBER(true), AS_BOOLEAN(v != 0.0), number(v), intValue(static_cast<int>(v)) {}
    explicit Value(bool v) : IS_BOOLEAN(true), AS_BOOLEAN(v) {}
    explicit Value(const std::string &v) : IS_STRING(true), AS_BOOLEAN(v != ""), value(v) {}
    explicit Value(const char *v) : IS_STRING(true), AS_BOOLEAN(std::string(v) != ""), value(std::string(v)) {}
    explicit Value(const std::shared_ptr<Module> &v) : IS_MODULE(true), AS_BOOLEAN(true), value(v) {}
    explicit Value(const std::shared_ptr<CreationType> &v) : IS_CLASS(true), AS_BOOLEAN(true), value(v) {}
    explicit Value(const std::shared_ptr<Instance> &v) : IS_INSTANCE(true), AS_BOOLEAN(static_cast<bool>(v)), value(v) {}
    explicit Value(const std::shared_ptr<IMethod> &v) : IS_METHOD(true), AS_BOOLEAN(true), value(v) {}
    explicit Value(const std::shared_ptr<ListValue> &v) : IS_LIST(true), AS_BOOLEAN(!v->empty()), value(std::move(v)) {}
    explicit Value(const std::shared_ptr<DictValue> &v) : IS_DICT(true), AS_BOOLEAN(!v->empty()), value(std::move(v)) {}

    /**
     * @brief True when this Value carries a scalar payload (null/bool/int/
     * double) that lives in the cached fields rather than inside `value`.
     *
     * Scalars deliberately keep the variant default-initialised (a trivial
     * bool), so copying/destroying them never runs the variant's visitor.
     */
    bool storesAsScalar() const
    {
        return IS_NUMBER || IS_BOOLEAN || IS_FORMLESS;
    }

    Value(const Value &other)
        : IS_FORMLESS(other.IS_FORMLESS), IS_INT(other.IS_INT), IS_DOUBLE(other.IS_DOUBLE),
          IS_NUMBER(other.IS_NUMBER), IS_BOOLEAN(other.IS_BOOLEAN), IS_STRING(other.IS_STRING),
          IS_LIST(other.IS_LIST), IS_DICT(other.IS_DICT), IS_MODULE(other.IS_MODULE),
          IS_CLASS(other.IS_CLASS), IS_INSTANCE(other.IS_INSTANCE), IS_METHOD(other.IS_METHOD),
          AS_BOOLEAN(other.AS_BOOLEAN), number(other.number), intValue(other.intValue)
    {
        if (!other.storesAsScalar())
            value = other.value;
    }

    Value(Value &&other) noexcept
        : IS_FORMLESS(other.IS_FORMLESS), IS_INT(other.IS_INT), IS_DOUBLE(other.IS_DOUBLE),
          IS_NUMBER(other.IS_NUMBER), IS_BOOLEAN(other.IS_BOOLEAN), IS_STRING(other.IS_STRING),
          IS_LIST(other.IS_LIST), IS_DICT(other.IS_DICT), IS_MODULE(other.IS_MODULE),
          IS_CLASS(other.IS_CLASS), IS_INSTANCE(other.IS_INSTANCE), IS_METHOD(other.IS_METHOD),
          AS_BOOLEAN(other.AS_BOOLEAN), number(other.number), intValue(other.intValue)
    {
        if (!other.storesAsScalar())
            value = std::move(other.value);
    }

    Value &operator=(const Value &other)
    {
        if (this == &other)
            return *this;

        IS_FORMLESS = other.IS_FORMLESS;
        IS_INT = other.IS_INT;
        IS_DOUBLE = other.IS_DOUBLE;
        IS_NUMBER = other.IS_NUMBER;
        IS_BOOLEAN = other.IS_BOOLEAN;
        IS_STRING = other.IS_STRING;
        IS_LIST = other.IS_LIST;
        IS_DICT = other.IS_DICT;
        IS_MODULE = other.IS_MODULE;
        IS_CLASS = other.IS_CLASS;
        IS_INSTANCE = other.IS_INSTANCE;
        IS_METHOD = other.IS_METHOD;
        AS_BOOLEAN = other.AS_BOOLEAN;
        number = other.number;
        intValue = other.intValue;

        if (other.storesAsScalar())
            value = std::monostate{};
        else
            value = other.value;

        return *this;
    }

    Value &operator=(Value &&other) noexcept
    {
        if (this == &other)
            return *this;

        IS_FORMLESS = other.IS_FORMLESS;
        IS_INT = other.IS_INT;
        IS_DOUBLE = other.IS_DOUBLE;
        IS_NUMBER = other.IS_NUMBER;
        IS_BOOLEAN = other.IS_BOOLEAN;
        IS_STRING = other.IS_STRING;
        IS_LIST = other.IS_LIST;
        IS_DICT = other.IS_DICT;
        IS_MODULE = other.IS_MODULE;
        IS_CLASS = other.IS_CLASS;
        IS_INSTANCE = other.IS_INSTANCE;
        IS_METHOD = other.IS_METHOD;
        AS_BOOLEAN = other.AS_BOOLEAN;
        number = other.number;
        intValue = other.intValue;

        if (other.storesAsScalar())
            value = std::monostate{};
        else
            value = std::move(other.value);

        return *this;
    }

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
        if (IS_NUMBER)
        {
            return number;
        }

        throw std::runtime_error("Value is not a number");
    }

    /**
     * @brief Converts this Value in place into a boolean, keeping the
     * existing object identity on the VM stack (no variant re-construction).
     */
    void setAsBoolean(bool v)
    {
        value = std::monostate{};
        number = 0.0;
        intValue = 0;
        IS_FORMLESS = false;
        IS_INT = false;
        IS_DOUBLE = false;
        IS_NUMBER = false;
        IS_BOOLEAN = true;
        AS_BOOLEAN = v;
    }

    /**
     * @brief Overwrites this Value in place with an integer result.
     */
    void setAsInt(int v)
    {
        value = std::monostate{};
        number = static_cast<double>(v);
        intValue = v;
        IS_FORMLESS = false;
        IS_BOOLEAN = false;
        IS_DOUBLE = false;
        IS_INT = true;
        IS_NUMBER = true;
        AS_BOOLEAN = (v != 0);
    }

    /**
     * @brief Overwrites this Value in place with a double result.
     */
    void setAsDouble(double v)
    {
        value = std::monostate{};
        number = v;
        intValue = static_cast<int>(v);
        IS_FORMLESS = false;
        IS_BOOLEAN = false;
        IS_INT = false;
        IS_DOUBLE = true;
        IS_NUMBER = true;
        AS_BOOLEAN = (v != 0.0);
    }

    int toInt() const
    {
        // if (IS_NUMBER)
        // {
            return (int)number;
        // }

        // throw std::runtime_error("Value is not a number");
    }

    const std::shared_ptr<Instance> toInstance() const;
    const std::shared_ptr<IMethod> asMethod() const;

    std::string toString() const;

    ListValue &asList()
    {
        if (!IS_LIST)
            throw std::runtime_error("This value is not a list.");

        return *std::get<std::shared_ptr<ListValue>>(value);
    }

    const ListValue &asList() const
    {
        if (!IS_LIST)
            throw std::runtime_error("This value is not a list.");

        return *std::get<std::shared_ptr<ListValue>>(value);
    }

    DictValue &asDict()
    {
        if (!IS_DICT)
            throw std::runtime_error("This value is not a dict.");

        return *std::get<std::shared_ptr<DictValue>>(value);
    }

    const DictValue &asDict() const
    {
        if (!IS_DICT)
            throw std::runtime_error("This value is not a dict.");

        return *std::get<std::shared_ptr<DictValue>>(value);
    }

    std::shared_ptr<CreationType> asClass() const
    {
        return std::get<std::shared_ptr<CreationType>>(value);
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

            return Value(left.intValue + right.intValue);
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

            return Value(left.intValue - right.intValue);
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
            return Value(left.intValue * right.intValue);
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

    friend bool operator==(const Value &left, const Value &right);

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
    friend class VM;
    Dust value;
};
