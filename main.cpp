#include <iostream>
#include "Expression.h"
#include "string"
#include "System.h"
#include <iostream>


using namespace std;

int main()
{
    System S;
    S.GetQuanTemplate("power_reservoirs.qnt");
    //Expression E("2*X+X^2+Y/X");
    Block B1;
    B1.SetName("myblock1");
    S.AddBlock(B1);

    S.block("myblock1")->SetVal("Storage",10);
    S.block("myblock1")->SetVal("reservoir_power",2);
    S.block("myblock1")->SetVal("reservoir_coeff",30);

    Block B2;
    B2.SetName("myblock2");
    S.AddBlock(B2);

    S.block("myblock2")->SetVal("Storage",5);
    S.block("myblock2")->SetVal("reservoir_power",2);
    S.block("myblock2")->SetVal("reservoir_coeff",30);
//  cout<<S.block("myblock2")->GetVal("flow")<<endl;

    Link L;
    L.SetName("myLink");
    S.AddLink(L,"myblock1","myblock2");
    cout<<S.link("myLink")->GetVal("flow")<<endl;

    //Quan X, Y;
    //X.SetType(Quan::_type::value);
    //X.SetVal(2);

    //Y.SetType(Quan::_type::value);
    //Y.SetVal(3);
    //B.AddQnantity("Y",Y);
    //cout<<B.GetVal("X")<<endl;
    //cout<<B.GetVal("Y")<<endl;
    //cout<<E.calc(&B)<<endl;
    return 0;
}
