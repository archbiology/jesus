#pragma once

class Interpreter;
class InspectStmt;

class VmInspector
{
public:
    static void inspect(Interpreter &jesus, const InspectStmt &stmt);
};
