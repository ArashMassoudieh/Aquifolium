#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <string>
#include <fstream>
#include <vector>

using namespace std;

struct _error
{
    string description;
    string cls;
    string funct;
    string objectname;
    int code;
};

class ErrorHandler
{
    public:
        ErrorHandler();
        virtual ~ErrorHandler();
        ErrorHandler(const ErrorHandler& other);
        ErrorHandler& operator=(const ErrorHandler& other);
        void Write(const string &filename);
        bool Append(const _error &err) {errors.push_back(err); return false;}
        bool Append(const string &objectname, const string &cls, const string &funct, const string &description, const int &code)
        {
            _error err;
            err.description = description;
            err.cls = cls;
            err.funct = funct;
            err.objectname = objectname;
            err.code = code;
            errors.push_back(err);
            return false;
        }
    protected:

    private:
        vector<_error> errors;

};

#endif // ERRORHANDLER_H
