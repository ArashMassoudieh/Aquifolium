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
        void AppendLink(Link* l, const Expression::loc &loc);
        double GetInflowValue(const string &variable,const Expression::timing &t);
        bool deletelinkstofrom(const string& linkname); //deletes a specific links from the list of links to and from the block
        vector<Link*> GetLinksFrom() { return links_from; }
        vector<Link*> GetLinksTo() { return links_to; }
    protected:

    private:
        vector<Link*> links_from;
        vector<Link*> links_to;
};

#endif // BLOCK_H
