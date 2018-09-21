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

void QuanSet::Append(QuanSet &qset)
{

    for (map<string, Quan>::iterator it = qset.begin(); it!=qset.end(); it++)
    {
    #ifdef Debug_mode
    cout<<it->second.GetName()<<"  " << quans[it->first].GetName() << "  " << it->first << endl;
    #endif // Debug_mode
        Append(it->second.GetName(),quans[it->first]);
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
