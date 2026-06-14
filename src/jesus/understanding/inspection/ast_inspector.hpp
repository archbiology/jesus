#pragma once

class Interpreter;
class InspectStmt;

class AstInspector
{
public:
    static void inspect(Interpreter &jesus, const InspectStmt &stmt);
};
