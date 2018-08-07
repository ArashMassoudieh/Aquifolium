#ifndef QUANSET_H
#define QUANSET_H

#include <map>
#include <Quan.h>

class QuanSet
{
    public:
        QuanSet();
        virtual ~QuanSet();
        QuanSet(const QuanSet& other);
        QuanSet& operator=(const QuanSet& other);
        bool Append(const string &s, const Quan &q);
        int Count(const string &s) {return quans.count(s);};
        Quan* operator[] (const string s);
    protected:

    private:
        map<string, Quan> quans;
        string last_error;
};

#endif // QUANSET_H
