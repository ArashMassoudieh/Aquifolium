#include "Parameter.h"

Parameter::Parameter()
{
    //ctor
}

Parameter::~Parameter()
{
    //dtor
}

Parameter::Parameter(const Parameter& other)
{
    _location = other._location;
    _quan = other._quan;
    prior_range = other.prior_range;
    prior_distribution = other.prior_distribution;
    value = other.value;
    last_error = other.last_error;
}

Parameter& Parameter::operator=(const Parameter& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    _location = rhs._location;
    _quan = rhs._quan;
    prior_range = rhs.prior_range;
    prior_distribution = rhs.prior_distribution;
    value = rhs.value;
    last_error = rhs.last_error;
    return *this;
}
