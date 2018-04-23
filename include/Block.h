#ifndef BLOCK_H
#define BLOCK_H
#include <map>
#include <string>
#include "Expression.h"

using namespace std;

class Block
{
    public:
        Block();
        virtual ~Block();
        double GetVal(const string&);
        string GetProperty(const string&);
        bool Set(const string& s, double value);
        bool Set(const string& s, const string & value);
        void AddProperty(const string &s, double val=0);
    protected:

    private:
        map<string, double> properties;
        map<string, string> setting;
        map<string, double> state;
        map<string, Expression> expression;
};

#endif // BLOCK_H
