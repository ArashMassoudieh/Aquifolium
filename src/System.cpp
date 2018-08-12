#include "System.h"
#include <fstream>
#include <json/json.h>
#pragma warning(disable : 4996)

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
    block(blk.GetName())->SetQuantities(metamodel, blk.GetType());
	return true;
}

bool System::AddLink(Link &lnk, const string &source, const string &destination)
{
    links.push_back(lnk);
    link(lnk.GetName())->SetParent(this);
    link(lnk.GetName())->SetQuantities(metamodel, lnk.GetType());
    link(lnk.GetName())->SetConnectedBlock(Expression::loc::source, source);
    link(lnk.GetName())->SetConnectedBlock(Expression::loc::destination, destination);
    block(source)->AppendLink(link(lnk.GetName()),Expression::loc::source);
    block(destination)->AppendLink(link(lnk.GetName()),Expression::loc::destination);
	return true;
}

Block *System::block(const string &s)
{
    for (int i=0; i<blocks.size(); i++)
        if (blocks[i].GetName() == s) return &blocks[i];

    AppendError("Block " + s + " was not found");
    return nullptr;
}

int System::blockid(const string &s)
{
    for (int i=0; i<blocks.size(); i++)
        if (blocks[i].GetName() == s) return i;

    AppendError("Block " + s + " was not found");
    return -1;
}

int System::linkid(const string &s)
{
    for (int i=0; i<links.size(); i++)
        if (links[i].GetName() == s) return i;

    AppendError("Link " + s + " was not found");
    return -1;
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
	if (!file.good())
	{
		cout << "File " + filename + " was not found!";
		return false;
	}

    file >> root;

    if(!reader.parse(file, root, true)){
            //for some reason it always fails to parse
        std::cout  << "Failed to parse configuration\n"
                   << reader.getFormattedErrorMessages();
        last_error = "Failed to parse configuration\n";
    }


    for (Json::ValueIterator object_types=root.begin(); object_types!=root.end(); ++object_types)
    {
        QuanSet quanset;
        for (Json::ValueIterator it=object_types->begin(); it!=object_types->end(); ++it)
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
            quanset.Append(it.key().asString(),Q);
			AddQnantity(it.key().asString(), Q);
        }
        metamodel.Append(object_types.key().asString(),quanset);
    }
	return true;
}

void System::CopyQuansToMembers()
{
    for (unsigned int i=0; i<blocks.size(); i++)
    {
        blocks[i].SetQuantities(metamodel,blocks[i].GetType());
    }
    for (unsigned int i=0; i<links.size(); i++)
        links[i].SetQuantities(metamodel,blocks[i].GetType());

}

bool System::OneStepSolve()
{
	return true;
}

bool System::OneStepSolve(const string &variable)
{
	renew(variable);
	#ifdef Debug_mode
    cout << "Calculating Residuals" <<endl;
    #endif // Debug_mode
    CVector_arma X = GetStateVariables(variable, Expression::timing::past);
    cout<<"X: " << X.toString()<<endl;
    CVector_arma F = GetResiduals(variable, X);
    cout<<"F: " << F.toString()<<endl;
    double err = F.norm2();
    double err_p = err;
    while (err>solversettings.NRtolerance)
    {
        if (solvertemporaryvars.updatejacobian)
        {
            solvertemporaryvars.Inverse_Jacobian = Invert(Jacobian(variable,X));
            solvertemporaryvars.updatejacobian = false;
        }
        X = X - solvertemporaryvars.NR_coefficient*solvertemporaryvars.Inverse_Jacobian*F;
        F = CVector_arma F = GetResiduals(variable, X);
        err_p = err;
        err = F.norm2();
        if (err>err_p)
            solvertemporaryvars.NR_coefficient*=solversettings.NR_coeff_reduction_factor;

    }
	#ifdef Debug_mode
	CMatrix_arma M = Jacobian("Storage",X);
	M.writetofile("M.txt");
	#endif // Debug_mode
	return true;
}

bool System::renew(const string & variable)
{
	bool out = true;
	for (unsigned int i = 0; i < blocks.size(); i++)
		out &= blocks[i].renew(variable);

	for (unsigned int i = 0; i < links.size(); i++)
		out &= links[i].renew(variable);

	return out;
}

CVector_arma System::GetStateVariables(const string &variable, const Expression::timing &tmg)
{
    CVector_arma X(blocks.size());
    for (unsigned int i=0; i<blocks.size(); i++)
        X[i] = blocks[i].GetVal(variable,tmg);
    return X;
}

void System::SetStateVariables(const string &variable, CVector_arma &X, const Expression::timing &tmg)
{
    for (unsigned int i=0; i<blocks.size(); i++)
    {
        blocks[i].SetVal(variable,X[i],tmg);
        #ifdef Debug_mode
        cout<<"Variable :"<< variable << "in " + blocks[i].GetName() << " was set to " + numbertostring(blocks[i].GetVal(variable,tmg)) << endl;
        #endif // Debug_mode
    }
}


CVector_arma System::GetResiduals(const string &variable, CVector_arma &X)
{
    CVector_arma F(blocks.size());
    SetStateVariables(variable,X,Expression::timing::present);
    CalculateFlows(Variable(variable)->GetCorrespondingFlowVar(),Expression::timing::present);
    for (unsigned int i=0; i<blocks.size(); i++)
    {
        F[i] = (X[i]-blocks[i].GetVal(variable,Expression::timing::past))/dt();
    }

    for (unsigned int i=0; i<links.size(); i++)
    {
        F[links[i].s_Block_No()] += links[i].GetVal(links[i].Variable(variable)->GetCorrespondingFlowVar(),Expression::timing::present);
        F[links[i].e_Block_No()] -= links[i].GetVal(links[i].Variable(variable)->GetCorrespondingFlowVar(),Expression::timing::present);
    }
    return F;
}

bool System::CalculateFlows(const string &var, const Expression::timing &tmg)
{
    for (int i=0; i<links.size(); i++)
    {
        links[i].SetVal(var,links[i].GetVal(var,tmg));
    }
	return true;
}
