#ifndef REACTION_H
#define REACTION_H

#include "Object.h"
#include "Expression.h"
#include <map>

class Reaction : public Object
{
public:
    Reaction();

private:
    map<string,Expression> Stoichiometries;
    Expression RateExpression;
    Reaction(System *parent);
    Reaction(const Reaction& other);
    Reaction& operator=(const Reaction& rhs);
    virtual ~Reaction();

};

#endif // REACTION_H
