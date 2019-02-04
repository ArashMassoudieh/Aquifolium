#include "Condition.h"

Condition::Condition()
{
    //ctor
}

Condition::~Condition()
{
    //dtor
}

Condition::Condition(const Condition &S)
{
    lhs = S.lhs;
    rhs = S.rhs;
    oprtr = S.oprtr;
    last_error = S.last_error;
}

Condition& Condition::operator=(const Condition& S)
{
    lhs = S.lhs;
    rhs = S.rhs;
    oprtr = S.oprtr;
    last_error = S.last_error;

    return *this;
}

Condition::Condition(const string &str)
{
    if (split(str,'<').size()>1)
    {
        oprtr = _oprtr::lessthan;
        vector<string> lrs = split(str,'<');
        lhs = Expression(lrs[0]);
        rhs = Expression(lrs[1]);
        return;
    }
    if (split(str,'>').size()>1)
    {
        oprtr = _oprtr::greaterthan;
        vector<string> lrs = split(str,'>');
        lhs = Expression(lrs[0]);
        rhs = Expression(lrs[1]);
        return;
    }
    else
        last_error = "Expression (" + str + ") does not contain a inequality operator!";
    return;

}
void Condition::SetRHS(const Expression &expr)
{
    rhs = expr;
}
void Condition::SetLHS(const Expression &expr)
{
    lhs = expr;
}
void Condition::SetRHS(const string &expr)
{
    rhs = Expression(expr);
}
void Condition::SetLHS(const string &expr)
{
    lhs = Expression(expr);
}
bool Condition::calc(Object *W, const Expression::timing &tmg)
{
    if (oprtr == _oprtr::greaterthan)
    {
        if (lhs.calc(W, tmg)>rhs.calc(W,tmg)) return true; else return false;
    }
    if (oprtr == _oprtr::lessthan)
    {
        if (lhs.calc(W, tmg)<rhs.calc(W,tmg)) return true; else return false;
    }

}

string Condition::ToString()
{
    string s;
    if (oprtr == _oprtr::lessthan)
        s = lhs.ToString() + "<" + rhs.ToString();
    if (oprtr == _oprtr::greaterthan)
        s = rhs.ToString() + ">" + rhs.ToString();
    return s;
}
