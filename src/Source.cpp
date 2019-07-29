#include "Source.h"
#include "Object.h"

Source::Source() : Object::Object()
{
    //ctor
}

Source::~Source()
{
    //dtor
}

Source::Source(const Source& other):Object::Object(other)
{
    timeseries = other.timeseries;
    coefficient = other.coefficient;
    evaluation_method = other.evaluation_method;
}

Source& Source::operator=(const Source& rhs)
{
    timeseries = rhs.timeseries;
    coefficient = rhs.coefficient;
    evaluation_method = rhs.evaluation_method;
    if (this == &rhs) return *this; // handle self assignment
    Object::operator=(rhs);
    return *this;
}

double Source::GetValue(Object *obj)
{
    double coeff = coefficient.CalcVal(obj,Expression::timing::present);
    double value;
    if (evaluation_method==_type::timeseries)
        value = timeseries.CalcVal(obj, Expression::timing::present);
    else
        value = 1;

    return coeff*value;

}
