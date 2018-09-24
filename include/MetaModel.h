#ifndef METAMODEL_H
#define METAMODEL_H

#include "QuanSet.h"
#include <map>

class MetaModel
{
    public:
        MetaModel();
        virtual ~MetaModel();
        MetaModel(const MetaModel& other);
        MetaModel& operator=(const MetaModel& other);
        bool Append(const string &s, const QuanSet &q);
        int Count(const string &s) {return metamodel.count(s);}
        map<string, QuanSet> *GetMetaModel() {return &metamodel;}
        QuanSet* operator[] (const string &typ);
        std::map<string,QuanSet>::iterator find(const string &name) {return metamodel.find(name);}
        std::map<string,QuanSet>::iterator end() {return metamodel.end();}
        std::map<string,QuanSet>::iterator begin() {return metamodel.begin();}
        unsigned int size() {return metamodel.size();}
        bool GetFromJsonFile(const string &filename);
        void Clear();
        string ToString(int _tabs=0);
    protected:

    private:
        map<string, QuanSet> metamodel;
        string last_error;
        vector<string> errors;
};

#endif // METAMODEL_H
