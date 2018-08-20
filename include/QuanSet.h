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
        size_t Count(const string &s) {return quans.count(s);};
        Quan& operator[] (const string &s);
        std::map<string,Quan>::iterator find(const string &name) {return quans.find(name);};
        std::map<string,Quan>::iterator end() {return quans.end();};
        std::map<string,Quan>::iterator begin() {return quans.begin();};
        unsigned int size() {return quans.size();};
    protected:

    private:
        map<string, Quan> quans;
        string last_error;
};

#endif // QUANSET_H
