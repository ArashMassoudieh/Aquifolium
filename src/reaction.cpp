#include "reaction.h"

Reaction::Reaction() : Object::Object()
{
    //ctor
}

Reaction::~Reaction()
{
    //dtor
}

Reaction::Reaction(const Reaction& other):Object::Object(other)
{

    Object::operator=(other);

}

Reaction& Reaction::operator=(const Reaction& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    Object::operator=(rhs);
    return *this;
}
