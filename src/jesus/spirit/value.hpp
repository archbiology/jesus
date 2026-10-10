#pragma once

#include <string>
#include <variant>
#include <iostream>
#include <vector>
#include <memory>
#include <cstdint>

class Value;                // Forward declaration, since it is used on Dust.
struct make_string_functor; // Forward declaration
struct Module;              // Forward declaration
struct CreationType;        // Forward declaration
struct Instance;            // Forward declaration
class IMethod;              // Forward declaration

using ListValue = std::vector<std::shared_ptr<Value>>;
using DictValue = std::vector<std::pair<std::shared_ptr<Value>, std::shared_ptr<Value>>>;

/**
 * @brief Heap payload for "heavy" Values (everything that is not a scalar).
 *
 * Scalars (null, bool, number) are stored directly in Value's cached fields,
 * so a scalar Value never allocates and never touches a std::variant. Only
 * strings, modules, classes, instances, methods, lists and dicts use this
 * payload. Because these are rare on tight numeric hot loops, the variant
 * machinery here stays off the critical path.
 */
struct Dust
{
    std::variant<
        std::string,
        std::shared_ptr<Module>,
        std::shared_ptr<Instance>,
        std::shared_ptr<IMethod>,
        std::shared_ptr<CreationType>,
        std::shared_ptr<ListValue>,
        std::shared_ptr<DictValue>> data;

    Dust(const Dust &) = default;
    Dust(Dust &&) = default;

    template <typename T>
        requires (!std::is_same_v<std::decay_t<T>, Dust>)
    explicit Dust(T &&v) : data(std::forward<T>(v)) {}
};

class Value
{
public:
    /**
     * @brief Type/truthiness flags, packed one bit per flag into a single
     * 16-bit word.
     *
     * The individual names remain directly accessible (e.g. `v.IS_NUMBER`),
     * but every copy/move/assign now transfers all flags with a single 16-bit
     * load/store instead of a dozen separate byte copies. This keeps the
     * per-operand overhead of the VM's value stack low.
     *
     * The anonymous struct-inside-union is a GNU/clang extension, consistent
     * with the computed-goto dispatch the VM already relies on.
     */
    union
    {
        uint16_t __flags;
        struct
        {
            bool IS_FORMLESS : 1 = false;
            bool IS_INT : 1 = false;
            bool IS_DOUBLE : 1 = false;
            bool IS_NUMBER : 1 = false;
            bool IS_BOOLEAN : 1 = false;
            bool IS_STRING : 1 = false;
            bool IS_LIST : 1 = false;
            bool IS_DICT : 1 = false;
            bool IS_MODULE : 1 = false;
            bool IS_CLASS : 1 = false;
            bool IS_INSTANCE : 1 = false;
            bool IS_METHOD : 1 = false;
            bool AS_BOOLEAN : 1 = false;
        };
    };

    // Bit positions of the individual flags inside __flags, in declaration
    // order (GCC/Clang pack bool bit-fields LSB-first). Assigning the whole
    // word at once lets setAs*() update every flag with a single store.
    static constexpr uint16_t F_FORMLESS = 1u << 0;
    static constexpr uint16_t F_INT      = 1u << 1;
    static constexpr uint16_t F_DOUBLE   = 1u << 2;
    static constexpr uint16_t F_NUMBER   = 1u << 3;
    static constexpr uint16_t F_BOOLEAN  = 1u << 4;
    static constexpr uint16_t F_STRING   = 1u << 5;
    static constexpr uint16_t F_LIST     = 1u << 6;
    static constexpr uint16_t F_DICT     = 1u << 7;
    static constexpr uint16_t F_MODULE   = 1u << 8;
    static constexpr uint16_t F_CLASS    = 1u << 9;
    static constexpr uint16_t F_INSTANCE = 1u << 10;
    static constexpr uint16_t F_METHOD   = 1u << 11;
    static constexpr uint16_t F_ASBOOL   = 1u << 12;

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
    explicit Value(const std::string &v) : IS_STRING(true), AS_BOOLEAN(v != ""), value(std::make_unique<Dust>(v)) {}
    explicit Value(const char *v) : IS_STRING(true), AS_BOOLEAN(std::string(v) != ""), value(std::make_unique<Dust>(std::string(v))) {}
    explicit Value(const std::shared_ptr<Module> &v) : IS_MODULE(true), AS_BOOLEAN(true), value(std::make_unique<Dust>(v)) {}
    explicit Value(const std::shared_ptr<CreationType> &v) : IS_CLASS(true), AS_BOOLEAN(true), value(std::make_unique<Dust>(v)) {}
    explicit Value(const std::shared_ptr<Instance> &v) : IS_INSTANCE(true), AS_BOOLEAN(static_cast<bool>(v)), value(std::make_unique<Dust>(v)) {}
    explicit Value(const std::shared_ptr<IMethod> &v) : IS_METHOD(true), AS_BOOLEAN(true), value(std::make_unique<Dust>(v)) {}
    explicit Value(const std::shared_ptr<ListValue> &v) : IS_LIST(true), AS_BOOLEAN(!v->empty()), value(std::make_unique<Dust>(v)) {}
    explicit Value(const std::shared_ptr<DictValue> &v) : IS_DICT(true), AS_BOOLEAN(!v->empty()), value(std::make_unique<Dust>(v)) {}

    Value(const Value &other)
        : __flags(other.__flags), number(other.number), intValue(other.intValue)
    {
        // Scalars leave `value` null; only heavy payloads are cloned.
        if (other.value)
            value = std::make_unique<Dust>(*other.value);
    }

    Value(Value &&other) noexcept
        : __flags(other.__flags), number(other.number), intValue(other.intValue),
          value(std::move(other.value))
    {
    }

    Value &operator=(const Value &other)
    {
        if (this == &other)
            return *this;

        __flags = other.__flags;
        number = other.number;
        intValue = other.intValue;

        if (other.value)
            value = std::make_unique<Dust>(*other.value);
        else
            value.reset();

        return *this;
    }

    Value &operator=(Value &&other) noexcept
    {
        if (this == &other)
            return *this;

        __flags = other.__flags;
        number = other.number;
        intValue = other.intValue;
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
        value.reset();
        number = 0.0;
        intValue = 0;
        __flags = F_BOOLEAN | (v ? F_ASBOOL : 0);
    }

    /**
     * @brief Overwrites this Value in place with an integer result.
     */
    void setAsInt(int v)
    {
        value.reset();
        number = static_cast<double>(v);
        intValue = v;
        __flags = F_INT | F_NUMBER | (v != 0 ? F_ASBOOL : 0);
    }

    /**
     * @brief Overwrites this Value in place with a double result.
     */
    void setAsDouble(double v)
    {
        value.reset();
        number = v;
        intValue = static_cast<int>(v);
        __flags = F_DOUBLE | F_NUMBER | (v != 0.0 ? F_ASBOOL : 0);
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

        return *std::get<std::shared_ptr<ListValue>>(value->data);
    }

    const ListValue &asList() const
    {
        if (!IS_LIST)
            throw std::runtime_error("This value is not a list.");

        return *std::get<std::shared_ptr<ListValue>>(value->data);
    }

    DictValue &asDict()
    {
        if (!IS_DICT)
            throw std::runtime_error("This value is not a dict.");

        return *std::get<std::shared_ptr<DictValue>>(value->data);
    }

    const DictValue &asDict() const
    {
        if (!IS_DICT)
            throw std::runtime_error("This value is not a dict.");

        return *std::get<std::shared_ptr<DictValue>>(value->data);
    }

    std::shared_ptr<CreationType> asClass() const
    {
        return std::get<std::shared_ptr<CreationType>>(value->data);
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
            return std::get<std::string>(left.value->data) < std::get<std::string>(right.value->data);

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
    std::unique_ptr<Dust> value;
};
