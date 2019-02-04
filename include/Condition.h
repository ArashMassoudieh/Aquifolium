#ifndef CONDITION_H
#define CONDITION_H
#include "Expression.h"

class Object;

enum class _oprtr{lessthan, greaterthan};

class Condition
{
    public:
        Condition();
        Condition(const string &str);
        Condition(const Condition &S);
        Condition& operator=(const Condition&);
        virtual ~Condition();
        void SetRHS(const Expression &expr);
        void SetLHS(const Expression &expr);
        void SetRHS(const string &expr);
        void SetLHS(const string &expr);
        bool calc(Object *W, const Expression::timing &tmg);
        string GetLastError() {return last_error;}
        string ToString();
    protected:

    private:
        Expression lhs;
        Expression rhs;
        _oprtr oprtr;
        string last_error;
};

#endif // CONDITION_H
