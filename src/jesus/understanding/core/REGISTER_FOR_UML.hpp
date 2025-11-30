#pragma once
#include "uml_registry.hpp"

#define REGISTER_FOR_UML(TYPE, ...)                        \
    static struct TYPE##__uml_registrar                    \
    {                                                      \
        TYPE##__uml_registrar()                            \
        {                                                  \
            UmlClassBuilder b;                             \
            b.name = #TYPE;                                \
            b __VA_ARGS__;                                 \
            UmlRegistry::instance().registerDescriptor(b); \
        }                                                  \
    } TYPE##__uml_reg_instance;
