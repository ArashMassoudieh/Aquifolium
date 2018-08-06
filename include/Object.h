#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include <map>
#include "Quan.h"

using namespace std;

class Object
{
    public:
        Object();
        virtual ~Object();
        Object(const Object& other);
        Object& operator=(const Object& other);
        double GetVal(const string& s, const Expression::timing &tmg=Expression::timing::past);
        bool AddQnantity(const string &name,const Quan &Q);
        bool SetQuantities(const map<string, Quan> &Q);
        bool SetVal(const string& s, double value, const Expression::timing &tmg = Expression::timing::past);
        bool SetVal(const string& s, const string & value, const Expression::timing &tmg = Expression::timing::past);
        double GetProperty(const string&);
        System *GetParent() const {return parent;};
        string GetName() const;
        bool SetName(const string &_name);
        Object* GetConnectedBlock(Expression::loc l);
        void SetConnectedBlock(Expression::loc l, const string &blockname);
        void AppendError(const string &s);
        void SetParent(System *s);
        Quan* CorrespondingFlowVariable(const string &s);
        Quan* Variable(const string &s);
    protected:

    private:
        map<string, Quan> var;
        vector<string> errors;
        string last_error;
        bool last_operation_success;
        map<string, string> setting;
        System *parent;
        string name;
        Object *s_Block;
        Object *e_Block;

};

#endif // OBJECT_H
