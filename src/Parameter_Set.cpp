#include "Parameter_Set.h"

Parameter_Set::Parameter_Set()
{
    //ctor
}

Parameter_Set::~Parameter_Set()
{
    //dtor
}

Parameter_Set::Parameter_Set(const Parameter_Set& other)
{
    parameters = other.parameters;
    lasterror = other.lasterror;
}

Parameter_Set& Parameter_Set::operator=(const Parameter_Set& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    parameters = rhs.parameters;
    lasterror = rhs.lasterror;
    return *this;
}

void Parameter_Set::Append(const string &name, const Parameter &param)
{
    parameters[name] = param;
    return;
}
Parameter* Parameter_Set::operator[](string name)
{
    if (parameters.count(name)==1)
        return &parameters[name];
    else
    {
        lasterror = "Parameter " + name + " does not exist!";
        return nullptr;
    }
}

Parameter* Parameter_Set::operator[](int i)
{
    return &parameters[getKeyAtIndex (i)];
}

string Parameter_Set::getKeyAtIndex (int index){
    map<string, Parameter>::const_iterator end = parameters.end();

    int counter = 0;
    for (map<string, Parameter>::const_iterator it = parameters.begin(); it != end; ++it) {
        counter++;

        if (counter == index)
            return it->first;
    }
}

