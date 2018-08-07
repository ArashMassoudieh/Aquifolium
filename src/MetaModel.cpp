#include "MetaModel.h"

MetaModel::MetaModel()
{
    //ctor
}

MetaModel::~MetaModel()
{
    //dtor
}

MetaModel::MetaModel(const MetaModel& other)
{
    metamodel = other.metamodel;
}

MetaModel& MetaModel::operator=(const MetaModel& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    metamodel = rhs.metamodel;
    return *this;
}

bool MetaModel::Append(const string &s, const QuanSet &q)
{
    if (metamodel.count(s)>0)
    {
        last_error = "Object type " + s + " Already exist";
        errors.push_back(last_error);
        return false;
    }
    else
        metamodel[s] = q;
    return true;
}

QuanSet* MetaModel::operator[] (const string &typ)
{
    if (metamodel.count(typ)==0)
    {
        last_error = "Type " + typ + " was not found!";
        return nullptr;
    }
    else
        return &(metamodel[typ]);
}
