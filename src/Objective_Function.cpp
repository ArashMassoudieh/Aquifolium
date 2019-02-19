#include "Objective_Function.h"
#include "System.h"

Objective_Function::Objective_Function()
{
    //ctor
}

Objective_Function::Objective_Function(System *_system)
{
    system = _system;
}

Objective_Function::~Objective_Function()
{
    //dtor
}

Objective_Function::Objective_Function(const Objective_Function& other)
{
    system = other.system;
    expression = other.expression;
    location = other.location;
}

Objective_Function& Objective_Function::operator=(const Objective_Function& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    system = rhs.system;
    expression = rhs.expression;
    location = rhs.location;
    return *this;
}

double Objective_Function::GetValue(const Expression::timing &tmg)
{
    if (system->block(location) != nullptr)
        return expression.calc(system->block(location),tmg);
    if (system->link(location) != nullptr)
        return expression.calc(system->link(location),tmg);
    lasterror = "Location " + location + "was not found in the system!";
    return 0;
}
