#include "known_types.hpp"
#include "atomic/numbers/number_type.hpp"
#include "atomic/numbers/real_type.hpp"
#include "atomic/numbers/natural_type.hpp"
#include "atomic/numbers/percentage_type.hpp"
#include "atomic/numbers/decimal_type.hpp"
#include "atomic/strings/text_type.hpp"
#include "atomic/strings/word_type.hpp"
#include "atomic/strings/phrase_type.hpp"
#include <memory>

void KnownTypes::registerBuiltInTypes()
{
    registerType(std::make_unique<WordType>());
    registerType(std::make_unique<PhraseType>());
    registerType(std::make_unique<TextType>());

    registerType(std::make_unique<RealType>());
    registerType(std::make_unique<NumberType>());
    registerType(std::make_unique<NaturalType>());
    registerType(std::make_unique<PercentageType>());
    registerType(std::make_unique<DecimalType>());
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
