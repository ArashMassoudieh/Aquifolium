#include "Block.h"

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

}

Block& Block::operator=(const Block& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    Object::operator=(rhs);
    return *this;
}


void Block::AppendLink(Link* l, const Expression::loc &loc)
{
    if (loc==Expression::loc::source)
        links_from.push_back(l);

    if (loc==Expression::loc::destination)
        links_to.push_back(l);
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




