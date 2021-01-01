#include "sema/SymbolTable.hpp"

using namespace std;

Attribute::Attribute(enumTypeSet n_typeset, enumType n_type)
            : typeset(n_typeset), type(n_type) {}

//Attribute::~Attribute

SymbolEntry::SymbolEntry(const char *n_name, enumKind n_kind, enumTypeSet n_typeset, enumType n_type, Attribute* n_attri)
            : sym_name(n_name), sym_kind(n_kind), sym_typeset(n_typeset), sym_type(n_type), sym_attri(n_attri) { 
                sym_which = 0; 
                this->isused = 1;
}

SymbolEntry::SymbolEntry(const char *n_name, enumKind n_kind, enumTypeSet n_typeset, string n_strtype, Attribute* n_attri)
            : sym_name(n_name), sym_kind(n_kind), sym_typeset(n_typeset), sym_strtype(n_strtype), sym_attri(n_attri) { 
                sym_which = 1; 
                this->isused = 1;
}

int SymbolEntry::get_which(){
    return this->sym_which;
}

string SymbolEntry::get_symkind(){
    switch(this->sym_kind){
        case PROGRAM_KIND: return "program"; break;
        case FUNCTION_KIND: return "function"; break;
        case PARA_KIND: return "parameter"; break;
        case VAR_KIND: return "variable"; break;
        case LOOP_VAR_KIND: return "loop_var"; break;
        case CONST_KIND: return "constant"; break;
    }
}

string SymbolEntry::get_symtype_str(){
    return this->sym_strtype;
}

int SymbolEntry::get_arrsize(){
    return this->get_arr().size();
}

string SymbolEntry::get_symtype(){
    if(this->sym_which==0){
        string tem;
        switch(this->sym_typeset){
            case SET_SCALAR:
                switch(this->sym_type){
                    case INT_TYPE: tem = "integer"; break;
                    case REAL_TYPE: tem = "real"; break;
                    case BOOL_TYPE: tem = "boolean"; break;
                    case STR_TYPE: tem = "string"; break;
                    case VOID_TYPE: tem = "void"; break;
                }
                break;
            case SET_CONST:
                switch(this->sym_type){
                    case INT_TYPE: tem = "integer"; break;
                    case REAL_TYPE: tem = "real"; break;
                    case BOOL_TYPE: tem = "boolean"; break;
                    case STR_TYPE: tem = "string"; break;
                    case VOID_TYPE: tem = "void"; break;
                }
                break;
            case SET_ARR:
                switch(this->sym_type){
                    case INT_TYPE: tem = "integer"; break;
                    case REAL_TYPE: tem = "real"; break;
                    case BOOL_TYPE: tem = "boolean"; break;
                    case STR_TYPE: tem = "string"; break;
                    case VOID_TYPE: tem = "void"; break;
                }
                for(int i=0;i<this->sym_attri->arr_val.size();i++){
                    tem += "[";
                    tem += to_string(this->sym_attri->arr_val[i]);
                    tem += "]";
                }
            case UNKNOWN_SET:
                tem = "void";
        }
        return tem;
    }
    else {
        return this->sym_strtype;
    }
}

string SymbolEntry::get_shorttype(){
    vector <string> tem_vec;
    if(this->get_which()==0)
        tem_vec = split(this->get_symtype()," ");
    else 
        tem_vec = split(this->get_symtype_str()," ");
    //cout<<"check type: "<<this->get_symtype()<<endl;

    return tem_vec[0];
    
}

vector <string> SymbolEntry::get_arr(){
    //string short = this->get_shorttype();
    vector <string> tem_vec;
    if(this->get_which()==0)
        tem_vec = split(this->get_symtype()," ");
    else 
        tem_vec = split(this->get_symtype_str()," ");
    //cout<<"check type: "<<this->get_symtype()<<endl;
    string tem;
    if(tem_vec.size()>1){
        tem = tem_vec[1];
    }
    else {
        vector <string> not_arr;
        return not_arr;
    }
    tem_vec = split(tem,"[]");
    return tem_vec;
}

vector <string> split(string s, string splitsep){
    vector <string> result;
    int current = 0;
    int next = 0;
    while(next!=-1){
        next = s.find_first_of(splitsep, current);
        if(next!=current){
            string tmp = s.substr(current, next-current);
            if(!tmp.empty()){
                result.push_back(tmp);
            }
        }
        current = next+1;
    }
    return result;
}
    
SymbolTable::SymbolTable() {}

SymbolTable::SymbolTable(int n_atlevel): atlevel(n_atlevel) {
    this->prev = NULL;
}

void SymbolTable::addSymbol(SymbolEntry *n_entry){
    entry[n_entry->sym_name] = n_entry;
    entry_str.push_back(n_entry->sym_name);
}

void SymbolTable::addloop(string loop_name){
    loop_var_str.push_back(loop_name);
}

bool SymbolTable::isexist(SymbolEntry *n_entry){
    /**/
    if(entry[n_entry->sym_name]!=nullptr){
        if(entry[n_entry->sym_name]->isused == true) return true;
    }
    return false;
}

void print_format(const char chr){
    for(size_t i=0;i<110;i++){
        printf("%c",chr);
    }
    puts("");
}

int SymbolTable::entry_num(){
    return entry_str.size();
}

void SymbolTable::print(){
    //cout<<"num: "<<entry_str.size()<<endl;
    print_format('=');
    printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type", "Attribute");
    print_format('-');
    //cout<<"here 1"<<endl;
    if(this->entry_str.size()!=0){
        //cout<<"here 2"<<endl;
        for(int i=0;i<this->entry_str.size();i++){   
            cout<<left<<setw(33);
            //cout<<"here 3"<<endl;
            cout<<entry[this->entry_str[i]]->sym_name;
            //cout<<"here 4"<<endl;
            
            
            cout<<left<<setw(11);
            cout<<entry[this->entry_str[i]]->get_symkind();
            //cout<<"here 5"<<endl;

            if(this->atlevel==0){
                string tem = to_string(this->atlevel);
                tem += "(global)";
                cout<<left<<setw(11);
                cout<<tem;            
            }
            else {
                string tem = to_string(this->atlevel);
                tem += "(local)";
                cout<<left<<setw(11);
                cout<<tem;    
            }
            //cout<<"here 6"<<endl;

            cout<<left<<setw(17);
            //cout<<"boolean";
            if(entry[this->entry_str[i]]->get_which()==0)
                cout<<entry[this->entry_str[i]]->get_symtype();
            else
                cout<<entry[this->entry_str[i]]->get_symtype_str();
            //cout<<"here 7"<<endl;
            //cout<<"integer, real [2][3]";
            switch(entry[this->entry_str[i]]->sym_kind){
                case FUNCTION_KIND:
                    //cout<<"function attribute"<<endl;
                    cout<<entry[this->entry_str[i]]->sym_attri->func_arg;
                    break;
                case CONST_KIND:
                    //cout<<"const attribute"<<endl;
                    switch(entry[this->entry_str[i]]->sym_typeset){
                        case SET_CONST: 
                            switch(entry[this->entry_str[i]]->sym_type){
                                case INT_TYPE: 
                                    //cout<<"int value"<<endl;
                                    cout<<entry[this->entry_str[i]]->sym_attri->int_val;
                                    break;
                                case REAL_TYPE: 
                                    //cout<<"real value"<<endl;
                                    cout<<to_string(entry[this->entry_str[i]]->sym_attri->dou_val);
                                    break;
                                case BOOL_TYPE: 
                                    //cout<<"bool value"<<endl;
                                    cout<<entry[this->entry_str[i]]->sym_attri->bool_val? "true" : "false";
                                    break;
                                case STR_TYPE:
                                    //cout<<"str value"<<endl; 
                                    cout<<entry[this->entry_str[i]]->sym_attri->str_val;
                                    break;
                            }
                            break;
                        //case SET_ARR:
                    }
                    break;
            }
            //cout<<"here 8"<<endl;
            puts("");
        }
    }
    
    print_format('-');
}

