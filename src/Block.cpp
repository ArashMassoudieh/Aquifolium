#include "Block.h"
#include "Link.h"
#include "System.h"

Block::Block() : Object::Object()
{
    //ctor
}

Block::~Block()
{
    //dtor
}

Block::Block(const Block& other):Object::Object(other)
{
    Object::operator=(other);
}

Block& Block::operator=(const Block& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    Object::operator=(rhs);
    return *this;
}


void Block::AppendLink(int i, const Expression::loc &loc)
{
    if (loc==Expression::loc::source)
        links_from_ids.push_back(i);

    if (loc==Expression::loc::destination)
        links_to_ids.push_back(i);
}

double Block::GetInflowValue(const string &variable, const Expression::timing &tmg)
{
	if (Variable(variable)->GetCorrespondingInflowVar() != "")
	{
		if (Variable(Variable(variable)->GetCorrespondingInflowVar()))
		{
			double inflow = CalcVal(Variable(variable)->GetCorrespondingInflowVar());
			Variable(Variable(variable)->GetCorrespondingInflowVar())->SetVal(inflow, tmg);
			return inflow;
		}
		else
			return 0;
	}
    return 0;
}

bool Block::deletelinkstofrom(const string& linkname)
{
    if (linkname == "_all")
    {
        links_from_ids.clear();
        links_to_ids.clear();
        return true; 
    }
    for (unsigned int i = 0; i < links_from_ids.size(); i++)
        if (GetLinksFrom()[i]->GetName() == linkname)
        {
            links_from_ids.erase(links_from_ids.begin() + i);
            return true;
        }

    for (unsigned int i = 0; i < links_to_ids.size(); i++)
        if (GetLinksTo()[i]->GetName() == linkname)
        {
            links_to_ids.erase(links_to_ids.begin() + i);
            return true;
        }

    return false; 
}

/*bool Block::AddProperty(const string &s, double val)
{
    if (properties.count(s)>0)
    {
        last_error = "property '" + s + "' already exists!";
        last_operation_success = false;
        return false;
    }

    if (var.count(s)>0)
    {
        last_error = "quantity '" + s + "' already exists!";
        last_operation_success = false;
        return false;
    }
    else
    {
        properties[s] = val;
        last_operation_success = true;
        return true;
    }

}*/



/*double Block::GetProperty(const string& s)
{
    if (properties.count(s)>0)
    {
        return properties[s];
        last_operation_success = true;
    }
    else
    {
        last_error = "property '" + s + "' does not exist!";
        last_operation_success = false;
        return 0;
    }
}*/

vector<Link*> Block::GetLinksFrom() {
    vector<Link* > v;
    for (unsigned int i=0; i<links_from_ids.size(); i++)
        v.push_back(Parent()->link(links_from_ids[i]));

    return v;
}
vector<Link*> Block::GetLinksTo() {
    vector<Link* > v;
    for (unsigned int i=0; i<links_to_ids.size(); i++)
        v.push_back(Parent()->link(links_to_ids[i]));

    return v;

}




