#include "Expression.h"

#include "Expression.h"
//#include "utility_funcs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <iostream>

Expression::Expression(void)
{
	funcs.push_back("_min");
	funcs.push_back("_max");
	funcs.push_back("_exp");
	funcs.push_back("_log");
	funcs.push_back("_abs");
	funcs.push_back("_sqr");
	opts.push_back("+");
	opts.push_back("-");
	opts.push_back("*");
	opts.push_back(";");
	opts.push_back("/");
	opts.push_back("^");

}

Expression::Expression(string S)
{
	text = S;
	funcs.push_back("_min");
	funcs.push_back("_max");
	funcs.push_back("_exp");
	funcs.push_back("_log");
	funcs.push_back("_abs");
	funcs.push_back("_sqr");
	opts.push_back("+");
	opts.push_back("-");
	opts.push_back("*");
	opts.push_back(";");
	opts.push_back("/");
	opts.push_back("^");

	vector<string> out;
	//bool inside_quote = false;
	unsigned int paranthesis_level = 0;
	unsigned int last_operator_location = -1;
	if (!parantheses_balance(S))
	{
		_errors.push_back("Parantheses do not match in" + S);
		return;
	}
	if (lookup(funcs,left(S,4))!=-1)
	{
		function = right(left(S,4),3);
	}
	if (left(S,1) == "(")
	{
		if (corresponding_parenthesis(S,0) == -1 )
		{
			_errors.push_back("Parantheses do not match in" + S);
		}
		else if (corresponding_parenthesis(S,0) == int(S.size()-1))
		{
			remove(S,0);
			remove(S,S.size() - 1);
			//if (opts.contains(S.left(1)))
			//	terms.append(CExpression("0"));
			if (lookup(funcs,left(S,4))!=-1)
			{
				function = right(left(S,4),3);
			}

		}
	}
	if (isnumber(S))
	{
		param_constant_expression = "constant";
		constant = atof(S);
	}
	else
	{
		if (contains(S,"(") || contains(S,")") || contains(S,"+") || contains(S,"-") || contains(S,"*") || contains(S,"/") || contains(S,"^") || contains(S,";"))
		{
			param_constant_expression = "expression";
			for (unsigned int i = 0; i < S.size(); i++)
			{
				if (S.substr(i, 1) == "(")
					paranthesis_level++;

				if (S.substr(i, 1) == ")")
					paranthesis_level--;

				if (paranthesis_level == 0)
					if ((S.substr(i, 1) == "+") || (S.substr(i, 1) == "-") || (S.substr(i, 1) == "*") || (S.substr(i, 1) == "/") || (S.substr(i, 1) == "^"))
					{
						operators.push_back(S.substr(i, 1));
						Expression sub_exp = Expression(trim(S.substr(last_operator_location+1, i -1- last_operator_location)));
						if (sub_exp.text != "")
						{
							if (operators.size() > 1)
								sub_exp.sign = operators[operators.size() - 2];
							else
								sub_exp.sign = "+";
							terms.push_back(sub_exp);
						}
						else
						{
							sub_exp = Expression("0");
							sub_exp.sign = "+";
							terms.push_back(sub_exp);
						}
						push_back(_errors,sub_exp._errors);
						last_operator_location = i;
					}
			}

			Expression sub_exp = Expression(trim(S.substr(last_operator_location+1, S.size() - last_operator_location)));
			if (operators.size() > 0)
				sub_exp.sign = operators[operators.size() - 1];
			else
				sub_exp.sign = "+";
			terms.push_back(sub_exp);
			push_back(_errors,sub_exp._errors);

		}
		else
		{
			param_constant_expression = "parameter";
			parameter = S;
		}
	}



}

Expression::Expression(const Expression & S)
{
	operators = S.operators;
	constant = S.constant;
	terms = S.terms;
	sign = S.sign;
	funcs = S.funcs;
	opts = S.opts;
	function = S.function;
	parameter = S.parameter;
	param_constant_expression = S.param_constant_expression;
	unit = S.unit;
	text = S.text;
}

Expression & Expression::operator=(const Expression &S)
{
	operators = S.operators;
	constant = S.constant;
	terms = S.terms;
	sign = S.sign;
	funcs = S.funcs;
	opts = S.opts;
	function = S.function;
	parameter = S.parameter;
	param_constant_expression = S.param_constant_expression;
	unit = S.unit;
	text = S.text;

	return *this;
}


Expression::~Expression(void)
{
}

vector<string> Expression::extract_operators(string s)
{
	vector<string> out;
	//bool inside_quote = false;
	int paranthesis_level = 0;
	for (unsigned int i = 0; i < s.size(); i++)
	{
		if (s.substr(i, 1) == "(")
			paranthesis_level++;

		if (s.substr(i, 1) == ")")
			paranthesis_level--;

		if (paranthesis_level == 0)
			if ((s.substr(i, 1) == "+") || (s.substr(i, 1) == "-") || (s.substr(i, 1) == "*") || (s.substr(i, 1) == "/") || (s.substr(i, 1) == "^")) out.push_back(s.substr(i, 1));


	}

	return out;
}

vector<string> Expression::extract_terms(string s)
{
	bool inside_quote = false;
	for (unsigned int i = 0; i < s.size(); i++)
	{
		if (s.substr(i, 1) == "'")
		{
			inside_quote = !inside_quote;
			remove(s, i);
		}
		if (inside_quote)
			if (s.substr(i, 1) == " ") replace(s, i, 1, "|");

	}
	vector<string> out = split(s,' ');
	for (unsigned int i = 0; i < out.size(); i++) replace(out[i],"|", " ");
	return out;
}

int lookup(const vector<string> &s, const string &s1)
{
    for (unsigned int i=0; i<s.size(); i++)
        if (s[i]==s1)
            return i;
    return -1;
}

int lookup(const vector<int> &s, const int &s1)
{
    for (unsigned int i=0; i<s.size(); i++)
        if (s[i]==s1)
            return i;
    return -1;
}

int lookup(const vector<vector<int> > &s, const vector<int> &s1)
{
    for (unsigned int i=0; i<s.size(); i++)
        if (s[i]==s1)
            return i;
    return -1;
}


/*double Expression::calc(Block *W)
{
	if (this->param_constant_expression == "constant")
		return constant;
	if (this->param_constant_expression == "parameter")
	{
		XString out;
		out = W->get_parameter_value(parameter);
		if (W->get_parameter_value(parameter).unit != "" && unit == "")
			unit = W->get_parameter_value(parameter).unit;
		return out.toDouble();
	}
	if (this->param_constant_expression == "expression")
	{

		for (int i = 0; i < terms.size(); i++)
		{
			sources.append(QList<int>());
			sources[i].append(i);
		}
		term_vals.resize(terms.size());
		for (int i = 0; i < terms.size(); i++) terms_calculated.push_back(false);
		for (int i = operators.size() - 1; i >= 0; i--)
		{
			if (operators[i] == "^")
				oprt(operators[i], i, i + 1, W);
		}
		for (int i = operators.size() - 1; i >= 0; i--)
		{
			if (operators[i] == "*")
				oprt(operators[i], i, i + 1, W);
		}

		for (int i = operators.size() - 1; i >= 0; i--)
		{
			if (operators[i] == "/")
				oprt(operators[i], i, i + 1, W);
		}

		for (int i = operators.size() - 1; i >= 0; i--)
		{
			if (operators[i] == "+")
				oprt(operators[i], i, i + 1, W);

		}

		for (int i = operators.size() - 1; i >= 0; i--)
		{
			if (operators[i] == "-")
			{
				oprt(operators[i], i, i + 1, W);
			}
		}
		if (function == "")
			return term_vals[0];
		else if (!operators.contains(";"))
			return func(function, term_vals[0]);
		else
			return func(function, term_vals[0], term_vals[1]);
	}

}
*/
double Expression::func(string &f, double val)
{

	if (f == "_exp")
		return exp(val);
	if (f == "_log")
		return log(val);
	if (f == "_abs")
		return fabs(val);
	if (f == "_sqr")
		return sqrt(val);
	return val;
}

double Expression::func(string &f, double val1, double val2)
{
	if (f == "_min")
		return min(val1, val2);
	if (f == "_max")
		return max(val1, val2);
	return val1;
}

double Expression::oprt(string &f, double val1, double val2)
{
	if (f == "^") return pow(val1, val2);
	if (f == "+") return val1 + val2;
	if (f == "-") return val1 + val2;
	if (f == "/") return val1 * val2;
	if (f == "*") return val1*val2;
	return 0;
}

/*double Expression::oprt(string &f, int i1, int i2, Block *W)
{

	for (unsigned int j = 0; j < sources[i1].size(); j++)
	{
		if (sources.size() > i2)
			for (int k=0; k<sources[i2].size(); k++)
				if (lookup(sources[sources[i2][k]],sources[i1][j])==-1) sources[sources[i2][k]].push_back(sources[i1][j]);

	}
	if (sources.size() > i2)
	for (int j = 0; j < sources[i2].size(); j++)
	{
		for (int k = 0; k<sources[i1].size(); k++)
			if (lookup(sources[sources[i1][k]],sources[i2][j])==-1) sources[sources[i1][k]].push_back(sources[i2][j]);

	}

	double val1;
	double val2;
	if (terms_calculated[i1]) val1 = term_vals[i1]; else val1 = terms[i1].calc(W);
	if (terms[i1].sign == "/") val1 = 1/val1;
	if (terms[i1].sign == "-") val1 = -val1;
	if (sources.size() > i2)
		if (terms_calculated[i2]) val2 = term_vals[i2]; else
		{
			val2 = terms[i2].calc(W);
			if (terms[i2].sign == "/") val2 = 1 / val2;
			if (terms[i2].sign == "-") val2 = -val2;
		}
	else
	{
		val1 = 0;
		val2 = val1;
	}

	if (unit == "")
	{
		if (terms[i1].unit != "") unit = terms[i1].unit;
		else if (terms[i2].unit != "") unit = terms[i2].unit;
	}

	for (int j = 0; j<sources[i1].size(); j++)
		term_vals[sources[i1][j]] = oprt(f, val1, val2);

	terms_calculated[i1] = true;
	terms_calculated[i2] = true;

	return term_vals[sources[i1][0]];
}
*/
int corresponding_parenthesis(string S, int i)
{
	string s = S;
    if (S.at(i) == '(')
	{
		int paranthesis_level = 1;
		for (unsigned int j = i+1; j < S.size(); j++)
		{
            if (S.at(j) == '(')
				paranthesis_level++;
            if (S.at(j) == ')')
				paranthesis_level--;

			if (paranthesis_level == 0)
				return j;
		}
		return -1;
	}


    if (S.at(i) == ')')
	{
		int paranthesis_level = 1;
		for (int j = i-1; j > 0; j--)
		{
            if (S.at(j) == ')')
				paranthesis_level++;
            if (S.at(j) == '(')
				paranthesis_level--;

			if (paranthesis_level == 0)
				return j;
		}
		return -1;
	}
	return -1;
}

bool parantheses_balance(string S)
{
	if (count(S,"(") == count(S,")"))
		return true;
	else
		return false;
}

bool contains(const string &s, const string &s1)
{
    for (unsigned int i=0; i<s.size()-s1.size()+1; i++)
        if (s.substr(i,s1.size())==s1)
            return true;
    return false;
}

int count(const string &s, const string &s1)
{
    int out=0;
    for (unsigned int i=0; i<s.size()-s1.size()+1; i++)
        if (s.substr(i,s1.size())==s1)
            out++;
    return out;
}


string left(const string &s, int i)
{
    return s.substr(0,i);
}
string right(const string &s, int i)
{
    return s.substr(s.size()-i-1,i);
}

void remove(string &s,unsigned int i)
{
    string S;
    for (unsigned int j=0; j<s.size(); j++)
        if (i!=j)
            S = S + s[j];
    s = S;
}

void replace(string &s,unsigned int i,string p)
{
    string S;
    for (unsigned int j=0; j<s.size(); j++)
        if (i!=j)
            S = S + s[j];
        else
            S = S + p;
    s = S;
}

bool replace(string &s,string s1, string s2)
{

    bool found = false;
    vector<int> pos;
    unsigned int j=0;
    while (j<s.size()-s1.size()+1)
    {
        if (s.substr(j,s1.size())==s1)
        {
            pos.push_back(j);
            remove(s,j,s1.size());
            found = true;
        }
        j++;
    }
    for (unsigned int j=0; j<pos.size(); j++)
    {
        insert(s,pos[j]+j*s2.size(),s2);
    }

    return found;
}

bool remove(string &s, string s1)
{
    bool found = false;
    for (unsigned int j=0; j<s.size()-s1.size()+1; j++)
        if (s.substr(j,s1.size())==s1)
        {
            remove(s,j,s1.size());
            found = true;
        }
    return found;
}

void insert(string &s, unsigned int pos, string s1)
{
    string S;

    for (unsigned int i=0; i<s.size(); i++)
    {
        if (i==pos)
            S = S + s1;
        S = S + s[i];
    }
    if (pos==s.size()) S=S+s1;
    s = S;
}

void replace(string &s,unsigned int i, unsigned int j, string p)
{
    string S;
    for (unsigned int k=0; k<s.size(); k++)
        if (k==i)
            S = S + p;
        else if (k<i || k>=i+j)
            S = S + s[j];

    s = S;
}


void remove(string &s,unsigned int pos, unsigned int len)
{
    for (unsigned int i=pos; i<pos+len; i++)
        remove(s, pos);
}

bool isnumber(char S)
{
	if ((((int)S > 47) && ((int)S < 58)) || (S=='.'))
		return true;
	else
		return false;
}

bool isnumber(string S)
{
	bool res = true;
	for (unsigned int i = 0; i < S.size(); i++)
		if (!isnumber(S[i]))
			res = false;

	return res;
}


bool isintegernumber(string S)
{
	bool out = true;
	for (unsigned int i = 0; i < S.size(); i++)
	{
		if (((int)S[i] <= 47) || ((int)S[i] >= 58))
			out = false;
	}
	return out;
}

double atof(const string &S)
{
    return atof(S.c_str());
}
double atoi(const string &S)
{
    return atoi(S.c_str());
}

string trim(const string &s)
{
	if (s.find_first_not_of(' ') == string::npos) return "";

	return s.substr( s.find_first_not_of(' '), s.find_last_not_of(' ') + 1 );
}

void push_back(vector<string> &s, const vector<string> &s1)
{
    for (unsigned int i=0; i<s1.size(); i++)
        s.push_back(s1[i]);
}

vector<string> split(const string &s, char del)
{
	unsigned int lastdel=0;
	vector<string> strings;
	for (unsigned int i=0; i<s.size(); i++)
	{
		if (s[i]==del)
		{
			strings.push_back(s.substr(lastdel, i-lastdel));
			lastdel = i+1;
		}
	}
    if (lastdel<s.size() && trim(s.substr(lastdel, s.size()-lastdel))!="\r" && trim(s.substr(lastdel, s.size() - lastdel)) != "") strings.push_back(trim(s.substr(lastdel, s.size()-lastdel)));  // works w/o trim- Trim can be deleted
	for (unsigned int i=0; i<strings.size(); i++) strings[i] = trim(strings[i]);					// Trim can be deleted
	if (strings.size()==1)
        if (strings[0]=="")
            strings.pop_back();
	return strings;

}




