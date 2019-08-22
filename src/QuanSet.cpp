#include "QuanSet.h"
#include "Object.h"
#include "System.h"
#include <json/json.h>

QuanSet::QuanSet()
{
    parent = nullptr;
}

QuanSet::~QuanSet()
{
    //dtor
}

QuanSet::QuanSet(Json::ValueIterator& object_types)
{
    parent = nullptr;
    Name() = object_types.key().asString();
    for (Json::ValueIterator it=object_types->begin(); it!=object_types->end(); ++it)
    {
        if (it.key()=="icon")
            IconFileName() = (*it)["filename"].asString();
        if (it.key()=="type")
        {
            if ((*object_types)[it.key().asString()]=="block")
                BlockLink = blocklink::block;
            if ((*object_types)[it.key().asString()]=="link")
                BlockLink = blocklink::link;
            if ((*object_types)[it.key().asString()]=="source")
                BlockLink = blocklink::source;
        }
        else
        {
            cout<<it.key().asString()<<endl;
            Quan Q(it);
            Append(it.key().asString(),Q);
        }
    }
}

QuanSet::QuanSet(const QuanSet& other)
{
    quans = other.quans;
    iconfilename = other.iconfilename;
    description = other.description;
    BlockLink = other.BlockLink;
    name = other.name;
    parent = nullptr;

}

QuanSet& QuanSet::operator=(const QuanSet& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    name = rhs.name;
    quans = rhs.quans;
    iconfilename = rhs.iconfilename;
    description = rhs.description;
    BlockLink = rhs.BlockLink;
    parent = nullptr;
    return *this;
}

bool QuanSet::Append(const string &s, const Quan &q)
{
    if (quans.count(s)>0)
    {
        AppendError(Name(),"QuanSet","Append","In " + Name() + ": Quantity " + s + " Already exists!",2001);
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
        AppendError(Name(),"QuanSet","GetVar","Variable " + s + " does not exist!",2001);
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

bool QuanSet::AppendError(const string &objectname, const string &cls, const string &funct, const string &description, const int &code)
{
    if (!parent)
        return false;
    if (!parent->Parent())
        return false;

    parent->Parent()->errorhandler.Append(objectname,cls,funct,description,code);
    return true;
}

vector<CTimeSeries*> QuanSet::TimeSeries()
{

    vector<CTimeSeries*> out;
    for (map<string,Quan>::iterator it=quans.begin(); it!=quans.end(); it++)
    {
        if (quans[it->first].GetType() == Quan::_type::timeseries && quans[it->first].TimeSeries()!=nullptr)
            out.push_back(quans[it->first].TimeSeries());
    }
    return out;
}
