#include <iostream>
#include "Expression.h"
#include "string"
#include "System.h"
#include <iostream>
#include "Condition.h"
#include "GA.h"
#include "Objective_Function_Set.h"

using namespace std;

int main()
{

    CGA<System> TempObj;
    System S;
    S.GetQuanTemplate("power_reservoirs_rules.qnt");
    //Expression E("2*X+X^2+Y/X");
    Block B1;
    B1.SetType("Reservoir");
    B1.SetName("Reservoir1");
    S.AddBlock(B1);

    S.block("Reservoir1")->SetVal("Storage",15);
    S.block("Reservoir1")->SetVal("reservoir_power",2);
    S.block("Reservoir1")->SetVal("reservoir_coeff",30);
    Condition cond("reservoir_power<reservoir_coeff");
    bool out = cond.calc(S.block("Reservoir1"),Expression::timing::present);
    cout<<out<<endl;

    if (S.block("Reservoir1")->Variable("inflow")->SetTimeSeries("Inflow.txt"))
   		cout << "Inflow file was set successfully!" << endl;
	else
		cout << "Inflow.txt was not found!"<<endl;

    Block B2;
    B2.SetType("Reservoir");
    B2.SetName("Reservoir2");
    S.AddBlock(B2);

    S.block("Reservoir2")->SetVal("Storage",5);
    S.block("Reservoir2")->SetVal("reservoir_power",2);
    S.block("Reservoir2")->SetVal("reservoir_coeff",30);
//  cout<<S.block("myblock2")->GetVal("flow")<<endl;

    Link L;
    L.SetType("Reservoir_link_rule");
    L.SetName("myLink");
    S.AddLink(L,"Reservoir1","Reservoir2");
    //cout<<"flow = " << S.link("myLink")->CalcVal("flow")<<endl;
    S.link("myLink")->SetVal("Q_max",50);
    S.link("myLink")->SetVal("Q_min",10);
    S.link("myLink")->SetVal("S_min",10);
    S.link("myLink")->SetVal("S_max",100);

    S.AppendParameter("Q_max_param",20,80);
    S.AppendParameter("Q_min_param",5,15);
    S.AppendParameter("S_min_param",5,15);
    S.AppendParameter("S_max_param",20,80);
    S.SetAsParameter("myLink","Q_max","Q_max_param");
    S.SetAsParameter("myLink","Q_min","Q_min_param");
    S.SetAsParameter("myLink","S_min","S_min_param");
    S.SetAsParameter("myLink","S_max","S_max_param");
    cout<<S.link("myLink")->Variable("flow")->GetRule()->ToString()<<endl;
    cout<<S.link("myLink")->Variable("flow")->ToString()<<endl;
    cout<<S.link("myLink")->GetVars()->ToString()<<endl;
    cout<<S.GetMetaModel()->ToString()<<endl;
	cout<<S.link("myLink")->CalcVal("flow",Expression::timing::present)<<endl;
	cout<<S.Variable("Storage")->GetCorrespondingFlowVar()<<endl;
	Block User1;
	User1.SetName("User1");
	User1.SetType("User");
	S.AddBlock(User1);

	Link User_link;
	User_link.SetName("User_link");
	User_link.SetType("User_flow");
	S.AddLink(User_link, "Reservoir2", "User1");

	if (S.link("User_link")->Variable("flow")->SetTimeSeries("Demand.txt"))
		cout << "Flow file was set successfully!" << endl;
	else
		cout << "Demand.txt was not found!"<<endl;

	vector<string> blocks = S.GetAllBlockTypes();
	vector<string> links = S.GetAllLinkTypes();

    S.AppendObjectiveFunction("Flow_myLink","myLink",Expression("flow"));
    S.dt() = 0.01;
	S.tend() = 10;
	S.SetVariableParents();
	S.SetSilent(true);
	S.Solve("Storage");
    S.GetObjectiveFunctionTimeSeries("Flow_myLink")->writefile("Obj.txt");
    S.GetOutputs().writetofile("text.txt");
    CGA <System> GA("GA_info.txt",S);
    GA.optimize();

    return 0;
}
