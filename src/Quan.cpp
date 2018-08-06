#include "Quan.h"
#include "Block.h"
#include "Link.h"
#include "System.h"

Quan::Quan()
{
    //ctor
}

Quan::~Quan()
{
    //dtor
}

Quan::Quan(const Quan& other)
{
    _expression = other._expression;
    _timeseries = other._timeseries;
    _var_name = other._var_name;
    _val = other._val;
    _parameters = other._parameters;
    type = other.type;
}

Quan& Quan::operator=(const Quan& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    _expression = rhs._expression;
    _timeseries = rhs._timeseries;
    _var_name = rhs._var_name;
    _val = rhs._val;
    _parameters = rhs._parameters;
    type = rhs.type;
    return *this;
}

double Quan::GetVal(Object *block, const Expression::timing &tmg)
{
    if (type == _type::constant)
        return _val;
    if (type == _type::expression)
        return _expression.calc(block);
    if (type == _type::timeseries)
        return _timeseries.interpol(block->GetParent()->GetTime());
    if (type == _type::value)
        return _val;
    last_error = "Quantity cannot be evaluated";
    return 0;
}

double Quan::GetVal(const Expression::timing &tmg)
{

    if (type == _type::constant)
    {
        if (tmg==Expression::timing::past)
            return _val;
        else
            return _val_star;
    }
    if (type == _type::expression)
        return _expression.calc(parent);
    if (type == _type::timeseries)
        return _timeseries.interpol(parent->GetParent()->GetTime());
    if (type == _type::value)
    {
        if (tmg==Expression::timing::past)
            return _val;
        else
            return _val_star;
    }
    if (type == _type::balance)
    {
        if (tmg==Expression::timing::past)
            return _val;
        else
            return _val_star;
    }
    last_error = "Quantity cannot be evaluated";
    return 0;
}

bool Quan::SetExpression(const string &E)
{
    _expression = E;
}

bool Quan::SetVal(const double &v, const Expression::timing &tmg)
{
    if (tmg == Expression::timing::past)
        _val = v;
    else if (tmg == Expression::timing::present)
        _val_star = v;
    else if (tmg == Expression::timing::both)
    {
        _val = v;
        _val_star = v;
    }
}

void Quan::SetCorrespondingFlowVar(const string &s)
{
    corresponding_flow_quan = s;
}

void Quan::SetMassBalance(bool on)
{
    perform_mass_balance = on;
}

void Quan::SetParent(Object *o)
{
    parent = o;
}

