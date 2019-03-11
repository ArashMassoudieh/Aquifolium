#include "Quan.h"
#include "Block.h"
#include "Link.h"
#include "System.h"

Quan::Quan()
{
    //ctor
}

Quan::~Quan()
{
    //dtor
}

Quan::Quan(const Quan& other)
{
    _expression = other._expression;
    _timeseries = other._timeseries;
    _rule = other._rule;
    _var_name = other._var_name;
    _val = other._val;
    _val_star = other._val_star;
    _parameters = other._parameters;
    type = other.type;
	corresponding_flow_quan = other.corresponding_flow_quan;
	corresponding_inflow_quan = other.corresponding_inflow_quan;
	includeinoutput = other.includeinoutput;
	description = other.description;
    unit = other.unit;
    default_unit = other.default_unit;
    default_val = other.default_val;
    input_type = other.input_type;
    defaults = other.defaults;
    delegate = other.delegate;
    category = other.category;
    input = other.input;
    experiment_dependent = other.experiment_dependent;
    description_code = other.description_code;
    abbreviation = other.abbreviation;
    criteria = other.criteria;
    warning_error = other.warning_error;
    warning_message = other.warning_message;
    ask_from_user = other.ask_from_user;
	//parent = other.parent;
}

Quan& Quan::operator=(const Quan& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    _expression = rhs._expression;
    _timeseries = rhs._timeseries;
    _rule = rhs._rule;
    _var_name = rhs._var_name;
    _val = rhs._val;
    _val_star = rhs._val_star;
    _parameters = rhs._parameters;
    type = rhs.type;
	corresponding_flow_quan = rhs.corresponding_flow_quan;
	corresponding_inflow_quan = rhs.corresponding_inflow_quan;
	includeinoutput = rhs.includeinoutput;
	description = rhs.description;
    unit = rhs.unit;
    default_unit = rhs.default_unit;
    default_val = rhs.default_val;
    input_type = rhs.input_type;
    defaults = rhs.defaults;
    delegate = rhs.delegate;
    category = rhs.category;
    input = rhs.input;
    experiment_dependent = rhs.experiment_dependent;
    description_code = rhs.description_code;
    abbreviation = rhs.abbreviation;
    criteria = rhs.criteria;
    warning_error = rhs.warning_error;
    warning_message = rhs.warning_message;
    ask_from_user = rhs.ask_from_user;
    //parent = rhs.parent;
    return *this;
}

double Quan::CalcVal(Object *block, const Expression::timing &tmg)
{
    if (type == _type::constant)
        return _val;
    if (type == _type::expression)
        return _expression.calc(block,tmg);
    if (type == _type::rule)
        return _rule.calc(block,tmg);
    if (type == _type::timeseries)
    {
        if (_timeseries.n>0)
            return _timeseries.interpol(block->GetParent()->GetTime());
        else
            return 0;
    }
    if (type == _type::value)
        return _val;
    last_error = "Quantity cannot be evaluated";
    return 0;
}

double Quan::GetVal(const Expression::timing &tmg)
{
    if (tmg==Expression::timing::past)
        return _val;
    else
        return _val_star;
}

double Quan::CalcVal(const Expression::timing &tmg)
{

    if (type == _type::constant)
    {
        if (tmg==Expression::timing::past)
            return _val;
        else
            return _val_star;
    }
    if (type == _type::expression)
        return _expression.calc(parent,tmg);
    if (type == _type::rule)
        return _rule.calc(parent,tmg);

    if (type == _type::timeseries)
    {
        if (_timeseries.n>0)
            return _timeseries.interpol(parent->GetParent()->GetTime());
        else
            return 0;
    }
    if (type == _type::value)
    {
        if (tmg==Expression::timing::past)
            return _val;
        else
            return _val_star;
    }
    if (type == _type::balance)
    {
        if (tmg==Expression::timing::past)
            return _val;
        else
            return _val_star;
    }
    last_error = "Quantity cannot be evaluated";
    return 0;
}

bool Quan::SetExpression(const string &E)
{
    _expression = E;
	return true;
}

bool Quan::SetRule(const string &R)
{
    _expression = R;
	return true;
}

string tostring(const Quan::_type &typ)
{
    if (typ==Quan::_type::balance) return "Balance";
    if (typ==Quan::_type::constant) return "Constant";
    if (typ==Quan::_type::expression) return "Expression";
    if (typ==Quan::_type::global_quan) return "Global";
    if (typ==Quan::_type::rule) return "Rule";
    if (typ==Quan::_type::timeseries) return "TimeSeries";
    if (typ==Quan::_type::value) return "Value";
    return "";
}


bool Quan::SetVal(const double &v, const Expression::timing &tmg)
{
    if (tmg == Expression::timing::past)
        _val = v;
    else if (tmg == Expression::timing::present)
        _val_star = v;
    else if (tmg == Expression::timing::both)
    {
        _val = v;
        _val_star = v;
    }
	return true;
}

Expression* Quan::GetExpression()
{
    return &_expression;
}

Rule* Quan::GetRule()
{
    return &_rule;
}


string Quan::ToString(int _tabs)
{
    string out = tabs(_tabs) + _var_name + ":\n";
    out += tabs(_tabs) + "{\n";
    if (type==_type::constant)
        out += tabs(_tabs+1) + "type: constant\n";
    if (type==_type::balance)
        out += tabs(_tabs+1) + "type: balance\n";
    if (type==_type::expression)
        out += tabs(_tabs+1) + "type: expression\n";
    if (type==_type::constant)
        out += tabs(_tabs+1) + "type: constant\n";
    if (type==_type::global_quan)
        out += tabs(_tabs+1) + "type: global_quantity\n";
    if (type==_type::timeseries)
        out += tabs(_tabs+1) + "type: time_series\n";
    if (type==_type::value)
        out += tabs(_tabs+1) + "type: value\n";

    if (type==_type::expression)
        out += tabs(_tabs+1) + "expression: " + _expression.ToString() + "\n";

    if (type==_type::rule)
        out += tabs(_tabs+1) + "rule: " + _rule.ToString(_tabs) + "\n";

    out += tabs(_tabs+1) + "val: ";
    out +=  numbertostring(_val);
    out += string("\n");
    out += tabs(_tabs+1) + "val*:";
    out += numbertostring(_val_star);
    out += string("\n");
    out += tabs(_tabs) + "}";
    return out;
}

void Quan::SetCorrespondingFlowVar(const string &s)
{
    corresponding_flow_quan = s;
}

void Quan::SetCorrespondingInflowVar(const string &s)
{
    corresponding_inflow_quan = s;
}



void Quan::SetMassBalance(bool on)
{
    perform_mass_balance = on;
}

void Quan::SetParent(Object *o)
{
    parent = o;
}

void Quan::Renew()
{
	_val_star = _val;
}

void Quan::Update()
{
	_val = _val_star;
}

bool Quan::SetTimeSeries(string filename)
{
	_timeseries.readfile(filename);
	if (_timeseries.file_not_found)
	{
		AppendError(GetName(),"Quan", "SetTimeSeries", filename + " was not found!", 3001);
		return false;
	}
	else
		return true;
}

bool Quan::AppendError(const string &objectname, const string &cls, const string &funct, const string &description, const int &code)
{
    if (!parent)
        return false;
    if (!parent->Parent())
        return false;
    parent->Parent()->errorhandler.Append(objectname, cls, funct, description, code);
    return true;
}
