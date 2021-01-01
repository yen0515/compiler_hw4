#include "AST/ast.hpp"
#include <vector>
#include <stack>
#include <string>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <map>

using namespace std;

enum enumKind{
    PROGRAM_KIND,
    FUNCTION_KIND,
    PARA_KIND,
    VAR_KIND,
    LOOP_VAR_KIND,
    CONST_KIND
};

enum enumType{
    INT_TYPE,
    REAL_TYPE,
    BOOL_TYPE,
    STR_TYPE,
    VOID_TYPE,
    UNKNOWN_TYPE
};

enum enumTypeSet{
    SET_SCALAR,
    SET_ARR,
    SET_CONST,
    UNKNOWN_SET
};

class Attribute{
    public:
        Attribute(enumTypeSet n_typeset, enumType n_type);
        //~Attribute();
        //string get_type();

    public:
        int32_t int_val;
        double dou_val;
        string str_val;
        string bool_val;
        vector <int> arr_val;
        string func_arg;
        enumType type;
        enumTypeSet typeset;
};
vector <string> split(string s, string splitsep);

class SymbolEntry{
    public:
        SymbolEntry() = default;
        SymbolEntry(const char *n_name, enumKind n_kind, enumTypeSet n_typeset, enumType n_type, Attribute* n_attri);
        SymbolEntry(const char *n_name, enumKind n_kind, enumTypeSet n_typeset, string n_strtype, Attribute* n_attri);
        //~SymbolEntry();
        string get_symtype();
        string get_shorttype();
        string get_symkind();
        string get_symtype_str();
        vector <string> get_arr();
        int get_arrsize();
        int get_which();

    public:
        bool isused = 0;
        const char *sym_name;
        enumKind sym_kind;
        enumType sym_type;
        enumTypeSet sym_typeset;
        Attribute* sym_attri;
        string sym_strtype;
        int sym_which;
};

void print_format(const char chr);

class SymbolTable{
    public:
        SymbolTable();
        SymbolTable(int n_atlevel);
        void addSymbol(SymbolEntry *n_entry);
        void addloop(string loop_name);
        void print();
        int entry_num(); 
        bool isexist(SymbolEntry *n_entry);
        
    public:
        map<string, SymbolEntry*> entry;
        vector <string> entry_str;
        vector <string> loop_var_str;
        SymbolTable *prev;
        int atlevel;
};
