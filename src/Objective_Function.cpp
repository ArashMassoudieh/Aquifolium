#include "Objective_Function.h"
#include "System.h"


Objective_Function::Objective_Function(): Object::Object()
{
    //ctor
}

Objective_Function::Objective_Function(System *_system)
{
    system = _system;
}

Objective_Function::Objective_Function(System *_system, const Expression &expr, const string &loc)
{
    system = _system;
    expression = expr;
    location = loc;
}

Objective_Function::~Objective_Function()
{
    //dtor
}

Objective_Function::Objective_Function(const Objective_Function& other)
{
    Object::operator=(other);
    expression = other.expression;
    location = other.location;
    type = other.type;
}

Objective_Function& Objective_Function::operator=(const Objective_Function& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    Object::operator=(rhs);
    expression = rhs.expression;
    location = rhs.location;
    type = rhs.type;
    return *this;
}

bool Objective_Function::SetProperty(const string &prop, const string &val)
{
    if (aquiutils::tolower(prop)=="expression")
    {
        expression = Expression(val);

    }
    if (aquiutils::tolower(prop)=="location" || aquiutils::tolower(prop)=="object")
    {
        location = val;

    }
    if (aquiutils::tolower(prop)=="method")
    {
        if (aquiutils::tolower(val)=="integrate") {type = objfunctype::Integrate;}
        if (aquiutils::tolower(val)=="value") {type = objfunctype::Value;}
        lasterror = "Type '" + val + "' was not recognized!";
    }
    return Object::SetProperty(prop,val);
    return false;
}

double Objective_Function::GetValue(const Expression::timing &tmg)
{
    if (system->block(location) != nullptr)
    {
        current_value = expression.calc(system->block(location),tmg);
        return current_value;
    }
    if (system->link(location) != nullptr)
    {
        current_value = expression.calc(system->link(location),tmg,true);
        return current_value;
    }
    lasterror = "Location " + location + "was not found in the system!";
    return 0;
}

void Objective_Function::append_value(double t, double val)
{
    stored_time_series.append(t,val);
    return;
}

void Objective_Function::append_value(double t)
{
    stored_time_series.append(t,GetValue(Expression::timing::present));
    return;
}

vector<string> Objective_Function::ItemswithOutput()
{
    vector<string> s = Object::ItemswithOutput();
    s.push_back("Time Series");
    return s;
}

double Objective_Function::GetObjective()
{
    if (type==objfunctype::Integrate)
        return stored_time_series.integrate();
    else if (type==objfunctype::Value)
        return GetValue(Expression::timing::present);
    else
        return 0;
}

double Objective_Function::Weight()
{
    if (GetVars()->Count("weight")>0)
    {
        return Variable("weight")->GetVal();
    }
    else
        return 1;
}




