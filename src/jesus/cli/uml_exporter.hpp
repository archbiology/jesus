#pragma once
#include <string>
#include "understanding/core/REGISTER_FOR_UML.hpp"

REGISTER_FOR_UML(
    UmlExporter,
    .packageName("cli")
        .methodsList({"exportPlantUml"}));

class UmlExporter
{
public:
    static void exportPlantUml(std::ostream &out);
};
