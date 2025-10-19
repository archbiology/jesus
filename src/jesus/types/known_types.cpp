#include "known_types.hpp"
#include "atomic/literals/truth.hpp"
#include "atomic/literals/nothing.hpp"
#include "atomic/literals/sex.hpp"
#include "atomic/literals/weekday.hpp"
#include "atomic/numbers/number_type.hpp"
#include "atomic/numbers/real_type.hpp"
#include "atomic/numbers/natural_type.hpp"
#include "atomic/numbers/percentage_type.hpp"
#include "atomic/numbers/decimal_type.hpp"
#include "atomic/strings/text_type.hpp"
#include "atomic/strings/word_type.hpp"
#include "atomic/strings/phrase_type.hpp"
#include "composite/class_type.hpp"
#include <memory>

void KnownTypes::registerBuiltInTypes()
{
    auto truth = std::make_shared<TruthType>();
    auto creation = std::make_shared<CreationType>(PrimitiveType::Good, "creation", "core");
    auto nothing = std::make_shared<NothingType>();
    auto sex = std::make_shared<SexType>();
    auto weekday = std::make_shared<WeekdayType>();

    auto integer = std::make_shared<NumberType>();
    auto natural = std::make_shared<NaturalType>();
    auto floating = std::make_shared<RealType>();
    auto text = std::make_shared<TextType>();

    auto klass = std::make_shared<ClassType>();

    BOOLEAN = TRUTH = truth;
    VOID = NOTHING = nothing;
    BORN = SEX = sex;
    WEEKDAY = weekday;

    INT = integer;
    NATURAL = natural;
    FLOAT = floating;
    DOUBLE = floating; // TODO: Really differentiate 'float' from 'double'
    STRING = text;
    CLASS = klass;

    registerType(truth);
    registerType(creation);
    registerType(nothing);
    registerType(sex);
    registerType(weekday);

    registerType(integer);
    registerType(natural);
    registerType(floating);
    registerType(std::make_shared<PercentageType>());
    registerType(std::make_shared<DecimalType>());

    registerType(text);
    registerType(std::make_shared<WordType>());
    registerType(std::make_shared<PhraseType>());

    registerType(klass);
}

void KnownTypes::registerType(std::shared_ptr<CreationType> type)
{
    const std::string fullname = makeKey(type->module_name, type->name);
    int id = type->id;

    if (typesById.size() <= id)
        typesById.resize(id + 50);

    typesById[id] = type.get();
    typesByName[fullname] = std::move(type);
}

const std::shared_ptr<CreationType> KnownTypes::resolve(const std::string &name, const std::string &module)
{
    const std::string key = makeKey(module, name);
    auto it = typesByName.find(key);
    if (it != typesByName.end())
        return it->second;

    return nullptr;
}

const CreationType *KnownTypes::getById(int id) // TODO: return smart pointer
{
    if (id < 0 || id >= typesById.size())
        return nullptr; // TODO: Remove this checking?

    return typesById[id];
}

bool KnownTypes::isValid(const CreationType *type, const Value &value)
{
    return type && type->validate(value);
}

std::string KnownTypes::makeKey(const std::string &module_name, const std::string &name)
{
    return module_name + "." + name;
}
