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
    iconfilename = other.iconfilename;
    description = other.description;
    BlockLink = other.BlockLink;
}

QuanSet& QuanSet::operator=(const QuanSet& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    quans = rhs.quans;
    iconfilename = rhs.iconfilename;
    description = rhs.description;
    BlockLink = rhs.BlockLink;
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

Quan& QuanSet::operator[] (const string &s)
{
    if (quans.count(s)==0)
    {
        last_error = "Variable " + s + " does not exist!";
    }
    else
        return quans[s];
}
