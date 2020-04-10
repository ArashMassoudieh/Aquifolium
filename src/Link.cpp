#include "Link.h"
#include "Block.h"

Link::Link():Object::Object()
{
    //ctor
}

Link::~Link()
{
    //dtor
}

Link::Link(const Link& other):Object::Object(other)
{
    //copy ctor
}

Link& Link::operator=(const Link& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    Object::operator=(rhs);
    //assignment operator
    return *this;
}


string Link::toCommand()
{
    string out = "from=" + Object::GetConnectedBlock(Expression::loc::source)->GetName() + "," + "to=" + Object::GetConnectedBlock(Expression::loc::destination)->GetName() + ",";
    out += Object::toCommand();
    return out;
}

