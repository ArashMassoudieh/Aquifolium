#include "QuanSet.h"
#include "Object.h"
#include "System.h"

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
    name = other.name;

}

QuanSet& QuanSet::operator=(const QuanSet& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    name = rhs.name;
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
        last_error = "In " + Name() + ": Quantity " + s + " Already exists!";
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
    ShowMessage(it->second.GetName() + "  " + qset[it->first].GetName() + "  " + it->first);
    #endif // Debug_mode
        Append(it->second.GetName(),qset[it->first]);
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

Quan& QuanSet::GetVar(const string &s)
{
    if (quans.count(s)==0)
    {
        last_error = "Variable " + s + " does not exist!";
    }
    else
        return quans[s];
}

string QuanSet::ToString(int _tabs)
{
    string out = tabs(_tabs) + name + ":\n";
    out += tabs(_tabs) + "{\n";
    if (BlockLink == blocklink::block)
        out += tabs(_tabs+1) + "type: block\n";
    else if (BlockLink == blocklink::link)
        out += tabs(_tabs+1) + "type: link\n";

    if (iconfilename!="")
    {
        out += tabs(_tabs+1) + "icon: {\n";
        out += tabs(_tabs+2) + "filename: " + iconfilename + "\n";
        out += tabs(_tabs+1) + "}\n";
    }


    for (map<string,Quan>::iterator it=quans.begin(); it!=quans.end(); it++)
    {
        out += quans[it->first].ToString(_tabs+1) + "\n";
    }

    out += tabs(_tabs) + "}\n";
    return out;
}


void QuanSet::ShowMessage(const string &msg)
{
    if (parent)
        if (parent->Parent())
            if (!parent->Parent()->IsSilent()) cout<<msg<<endl;
}

void QuanSet::SetAllParents()
{
    for (map<string,Quan>::iterator it=quans.begin(); it!=quans.end(); it++)
        quans[it->first].SetParent(parent);
}
