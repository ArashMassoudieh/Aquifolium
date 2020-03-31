#include "Object.h"
#include "System.h"
#include <string>

Object::Object()
{
    //ctor
}

Object::~Object()
{
    //dtor
}

Object::Object(const Object& other)
{
    var = other.var;
    setting = other.setting;
    name = other.GetName();
    parent = other.GetParent();
	s_Block_no = other.s_Block_no;
	e_Block_no = other.e_Block_no;
	outflowlimitfactor_current = 1;
	outflowlimitfactor_past = 1;
	type = other.type;
	limitoutflow = false;
	SetAllParents();
}

Object& Object::operator=(const Object& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    setting = rhs.setting;
    name = rhs.GetName();
    parent = rhs.GetParent();
    var = rhs.var;
	s_Block_no = rhs.s_Block_no;
	e_Block_no = rhs.e_Block_no;
	type = rhs.type;
    SetAllParents();
	outflowlimitfactor_current = 1; 
	outflowlimitfactor_past = 1;
	limitoutflow = false; 
    return *this;
}

double Object::CalcVal(const string& s,const Expression::timing &tmg)
{
    if (var.Count(s)==1)
    {
        #ifdef Debug_mode
        //ShowMessage(string("Object: ") + name + " Variable: " + s + " Value: " + numbertostring(var[s].CalcVal(tmg)));
        #endif // Debug_mode
		return var[s].CalcVal(tmg);
    }
    else
    {
        if (parent)
        Parent()->errorhandler.Append(GetName(),"Object","CalcVal","property '" + s + "' does not exist!",1001);
        last_operation_success = false;
        return 0;
    }

}

double Object::GetVal(const string& s,const Expression::timing &tmg, bool limit)
{
    if (var.Count(s)==1)
    {
        if (!limit)
            return var[s].GetVal(tmg);
        else
            return var[s].GetVal(tmg)*GetOutflowLimitFactor(tmg);
    }
    else
    {
        Parent()->errorhandler.Append(GetName(),"Object","GetVal","property '" + s + "' does not exist!",1002);
        last_operation_success = false;
        return 0;
    }

}


bool Object::AddQnantity(const string &name,const Quan &Q)
{
    if (var.find(name)!=var.end() && !var.size())
    {
        Parent()->errorhandler.Append(GetName(),"Object","AddQnantity","Variable " + name + " already exists! ",1003);
        return false;
    }
    else
    {
		var.Append(name, Q);
        return true;
    }

}

bool Object::SetQuantities(MetaModel &m, const string& typ )
{
    if (m.Count(typ)==0)
    {
        Parent()->errorhandler.Append(GetName(),"Object","SetQuantities","Type " + typ + "was not found!",1004);
        last_error = "Type " + typ + "was not found";
		return false;
	}
    else
        var = *m[typ];
    SetDefaults();
    for (map<string, Quan>::const_iterator s = var.begin(); s != var.end(); ++s)
        var[s->first].SetParent(this);
	return true;
}

bool Object::SetQuantities(MetaModel *m, const string& typ )
{
    if (m->Count(typ)==0)
    {
        Parent()->errorhandler.Append(GetName(),"Object","SetQuantities","Type " + typ + "was not found!",1004);
        last_error = "Type " + typ + "was not found";
        return false;
    }
    else
        var = *(m->GetItem(typ));
    SetDefaults();
    for (map<string, Quan>::const_iterator s = var.begin(); s != var.end(); ++s)
        var[s->first].SetParent(this);
    return true;
}

void Object::SetDefaults()
{
    for (map<string, Quan>::const_iterator s = var.begin(); s != var.end(); ++s)
        var[s->first].SetVal(atof(var[s->first].Default().c_str()),Expression::timing::both);
}

bool Object::SetVal(const string& s, double value, const Expression::timing &tmg)
{
    if (var.find(s)!=var.end())
    {
        var[s].SetVal(value,tmg);
        return true;
    }
    else
    {
        Parent()->errorhandler.Append(GetName(),"Object","SetVal","Variable " + s + " was not found!",1005);
        last_error = "Variable " + s + " was not found!";
        return false;
    }
}

bool Object::SetVal(const string& s, const string & value, const Expression::timing &tmg)
{
    if (var.find(s)!=var.end())
    {
        var[s].SetVal(aquiutils::atof(value),tmg);
        return true;
    }
    else
    {
        Parent()->errorhandler.Append(GetName(),"Object","SetVal","Variable " + s + " was not found!",1006);
        last_error = "Variable " + s + " was not found!";
        return false;
    }
}

string Object::GetName() const
{
    return name;
}

bool Object::SetName(const string &s, bool setprop)
{
    if (setprop)
        var["Name"].SetProperty(s);
    name = s;
    return true;
}


Object* Object::GetConnectedBlock(Expression::loc l)
{
    if (l==Expression::loc::destination)
        return e_Block;
    if (l==Expression::loc::source)
        return s_Block;
    if (l==Expression::loc::self)
        return this;

}

void Object::SetConnectedBlock(Expression::loc l, const string &blockname)
{
    if (GetParent()->block(blockname)==nullptr)
    {
        Parent()->errorhandler.Append(GetName(),"Object","SetConnectedBlock","Block '" +blockname + "' does not exist!",1008);
        last_error = "Block '" +blockname + "' does not exist!";
        GetParent()->AppendError(last_error);
    }
    else
    {
        if (l==Expression::loc::source)
        {
            s_Block = GetParent()->block(blockname);
            s_Block_no = GetParent()->blockid(blockname);
        }
        if (l==Expression::loc::destination)
        {
            e_Block = GetParent()->block(blockname);
            e_Block_no = GetParent()->blockid(blockname);
        }
    }

}

void Object::AppendError(const string &s)
{
	errors.push_back(s);
    last_error = s;
}

void Object::SetParent(System *s)
{
    parent = s;
    SetAllParents();
}

Quan* Object::CorrespondingFlowVariable(const string &s)
{
    if (var.Count(Variable(s)->GetCorrespondingFlowVar())==0)
    {
        Parent()->errorhandler.Append(GetName(),"Object","CorrespondingFlowVariable","Variable '" + s +"' does not exist!",1009);
        return nullptr;
    }
    else
        return Variable(Variable(s)->GetCorrespondingFlowVar());
}

Quan* Object::Variable(const string &s)
{
    if (var.Count(s)==0)
    {
#ifdef Debug_mode
		ShowMessage("In '" + name + "': " + "Variable '" + s + "' does not exist!");
#endif
		//Parent()->errorhandler.Append(GetName(),"Object","Variable","Variable '" + s +"' does not exist!",1010);
		return nullptr;
    }
    else
        return &var[s];
}

bool Object::HasQuantity(const string &q)
{
    if (var.Count(q)==0)
        return false;
    else
        return true;

}

bool Object::Renew(const string & variable)
{
	if (!Variable(variable))
	{
		Parent()->errorhandler.Append(GetName(),"Object","Renew","Variable '" + variable + "' does not exist!",1011);
		return false;
	}
	else
		Variable(variable)->Renew();
	return true;

}

bool Object::Update(const string & variable)
{
	if (!Variable(variable))
	{
		Parent()->errorhandler.Append(GetName(),"Object","Update","Variable '" + variable + "' does not exist!",1011);
		return false;
	}
	else
		Variable(variable)->Update();
	return true;

}

bool Object::CalcExpressions(const Expression::timing &tmg)
{
	for (map<string, Quan>::const_iterator s = var.begin(); s != var.end(); ++s)
		if (var[s->first].GetType() == Quan::_type::expression)
			Variable(s->first)->SetVal(Variable(s->first)->CalcVal(tmg),tmg);
	return true; 
}

void Object::SetVariableParents()
{
	var.SetParent(this);
	for (map<string, Quan>::const_iterator s = var.begin(); s != var.end(); ++s)
	{
		var[s->first].SetParent(this);
		if (var[s->first].GetType() == Quan::_type::source)
			var[s->first].SetProperty(var[s->first].SourceName());
	}
}

void Object::ShowMessage(const string &msg) {if (!parent->IsSilent()) cout<<msg<<std::endl;}

void Object::SetAllParents()
{
    var.SetParent(this);

}

bool Object::SetProperty(const string &prop, const string &value)
{
    if (!HasQuantity(prop))
    {
        Parent()->errorhandler.Append(GetName(),"Object","SetProperty","Object '" + GetName() + "' has no property called '" + prop + "'",1012);
        return false;
    }
    if (var[prop].GetType() == Quan::_type::value || var[prop].GetType() == Quan::_type::balance || var[prop].GetType() == Quan::_type::constant)
    {
        var[prop].SetVal(aquiutils::atof(value),Expression::timing::both);
        return true;
    }
    if (var[prop].GetType() == Quan::_type::expression)
    {
        Parent()->errorhandler.Append(GetName(),"Object","SetProperty","In object '" + GetName() + "', property '" + prop + "' is an expression and cannot be set during the run time",1013);
        return false;
    }
    if (var[prop].GetType() == Quan::_type::rule)
    {
        Parent()->errorhandler.Append(GetName(),"Object","SetProperty","In object '" + GetName() + "', property '" + prop + "' is a rule and cannot be set during the run time",1014);
        return false;
    }

    if (var[prop].GetType() == Quan::_type::timeseries || var[prop].GetType() == Quan::_type::prec_timeseries)
    {
        return var[prop].SetProperty(value);
    }
    if (var[prop].GetType() == Quan::_type::source)
    {
        return var[prop].SetProperty(value);
    }
    if (var[prop].GetType() == Quan::_type::string)
    {
        return var[prop].SetProperty(value);
    }


}

string Object::toString(int _tabs)
{
    string out = aquiutils::tabs(_tabs) + "Name: " + GetName() + "\n";
    out += aquiutils::tabs(_tabs) + "Type: " + GetType() + "\n";
    out += this->var.ToString(_tabs + 1);
    return out;
}
