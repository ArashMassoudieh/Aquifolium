#include "QuanSet.h"
#include "Object.h"
#include "System.h"
#include <json/json.h>


#ifdef QT_version
#include "qjsonobject.h"
#endif // Qt_version


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
	ObjectType = "Entity"; 
	BlockLink = blocklink::entity;
	for (Json::ValueIterator it=object_types->begin(); it!=object_types->end(); ++it)
    {
        if (it.key()=="icon")
            IconFileName() = (*it)["filename"].asString();
		else if (it.key()=="typecategory")
			typecategory = (*object_types)[it.key().asString()].asString();
        else if (it.key()=="type")
        {
			string _type = (*object_types)[it.key().asString()].asString(); 
			if (_type == "block")
			{
				BlockLink = blocklink::block;
				ObjectType = "Block";
			}
			else if (_type == "link")
			{
				BlockLink = blocklink::link;
				ObjectType = "Connector";
			}
			else if (_type == "source")
			{
				BlockLink = blocklink::source;
				ObjectType = "Source";
			}
			else 
			{
				BlockLink = blocklink::entity;
				ObjectType = "Entity";
			}
        }
		else if (it.key()=="description")
			description = (*object_types)[it.key().asString()].asString();
		else
        {
            //cout<<it.key().asString()<<endl;
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
	ObjectType = other.ObjectType; 
    name = other.name;
	typecategory = other.typecategory; 
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
	ObjectType = rhs.ObjectType; 
	typecategory = rhs.typecategory;
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
    string out = aquiutils::tabs(_tabs) + name + ":\n";
    out += aquiutils::tabs(_tabs) + "{\n";
    if (BlockLink == blocklink::block)
        out += aquiutils::tabs(_tabs+1) + "type: block\n";
    else if (BlockLink == blocklink::link)
        out += aquiutils::tabs(_tabs+1) + "type: link\n";

    if (iconfilename!="")
    {
        out += aquiutils::tabs(_tabs+1) + "icon: {\n";
        out += aquiutils::tabs(_tabs+2) + "filename: " + iconfilename + "\n";
        out += aquiutils::tabs(_tabs+1) + "}\n";
    }


    for (map<string,Quan>::iterator it=quans.begin(); it!=quans.end(); it++)
    {
        out += quans[it->first].ToString(_tabs+1) + "\n";
    }

    out += aquiutils::tabs(_tabs) + "}\n";
    return out;
}


void QuanSet::ShowMessage(const string &msg)
{
    if (parent)
        if (parent->Parent())
            if (!parent->Parent()->IsSilent()) cout<<msg<<std::endl;
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

vector<string> QuanSet::QuanNames()
{
    vector<string> out;
    for (map<string,Quan>::iterator it=quans.begin(); it!=quans.end(); it++)
        out.push_back(it->first);
    return out;
}

#ifdef QT_version
QStringList QuanSet::QQuanNames()
{
    QStringList out;
    for (map<string,Quan>::iterator it=quans.begin(); it!=quans.end(); it++)
        out<< QString::fromStdString(it->first);
    return out;
}

QuanSet::QuanSet(QJsonObject::Iterator& object_types)
{
	parent = nullptr;
	Name() = object_types.key().toStdString();
	ObjectType = "Entity";
	BlockLink = blocklink::entity;
	for (QJsonObject::Iterator it = object_types->toArray.begin(); it != object_types->toArray.end(); ++it)
	{
		if (it.key() == "icon")
			IconFileName() = (*it)["filename"].asString();
		else if (it.key() == "typecategory")
			typecategory = (*object_types)[it.key().asString()].asString();
		else if (it.key() == "type")
		{
			string _type = (*object_types)[it.key().asString()].asString();
			if (_type == "block")
			{
				BlockLink = blocklink::block;
				ObjectType = "Block";
			}
			else if (_type == "link")
			{
				BlockLink = blocklink::link;
				ObjectType = "Connector";
			}
			else if (_type == "source")
			{
				BlockLink = blocklink::source;
				ObjectType = "Source";
			}
			else
			{
				BlockLink = blocklink::entity;
				ObjectType = "Entity";
			}
		}
		else if (it.key() == "description")
			description = (*object_types)[it.key().asString()].asString();
		else
		{
			//cout<<it.key().asString()<<endl;
			Quan Q(it);
			Append(it.key().asString(), Q);
		}
	}
}


#endif
