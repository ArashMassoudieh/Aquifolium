#include <iostream>
#include "Expression.h"
#include "string"

using namespace std;

int main()
{
    string s = "Hi, My name is Arash Massoudieh, I go by name Arash";
    cout<<count(s,"Arash")<<endl;
    cout<<contains(s,"Arash")<<endl;
    bool x = replace(s,"Arash", "Ali");
    cout << s << endl;
    remove(s,"Ali");
    cout << s << endl;
    return 0;
}
