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
    string out = aquiutils::tabs(_tabs) + "Name: " + GetName() + "\n";
    out += aquiutils::tabs(_tabs) + "low: " + aquiutils::numbertostring(prior_range.low) + "\n";
    out += aquiutils::tabs(_tabs) + "high: " + aquiutils::numbertostring(prior_range.high) + "\n";
    out += aquiutils::tabs(_tabs) + "quans: {";

    for (int i=0; i<_quan.size(); i++ ) { out +=  _quan[i]; if (i<_quan.size()-1) out += ","; }
    out += "}\n";
    out += aquiutils::tabs(_tabs) + "locations: {";
    for (int i=0; i<_location.size(); i++ ) { out +=  _location[i]; if (i<_location.size()-1) out += ","; }
    out += "}\n";
    out += aquiutils::tabs(_tabs) + "prior distribution: " + prior_distribution + "\n";
    out += aquiutils::tabs(_tabs) + "value: " + aquiutils::numbertostring(value) + "\n";
    return out;
}

bool Parameter::HasQuantity(const string &qntty)
{
    if (aquiutils::tolower(qntty)!="location" && aquiutils::tolower(qntty)!="quan" && aquiutils::tolower(qntty)!="range" && aquiutils::tolower(qntty)!="low" && aquiutils::tolower(qntty)!="prior_distribution" && aquiutils::tolower(qntty)!="name" && aquiutils::tolower(qntty)!="value")
        return false;
    else
        return true;
}

string Parameter::Variable(const string &qntty)
{
    if (aquiutils::tolower(qntty)=="location")
    {
        string out;
        out += "{";
        for (int i=0; i<_location.size(); i++ ) { out +=  _location[i]; if (i<_location.size()-1) out += ","; }
        out += "}";
        return out;
    }
    if (aquiutils::tolower(qntty)=="name")
        return GetName();

    if (aquiutils::tolower(qntty)=="low")
        return aquiutils::numbertostring(prior_range.low);
    if (aquiutils::tolower(qntty)=="high")
        return aquiutils::numbertostring(prior_range.high);

    if (aquiutils::tolower(qntty)=="quan")
    {
        string out;
        out += "{";
        for (int i=0; i<_quan.size(); i++ ) { out +=  _quan[i]; if (i<_quan.size()-1) out += ","; }
        out += "}";
        return out;
    }

    if (aquiutils::tolower(qntty)=="prior_distribution")
        return prior_distribution;

    if (aquiutils::tolower(qntty)=="value")
        return aquiutils::numbertostring(value);

    if (aquiutils::tolower(qntty)=="range")
        return "{" + aquiutils::numbertostring(prior_range.low) +"," + aquiutils::numbertostring(prior_range.high) + "}";

    return "";
}
