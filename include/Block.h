#ifndef BLOCK_H
#define BLOCK_H
#include <map>
#include <string>
#include "Expression.h"
#include "Quan.h"
#include "Object.h"

using namespace std;

class System;
class Link;

class Block: public Object
{
    public:
        Block();
        Block(System *parent);
        Block(const Block& other);
        Block& operator=(const Block& rhs);
        virtual ~Block();
        void AppendLink(int i, const Expression::loc &loc);
        double GetInflowValue(const string &variable,const Expression::timing &t);
        bool deletelinkstofrom(const string& linkname="_all"); //deletes a specific links from the list of links to and from the block
        vector<Link*> GetLinksFrom();
        vector<Link*> GetLinksTo();

    protected:

    private:
        vector<int> links_from_ids;
        vector<int> links_to_ids;
};

#endif // BLOCK_H
