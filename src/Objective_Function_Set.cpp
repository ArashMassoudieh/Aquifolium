#include "Objective_Function_Set.h"

Objective_Function_Set::Objective_Function_Set()
{
    //ctor
}

Objective_Function_Set::~Objective_Function_Set()
{
    //dtor
}

Objective_Function_Set::Objective_Function_Set(const Objective_Function_Set& other)
{
    objectivefunctions = other.objectivefunctions;
}

Objective_Function_Set& Objective_Function_Set::operator=(const Objective_Function_Set& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    objectivefunctions = rhs.objectivefunctions;
    return *this;
}

void Objective_Function_Set::Append(const string &name, const Objective_Function &o_func, double weight)
{
    obj_funct_weight tobeappended;
    tobeappended.obj_funct = o_func;
    tobeappended.weight = weight;
    objectivefunctions[name] = tobeappended;
    return;
}
obj_funct_weight* Objective_Function_Set::operator[](string name)
{
    if (objectivefunctions.count(name)==1)
        return &objectivefunctions[name];
    else
    {
        lasterror = "objective function " + name + " does not exist!";
        return nullptr;
    }
}

double Objective_Function_Set::Calculate()
{
    double out = 0;
    for (map<string, obj_funct_weight>::iterator it = objectivefunctions.begin(); it != objectivefunctions.end(); it++)
        out += it->second->obj_funct.GetValue(Expression::timing::present)*it->weight;

    return out;
}
