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
    S->GetOutputs().writetofile("output.txt");
	//CGA <System> GA("GA_info.txt",*S);
    //GA.optimize();

    return 0;
}
