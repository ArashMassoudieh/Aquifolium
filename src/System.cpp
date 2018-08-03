#include "System.h"
#include <fstream>
#include <json/json.h>

System::System():Object::Object()
{
    //ctor
}

System::~System()
{
    //dtor
}

System::System(const System& other):Object::Object(other)
{
    blocks = other.blocks;
    links = other.links;
}

System& System::operator=(const System& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    Object::operator=(rhs);
    blocks = rhs.blocks;
    links = rhs.links;
    return *this;
}

bool System::AddBlock(Block &blk)
{
    blocks.push_back(blk);
    block(blk.GetName())->SetParent(this);
    block(blk.GetName())->SetQuantities(quan_template);
}

bool System::AddLink(Link &lnk, const string &source, const string &destination)
{
    links.push_back(lnk);
    link(lnk.GetName())->SetParent(this);
    link(lnk.GetName())->SetQuantities(quan_template);
    link(lnk.GetName())->SetConnectedBlock(Expression::loc::source, source);
    link(lnk.GetName())->SetConnectedBlock(Expression::loc::destination, destination);
    block(source)->AppendLink(link(lnk.GetName()),Expression::loc::source);
    block(destination)->AppendLink(link(lnk.GetName()),Expression::loc::destination);
}

Block *System::block(const string &s)
{
    for (int i=0; i<blocks.size(); i++)
        if (blocks[i].GetName() == s) return &blocks[i];

    AppendError("Block " + s + " was not found");
    return nullptr;
}


Link *System::link(const string &s)
{
    for (int i=0; i<links.size(); i++)
        if (links[i].GetName() == s) return &links[i];

    AppendError("Link " + s + " was not found");
    return nullptr;
}

bool System::GetQuanTemplate(string filename)
{
    Json::Value root;
    Json::Reader reader;

    std::ifstream file(filename);
    cout<<file.good()<<endl;
    file >> root;

    if(!reader.parse(file, root, true)){
            //for some reason it always fails to parse
        std::cout  << "Failed to parse configuration\n"
                   << reader.getFormattedErrorMessages();
        last_error = "Failed to parse configuration\n";
    }



    for (Json::ValueIterator it=root.begin(); it!=root.end(); ++it)

    {
        Quan Q;
        if ((*it)["type"].asString()=="balance")
        {
            Q.SetType(Quan::_type::balance);
            Q.SetCorrespondingFlowVar((*it)["flow"].asString());
        }
        if ((*it)["type"].asString()=="constant")
            Q.SetType(Quan::_type::constant);
        if ((*it)["type"].asString()=="expression")
        {
            Q.SetType(Quan::_type::expression);
            Q.SetExpression((*it)["expression"].asString());
        }
        if ((*it)["type"].asString()=="global")
            Q.SetType(Quan::_type::global_quan);
        if ((*it)["type"].asString()=="timeseries")
            Q.SetType(Quan::_type::timeseries);
        if ((*it)["type"].asString()=="value")
            Q.SetType(Quan::_type::value);

        cout<<it.key().asString()<<endl;
        quan_template[it.key().asString()] = Q;
    }

}

void System::CopyQuansToMembers()
{
    for (unsigned int i=0; i<blocks.size(); i++)
    {
        blocks[i].SetQuantities(quan_template);
    }
    for (unsigned int i=0; i<links.size(); i++)
        links[i].SetQuantities(quan_template);

}

bool System::OneStepSolve()
{

}

CVector_arma System::GetStateVariables(const string &variable, const Expression::timing &tmg)
{
    CVector_arma X(blocks.size());
    for (unsigned int i=0; i<blocks.size(); i++)
        X[i] = blocks[i].GetVal(variable,tmg);
}

void System::SetStateVariables(const string &variable, CVector_arma &X, const Expression::timing &tmg)
{
    for (unsigned int i=0; i<blocks.size(); i++)
        blocks[i].SetVal(variable,X[i],tmg);
}


CVector_arma System::GetResiduals(const string &variable, CVector_arma &X)
{
    CVector_arma F(blocks.size());
    SetStateVariables(variable,X,Expression::timing::present);
    CalculateFlows(Variable(variable)->GetCorrespondingFlowVar(),Expression::timing::present);
    for (unsigned int i=0; i<blocks.size(); i++)
    {
        F[i] = (blocks[i].GetVal(variable) - X[i])/dt();
    }
}

bool System::CalculateFlows(const string &var, const Expression::timing &tmg)
{
    for (int i=0; i<links.size(); i++)
    {
        links[i].SetVal(var,links[i].GetVal(var,tmg));
    }
}
