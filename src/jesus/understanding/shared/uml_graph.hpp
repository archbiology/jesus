#pragma once
#include "../core/uml_registry.hpp"

/**
 * @brief Represents a UML diagram generator for the classes stored in the UmlRegistry.
 *
 * It is responsible for producing textual diagram formats (currently PlantUML)
 * from the metadata collected at compile-time through REGISTER_FOR_UML macros.
 *
 * The diagram typically consists of:
 * - **Class declarations** (name, attributes, methods)
 * - **Inheritance relationships** (solid arrows)
 * - **Associations** inferred from fields whose types represent other registered classes
 * - **Usage/dependency relationships** inferred from constructor-injected dependencies
 */
class UmlGraph
{
public:
    /**
     * @brief Constructs a UML graph bound to an existing registry.
     *
     * @param registry Reference to the UmlRegistry that stores all class descriptors.
     */
    explicit UmlGraph(const UmlRegistry &registry);

    /**
     * @brief Prints a complete UML class diagram in PlantUML format.
     *
     * This method writes a full diagram including:
     * - Class boxes
     * - Inheritance relations
     * - Association relations (field-based)
     * - "Uses" relations (constructor-based dependencies)
     *
     * @param out Output stream to write the PlantUML diagram to.
     */
    void printPlantUml(std::ostream &out) const;

private:
    /**
     * @brief Reference to the registry containing all collected class metadata.
     */
    const UmlRegistry &registry;

    void writePerPackage(const std::string &directory) const;

    /**
     * @brief Print PlantUML class definitions.
     *
     * Includes:
     * - Class names
     * - Attributes (fields)
     * - Methods
     */
    void printClasses(std::ostream &out) const;

    /**
     * @brief Print inheritance relationships between classes.
     *
     * Generates lines such as:
     *
     *  BaseClass <|-- DerivedClass
     */
    void printInheritance(std::ostream &out) const;

    /**
     * @brief Print association relationships based on field types.
     *
     * For each field whose type refers to another UML-registered class,
     * an association arrow is generated:
     *
     *  ClassA --> ClassB
     *
     * Multiplicity or decorators may be added depending on type inference
     * (e.g., vector<T> → 0..*).
     */
    void printAssociations(std::ostream &out) const;

    /**
     * @brief Emits usage/dependency relationships.
     *
     * These represent optional (soft) dependencies such as constructor parameters,
     * producing dashed arrows:
     *
     *  ClassA ..> ClassB
     */
    void printUses(std::ostream &out) const;
};
