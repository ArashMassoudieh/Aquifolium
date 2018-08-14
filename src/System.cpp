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
            if (it->isMember("includeinoutput"))
            {
                if ((*it)["includeinoutput"].asString()=="true")
                    Q.SetIncludeInOutput(true);
                else
                    Q.SetIncludeInOutput(false);
            }
            else
                Q.SetIncludeInOutput(false);
            //cout<<it.key().asString()<<endl;
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

bool System::Solve(const string &variable)
{
    InitiateOutputs();
    PopulateOutputs();
    SolverSettings.dt = SimulationParameters.dt0;
    SolverSettings.t = SimulationParameters.tstart;
    while (SolverSettings.t<SimulationParameters.tend+SolverSettings.dt)
    {
        #ifdef Debug_mode
        cout << "t = " << SolverSettings.t << ", dt = " << SolverSettings.dt << ", SolverTempVars.numiterations =" << SolverTempVars.numiterations << endl;
        #endif // Debug_mode
        bool success = OneStepSolve(variable);
        if (!success)
        {
            #ifdef Debug_mode
            cout<<"failed!"<<endl;
            #endif // Debug_mode
            SolverSettings.dt *= SolverSettings.NR_timestep_reduction_factor_fail;
            SolverTempVars.updatejacobian = true;
        }
        else
        {
            SolverSettings.t += SolverSettings.dt;
            if (SolverTempVars.numiterations>SolverSettings.NR_niteration_upper)
            {
                SolverSettings.dt = max(SolverSettings.dt*SolverSettings.NR_timestep_reduction_factor,SolverSettings.minimum_timestep);
                SolverTempVars.updatejacobian = true;
            }
            if (SolverTempVars.numiterations<SolverSettings.NR_niteration_lower)
                SolverSettings.dt /= SolverSettings.NR_timestep_reduction_factor;
            PopulateOutputs();
            Update(variable);
        }

    }
    return true;
}

bool System::SetProp(const string &s, const double &val)
{

}

void System::InitiateOutputs()
{
    Outputs.AllOutPuts.clear();
    for (unsigned int i=0; i<blocks.size(); i++)
    {
        for (map<string, Quan>::iterator it = blocks[i].GetVars()->begin(); it != blocks[i].GetVars()->end(); it++)
            if (it->second.IncludeInOutput())
                Outputs.AllOutPuts.append(CBTC(), blocks[i].GetName() + "_" + it->first);
    }

    for (unsigned int i=0; i<links.size(); i++)
    {
        for (map<string, Quan>::iterator it = links[i].GetVars()->begin(); it != links[i].GetVars()->end(); it++)
            if (it->second.IncludeInOutput())
                Outputs.AllOutPuts.append(CBTC(), links[i].GetName() + "_" + it->first);
    }

}


void System::PopulateOutputs()
{
    for (unsigned int i=0; i<blocks.size(); i++)
    {
        for (map<string, Quan>::iterator it = blocks[i].GetVars()->begin(); it != blocks[i].GetVars()->end(); it++)
            if (it->second.IncludeInOutput())
                Outputs.AllOutPuts[blocks[i].GetName() + "_" + it->first].append(SolverSettings.t,blocks[i].GetVal(it->first,Expression::timing::present));
    }

    for (unsigned int i=0; i<links.size(); i++)
    {
        for (map<string, Quan>::iterator it = links[i].GetVars()->begin(); it != links[i].GetVars()->end(); it++)
            if (it->second.IncludeInOutput())
                Outputs.AllOutPuts[links[i].GetName() + "_" + it->first].append(SolverSettings.t,links[i].GetVal(it->first,Expression::timing::present));
    }

}


bool System::OneStepSolve(const string &variable)
{
	Renew(variable);
	#ifdef Debug_mode
//  cout << "Calculating Residuals" <<endl;
    #endif // Debug_mode
    CVector_arma X = CalcStateVariables(variable, Expression::timing::past);
//  cout<<"X: " << X.toString()<<endl;
    CVector_arma F = GetResiduals(variable, X);
//  cout<<"F: " << F.toString()<<endl;
    double err_ini = F.norm2();
    double err;
    double err_p = err = err_ini;
    SolverTempVars.numiterations = 0;
    while (err/err_ini>SolverSettings.NRtolerance)
    {
        SolverTempVars.numiterations++;
        if (SolverTempVars.updatejacobian)
        {
            SolverTempVars.Inverse_Jacobian = Invert(Jacobian(variable,X));
            SolverTempVars.updatejacobian = false;
            SolverTempVars.NR_coefficient = 1;
        }
        X = X - SolverTempVars.NR_coefficient*SolverTempVars.Inverse_Jacobian*F;
        F = GetResiduals(variable, X);
        err_p = err;
        err = F.norm2();
        #ifdef Debug_mode
        cout << err << endl;
        #endif // Debug_mode
        if (err>err_p)
            SolverTempVars.NR_coefficient*=SolverSettings.NR_coeff_reduction_factor;
        //else
        //    SolverTempVars.NR_coefficient/=SolverSettings.NR_coeff_reduction_factor;
        if (SolverTempVars.numiterations>SolverSettings.NR_niteration_max)
            return false;
    }
	#ifdef Debug_mode
//	CMatrix_arma M = Jacobian("Storage",X);
//	M.writetofile("M.txt");
	#endif // Debug_mode
	return true;
}

bool System::Renew(const string & variable)
{
	bool out = true;
	for (unsigned int i = 0; i < blocks.size(); i++)
		out &= blocks[i].Renew(variable);

	for (unsigned int i = 0; i < links.size(); i++)
		out &= links[i].Renew(variable);

	return out;
}

bool System::Update(const string & variable)
{
	bool out = true;
	for (unsigned int i = 0; i < blocks.size(); i++)
		out &= blocks[i].Update(variable);

	for (unsigned int i = 0; i < links.size(); i++)
		out &= links[i].Update(variable);

	return out;
}


CVector_arma System::CalcStateVariables(const string &variable, const Expression::timing &tmg)
{
    CVector_arma X(blocks.size());
    for (unsigned int i=0; i<blocks.size(); i++)
        X[i] = blocks[i].CalcVal(variable,tmg);
    return X;
}

void System::SetStateVariables(const string &variable, CVector_arma &X, const Expression::timing &tmg)
{
    for (unsigned int i=0; i<blocks.size(); i++)
    {
        blocks[i].SetVal(variable,X[i],tmg);
        #ifdef Debug_mode
//      cout<<"Variable :"<< variable << "in " + blocks[i].GetName() << " was set to " + numbertostring(blocks[i].GetVal(variable,tmg)) << endl;
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
        F[links[i].s_Block_No()] += links[i].GetVal(blocks[links[i].s_Block_No()].Variable(variable)->GetCorrespondingFlowVar(),Expression::timing::present);
        F[links[i].e_Block_No()] -= links[i].GetVal(blocks[links[i].s_Block_No()].Variable(variable)->GetCorrespondingFlowVar(),Expression::timing::present);
    }
    return F;
}

bool System::CalculateFlows(const string &var, const Expression::timing &tmg)
{
    for (int i=0; i<links.size(); i++)
    {
        links[i].SetVal(var,links[i].CalcVal(var,tmg));
    }
	return true;
}

CMatrix_arma System::Jacobian(const string &variable, CVector_arma &X)
{
    CMatrix_arma M(X.num);

    CVector_arma F0 = GetResiduals(variable, X);
    for (int i=0; i < X.num; i++)
    {
        CVector_arma V = Jacobian(variable, X, F0, i);
        for (int j=0; j<X.num; j++)
            M(i,j) = V[j];
    }

  return Transpose(M);
}


CVector_arma System::Jacobian(const string &variable, CVector_arma &V, CVector_arma &F0, int i)  //Works also w/o reference (&)
{
  double epsilon;
  epsilon = -1e-6;
  CVector_arma V1(V);
  V1[i] += epsilon;
  #ifdef Debug_mode
//cout<<i<<":"<<V1.toString()<<endl;
  #endif // Debug_mode
  CVector_arma F1;
  F1 = GetResiduals(variable,V1);
  CVector_arma grad = (F1 - F0) / epsilon;
  if (grad.norm2() == 0)
  {
    epsilon = 1e-6;
    V1 = V;
    V1[i] += epsilon;
    F1 = GetResiduals(variable,V1);
    grad = (F1 - F0) / epsilon;
  }
  return grad;

}


