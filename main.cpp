#include <iostream>
#include "Expression.h"
#include "string"
#include "System.h"
#include <iostream>
#include "Condition.h"
#include "GA.h"
#include "Objective_Function_Set.h"
#include "Script.h"

using namespace std;

int main()
{
    Script scr("script.txt");


    CGA<System> TempObj;
    System *S = scr.CreateSystem();
    S->errorhandler.Write("errors.txt");



    cout<<S->link("myLink")->Variable("flow")->GetRule()->ToString()<<endl;
    cout<<S->link("myLink")->Variable("flow")->ToString()<<endl;
    cout<<S->link("myLink")->GetVars()->ToString()<<endl;
    cout<<S->GetMetaModel()->ToString()<<endl;
	cout<<S->link("myLink")->CalcVal("flow",Expression::timing::present)<<endl;
	cout<<S->Variable("Storage")->GetCorrespondingFlowVar()<<endl;
	Block User1;
	User1.SetName("User1");
	User1.SetType("User");
	S->AddBlock(User1);

	Link User_link;
	User_link.SetName("User_link");
	User_link.SetType("User_flow");
	S->AddLink(User_link, "Reservoir2", "User1");

	if (S->link("User_link")->Variable("flow")->SetTimeSeries("Demand.txt"))
		cout << "Flow file was set successfully!" << endl;
	else
		cout << "Demand.txt was not found!"<<endl;

	vector<string> blocks = S->GetAllBlockTypes();
	vector<string> links = S->GetAllLinkTypes();

    S->AppendObjectiveFunction("Flow_myLink","myLink",Expression("flow"));
    S->dt() = 0.01;
	S->tend() = 10;
	S->SetVariableParents();
	S->SetSilent(true);
	S->errorhandler.Write("errors.txt");
	S->Solve("Storage");
    S->GetObjectiveFunctionTimeSeries("Flow_myLink")->writefile("Obj.txt");
    S->GetOutputs().writetofile("text.txt");
    CGA <System> GA("GA_info.txt",*S);
    GA.optimize();

    return 0;
}
