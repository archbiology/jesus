#include "uml_exporter.hpp"
#include "understanding/shared/uml_graph.hpp"
#include "understanding/core/uml_registry.hpp"

void UmlExporter::exportPlantUml(std::ostream &out)
{
    UmlGraph graph(UmlRegistry::instance());
    graph.printPlantUml(out);
}
