#include "Parameter.h"
#include "Expression.h"


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

string Parameter::toString(int _tabs)
{
    string out = tabs(_tabs) + "Name: " + GetName() + "\n";
    out += tabs(_tabs) + "low: " + numbertostring(prior_range.low) + "\n";
    out += tabs(_tabs) + "high: " + numbertostring(prior_range.high) + "\n";
    out += tabs(_tabs) + "quans: {";

    for (int i=0; i<_quan.size(); i++ ) { out +=  _quan[i]; if (i<_quan.size()-1) out += ","; }
    out += "}\n";
    out += tabs(_tabs) + "locations: {";
    for (int i=0; i<_location.size(); i++ ) { out +=  _location[i]; if (i<_location.size()-1) out += ","; }
    out += "}\n";
    out += tabs(_tabs) + "prior distribution: " + prior_distribution + "\n";
    out += tabs(_tabs) + "value: " + numbertostring(value) + "\n";
    return out;
}

bool Parameter::HasQuantity(const string &qntty)
{
    if (tolower(qntty)!="location" && tolower(qntty)!="quan" && tolower(qntty)!="range" && tolower(qntty)!="low" && tolower(qntty)!="prior_distribution" && tolower(qntty)!="name" && tolower(qntty)!="value")
        return false;
    else
        return true;
}

string Parameter::Variable(const string &qntty)
{
    if (tolower(qntty)=="location")
    {
        string out;
        out += "{";
        for (int i=0; i<_location.size(); i++ ) { out +=  _location[i]; if (i<_location.size()-1) out += ","; }
        out += "}";
        return out;
    }
    if (tolower(qntty)=="name")
        return GetName();

    if (tolower(qntty)=="low")
        return numbertostring(prior_range.low);
    if (tolower(qntty)=="high")
        return numbertostring(prior_range.high);

    if (tolower(qntty)=="quan")
    {
        string out;
        out += "{";
        for (int i=0; i<_quan.size(); i++ ) { out +=  _quan[i]; if (i<_quan.size()-1) out += ","; }
        out += "}";
        return out;
    }

    if (tolower(qntty)=="prior_distribution")
        return prior_distribution;

    if (tolower(qntty)=="value")
        return numbertostring(value);

    if (tolower(qntty)=="range")
        return "{" + numbertostring(prior_range.low) +"," + numbertostring(prior_range.high) + "}";

    return "";
}
