#include "QuanSet.h"

QuanSet::QuanSet()
{
    //ctor
}

QuanSet::~QuanSet()
{
    //dtor
}

QuanSet::QuanSet(const QuanSet& other)
{
    quans = other.quans;
}

QuanSet& QuanSet::operator=(const QuanSet& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    quans = rhs.quans;
    return *this;
}

bool QuanSet::Append(const string &s, const Quan &q)
{
    if (quans.count(s)>0)
    {
        last_error = "Quantity " + s + " Already exists!";
        return false;
    }
    else
    {
        quans[s] = q;
    }
}

Quan* QuanSet::operator[] (const string s)
{
    if (quans.count(s)==0)
    {
        last_error = "Variable " + s + " does not exist!";
        return nullptr;
    }
    else
        return quans(s);
}
