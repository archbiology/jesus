#pragma once

class Interpreter;
class InspectStmt;

class BytecodeInspector
{
public:
    static void inspect(Interpreter &jesus, const InspectStmt &stmt);
};
