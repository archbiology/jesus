#include "uml_graph.hpp"
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include "../../utils/version.hpp"

// namespace fs = std::filesystem;

UmlGraph::UmlGraph(const UmlRegistry &reg)
    : registry(reg) {}

void UmlGraph::printPlantUml(std::ostream &out) const
{
    const std::string version = JESUS_VERSION;
    std::string umlFolder = "uml-jesus";// + version;

    writePerPackage(umlFolder);

    // out << "@startuml\n";
    // out << "skinparam classAttributeIconSize 0\n";

    // printClasses(out);
    // printInheritance(out);
    // printAssociations(out);
    // printUses(out);

    // out << "@enduml\n";
}

void UmlGraph::writePerPackage(const std::string &directory) const
{
    // ensure directory exists
    std::filesystem::create_directories(directory);

    // Step 1: Group classes by package
    std::map<std::string, std::vector<const UmlClassInfo *>> groups;

    for (auto &pair : registry.allClasses())
    {
        const auto &cls = pair.second;

        std::string pkg = cls.package.empty() ? "default" : cls.package;

        groups[pkg].push_back(&cls);
    }

    // Step 2: Generate files per package
    for (auto &[pkg, classes] : groups)
    {
        // replace dots by underscores for filename safety
        // std::string package;
        // for (char &c : package)
        //     if (c == '.')
        //         c = '_';

        // replace dots by underscores for filename safety
        std::string package = pkg;
        for (char &c : package)
            if (c == '.')
                c = '_';

        std::string filename = directory + "/" + package + ".puml";

        // for (char &c : filename)
        //     if (c == '.')
        //         c = '_';

        std::ofstream out(filename);
        out << "@startuml\n";

        out << "package \"" << package << "\" {\n";

        for (auto cls : classes)
        {
            out << "class " << cls->name << " {\n";
            for (auto &f : cls->fields)
            {
                out << "  " << f.name;
                if (!f.type.empty())
                    out << ": " << f.type;
                out << "\n";
            }
            for (auto &m : cls->methods)
                out << "  " << m << "()\n";

            out << "}\n";
        }

        out << "}\n";

        // local inheritance & dependencies
        for (auto cls : classes)
        {
            for (auto &p : cls->parents)
            {
                out << p << " <|-- " << cls->name << "\n";
            }
            for (auto &field : cls->fields)
            {
                if (!field.type.empty() && std::isupper(field.type[0]))
                {
                    out << cls->name << " --> " << field.type << "\n";
                }
            }
            for (auto &u : cls->uses)
            {
                out << cls->name << " ..> " << u << " : «uses»\n";
            }
        }

        out << "@enduml\n";
        out.close();

        // Step 3 — Log / message for each file
        std::cout << "[UML] Generated: " << filename << "\n";
    }
}

void UmlGraph::printClasses(std::ostream &out) const
{
    // First group classes by package
    std::unordered_map<std::string, std::vector<const UmlClassInfo *>> byPackage;

    for (const auto &pair : registry.allClasses())
    {
        byPackage[pair.second.package].push_back(&pair.second);
    }

    // Now print each package
    for (auto &pkg : byPackage)
    {
        if (!pkg.first.empty())
        {
            out << "package \"" << pkg.first << "\" {\n";
        }

        for (auto *cls : pkg.second)
        {
            out << "  class " << cls->name << " {\n";

            // fields
            for (auto &f : cls->fields)
            {
                out << "    " << f.name;
                if (!f.type.empty())
                    out << ": " << f.type;
                out << "\n";
            }

            // methods
            for (auto &m : cls->methods)
                out << "    " << m << "()\n";

            out << "  }\n";
        }

        if (!pkg.first.empty())
        {
            out << "}\n\n";
        }
    }

    // for (const auto &pair : registry.allClasses())
    // {
    //     const auto &cls = pair.second;

    //     out << "class " << cls.name;

    //     out << " {\n";
    //     for (auto &f : cls.fields)
    //     {
    //         out << "  " << f.name;

    //         if (!f.type.empty())
    //             out << ": " << f.type;

    //         out << "\n";
    //     }

    //     for (auto &m : cls.methods)
    //         out << "  " << m << "()\n";
    //     out << "}\n";
    // }
}

void UmlGraph::printInheritance(std::ostream &out) const
{
    for (const auto &pair : registry.allClasses())
    {
        const auto &cls = pair.second;

        for (auto &p : cls.parents)
        {
            out << p << " <|-- " << cls.name << "\n";
        }
    }
}

void UmlGraph::printAssociations(std::ostream &out) const
{
    for (auto &pair : registry.allClasses())
    {
        const auto &cls = pair.second;

        for (const auto &field : cls.fields)
        {
            if (!field.type.empty() && std::isupper(field.type[0]))
            {
                out << cls.name << " --> " << field.type << "\n";
            }
        }
    }
}

void UmlGraph::printUses(std::ostream &out) const
{
    for (auto &pair : registry.allClasses())
    {
        const auto &cls = pair.second;

        for (const auto &u : cls.uses)
        {
            out << cls.name << " ..> " << u << " : «uses»\n";
        }
    }
}