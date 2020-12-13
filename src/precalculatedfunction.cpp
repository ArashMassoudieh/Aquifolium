#include "precalculatedfunction.h"

PreCalculatedFunction::PreCalculatedFunction() : CTimeSeries::CTimeSeries()
{
    //ctor
}

PreCalculatedFunction::~PreCalculatedFunction()
{
    //dtor
}

PreCalculatedFunction::PreCalculatedFunction(const PreCalculatedFunction& other):CTimeSeries::CTimeSeries(other)
{
    CTimeSeries::operator=(other);
}

PreCalculatedFunction& PreCalculatedFunction::operator=(const PreCalculatedFunction& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    CTimeSeries::operator=(rhs);
    return *this;
}

