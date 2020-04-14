#ifndef QUANSET_H
#define QUANSET_H

#ifdef QT_version
    #include <QStringList>
	#include <QJsonObject>
#endif

#include <map>
#include <Quan.h>

class Object;

enum class blocklink {block, link, source, entity};

class QuanSet
{
    public:
        QuanSet();
        QuanSet(Json::ValueIterator &object_type);
        virtual ~QuanSet();
        QuanSet(const QuanSet& other);
        QuanSet& operator=(const QuanSet& other);
        bool Append(const string &s, const Quan &q);
        void Append(QuanSet &qset);
        size_t Count(const string &s) const {return quans.count(s);}
        Quan& operator[] (const string &s);
        Quan& GetVar(const string &s);
        Quan* GetVar(int i);
        Quan* GetVarAskable(int i);
        std::map<string,Quan>::iterator find(const string &name) {return quans.find(name);}
        std::map<string,Quan>::iterator end() {return quans.end();}
        std::map<string,Quan>::iterator begin() {return quans.begin();}
        unsigned long size() {return quans.size();}
        unsigned long AskableSize();
        string &Description() {return description;}
        string &IconFileName() {return iconfilename;}
        string &Name() {return name;}
        void ShowMessage(const string &msg);
        string ToString(int tabs=0);
        blocklink BlockLink;
        void SetParent(Object *p) {parent = p; SetAllParents();}
        void SetAllParents();
        Object *Parent() {return parent; }
        vector<CTimeSeries*> TimeSeries();
        vector<string> QuanNames();
        string toCommand();
#ifdef QT_version
        QStringList QQuanNames();
		QuanSet(QJsonObject& object_types);
#endif
        bool AppendError(const string &objectname, const string &cls, const string &funct, const string &description, const int &code);
		string ObjectType; 
		string& CategoryType() { return typecategory; }
    protected:

    private:
        Object* parent = nullptr;
        string name = "";
        map<string, Quan> quans;
        string last_error = "";
        string description = "";
        string iconfilename = "";
        string typecategory = "";


};

#endif // QUANSET_H
