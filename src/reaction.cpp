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
    RateExpression = other.RateExpression;
    Stoichiometries = other.Stoichiometries;
    Object::operator=(other);

}

Reaction& Reaction::operator=(const Reaction& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    RateExpression = rhs.RateExpression;
    Stoichiometries = rhs.Stoichiometries;
    Object::operator=(rhs);
    return *this;
}
