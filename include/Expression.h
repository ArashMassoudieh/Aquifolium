#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <string>
#include <vector>

class Block;

using namespace std;

class Expression
{
    public:
        virtual ~Expression();
        Expression(void);
        Expression(string S);
        Expression(const Expression &S);
        Expression& operator=(const Expression&);
        vector<string> operators;
        vector<Expression> terms;
        string sign;
        string function;
        string parameter;
        double constant;
        string param_constant_expression;
        vector<string> extract_operators(string s);
        vector<string> _errors;
        vector<string> extract_terms(string s);
        double calc(Block * W);
        double func(string & f, double val);
        double func(string & f, double val1, double val2);
        double oprt(string & f, double val1, double val2);
        double oprt(string & f, int i1, int i2, Block * W);
        vector<string> funcs;
        string unit;
        string text;
        vector<string> opts;

    protected:

    private:
        vector<double> term_vals;
        vector<bool> terms_calculated;
        vector<vector<int> > sources;
};

#endif // EXPRESSION_H


int corresponding_parenthesis(string S, int i);
bool parantheses_balance(string S);
bool contains(const string &s, const string &s1);
string left(const string &s, int i);
string right(const string &s, int i);
int count(const string &s, const string &s1);
int lookup(const vector<string> &s, const string &s1);
int lookup(const vector<int> &s, const int &s1);
int lookup(const vector<vector<int> > &s, const vector<int> &s1);
void remove(string &s,unsigned int i);
void remove(string &s,unsigned int pos, unsigned int len);
bool isnumber(char S);
bool isnumber(string S);
bool isintegernumber(string S);
double atof(const string &S);
double atoi(const string &S);
string trim(const string &s);
void push_back(vector<string> &s, const vector<string> &s1);
void replace(string &s,unsigned int i,string p);
void replace(string &s,unsigned int i, unsigned int j,string p);
bool replace(string &s,string s1, string s2);
bool remove(string &s, string s1);
void insert(string &s, unsigned int pos, string s1);
vector<string> split(const string &s, char del);
