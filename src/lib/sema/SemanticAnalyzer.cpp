#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"

using namespace std;

SemanticAnalyzer::SemanticAnalyzer(string n_filename, FILE* n_fp)
                : filename(n_filename), fp(n_fp) {}

void SemanticAnalyzer::can_print(){
    can_print_table = 1;
}

void SemanticAnalyzer::pushScope(SymbolTable *n_scope){
    n_scope->prev = cur;
    this->tables.push(n_scope);
    cur = n_scope;
    
}

void SemanticAnalyzer::popScope(){
    cur = cur->prev;
    this->tables.pop();
}

void SemanticAnalyzer::print_table(){
    if(can_print_table)
        (*tables.top()).print();
}

int SemanticAnalyzer::func_index(string n_name){
    int index;
    bool isdefine_func = 0;
    vector <string> tem_vec;
    string tem;
    while(!func_var.empty()){
        tem = func_var.top();
        tem_vec.push_back(tem);
        func_var.pop();
    }
    for(int i=tem_vec.size()-1;i>=0;i--){
        if(tem_vec[i]==n_name) {
            isdefine_func = 1;
            index = tem_vec.size()-1-i;
        }
        func_var.push(tem_vec[i]);
    }
    if(isdefine_func) return index;
    return -1;
}

int SemanticAnalyzer::get_arrsize(string n_name){
    bool isdefine = 0;
    SymbolTable *current = cur;
    while(1){
        if(current->entry[n_name]==NULL) break;
        if(current->entry[n_name]->isused){
            isdefine = 1;
            break;
        }
        else{
            if(current->atlevel==0) {
                break;
            }
            else current = current->prev;
        }
    }
    if(isdefine){
        return current->entry[n_name]->get_arrsize();
    }
    else {
        return -1;
    }
}

vector <string> SemanticAnalyzer::get_arr(string n_name){
    bool isdefine = 0;
    SymbolTable *current = cur;
    while(1){
        if(current->entry[n_name]==NULL) break;
        if(current->entry[n_name]->isused){
            isdefine = 1;
            break;
        }
        else{
            if(current->atlevel==0) {
                break;
            }
            else current = current->prev;
        }
    }
    if(isdefine){
        return current->entry[n_name]->get_arr();
    }
    else {
        vector <string> err;
        err.push_back("error");
        return err;
    }
}

string SemanticAnalyzer::get_shorttype(string n_name){
    bool isdefine = 0;
    SymbolTable *current = cur;
    while(1){
        if(current->entry[n_name]==NULL) break;
        if(current->entry[n_name]->isused){
            isdefine = 1;
            break;
        }
        else{
            if(current->atlevel==0) {
                break;
            }
            else current = current->prev;
        }
    }
    if(isdefine){
        return current->entry[n_name]->get_shorttype();
    }
    else {
        return "error";
    }
}

bool SemanticAnalyzer::check_exist(string n_name){
    bool isdefine = 0;
    SymbolTable *current = cur;
    while(1){
        if(current->entry[n_name]==NULL) break;
        if(current->entry[n_name]->isused){
            isdefine = 1;
            break;
        }
        else{
            if(current->atlevel==0) {
                break;
            }
            else current = current->prev;
        }
    }
    return isdefine;
}

bool SemanticAnalyzer::modify_loopvar(string n_name){
    bool isdefine_loop = 0;
    vector <string> tem_vec;
    string tem;
    while(!loop_var.empty()){
        tem = loop_var.top();
        tem_vec.push_back(tem);
        loop_var.pop();
    }
    for(int i=tem_vec.size()-1;i>=0;i--){
        if(tem_vec[i]==n_name) isdefine_loop = 1;
        loop_var.push(tem_vec[i]);
    }
    return isdefine_loop;
}

bool SemanticAnalyzer::modify_funcvar(string n_name){
    bool isdefine_func = 0;
    vector <string> tem_vec;
    string tem;
    while(!func_var.empty()){
        tem = func_var.top();
        tem_vec.push_back(tem);
        func_var.pop();
    }
    for(int i=tem_vec.size()-1;i>=0;i--){
        if(tem_vec[i]==n_name) isdefine_func = 1;
        func_var.push(tem_vec[i]);
    }
    return isdefine_func;
}

bool SemanticAnalyzer::modify_const(string n_name){
    bool isdefine_const = 0;
    vector <string> tem_vec;
    string tem;
    while(!const_var.empty()){
        tem = const_var.top();
        tem_vec.push_back(tem);
        const_var.pop();
    }
    for(int i=tem_vec.size()-1;i>=0;i--){
        if(tem_vec[i]==n_name) isdefine_const = 1;
        const_var.push(tem_vec[i]);
    }
    return isdefine_const;
}

void SemanticAnalyzer::error1(string loc_line, string loc_col){
    error_msg += "<Error> Found in line ";
    error_msg += loc_line;
    error_msg += ", column ";
    error_msg += loc_col;
    error_msg += ": ";
}

void SemanticAnalyzer::error2(int loc_line, int loc_col){
    char tem[1024];
    fseek(this->fp, 0, SEEK_SET);
    for(int i=0;i<loc_line;i++){
        fgets(tem, 1024, fp);
    }
    error_msg += "    ";
    error_msg += string(tem);
    error_msg += "    ";
    for(int i=0;i<loc_col-1;i++) error_msg += " ";
    error_msg += "^\n";
}

void SemanticAnalyzer::visit(ProgramNode &p_program) {
    
    SymbolTable *prog_table = new SymbolTable(level);
    this->pushScope(prog_table);
    Attribute *prog_attri = new Attribute(UNKNOWN_SET, VOID_TYPE);
    SymbolEntry* prog_entry = new SymbolEntry(p_program.getNameCString(), PROGRAM_KIND, UNKNOWN_SET, VOID_TYPE, prog_attri);
    (*tables.top()).addSymbol(prog_entry);

    p_program.visitChildNodes(*this);
    while(!func_var.empty())
        func_var.pop();
    this->print_table();
    this->popScope();
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
    if(p_variable.get_enumType()=="integer"){ //int
        if(p_variable.isconstant()){
            Attribute *tem_attri = new Attribute(SET_CONST, INT_TYPE);
            tem_attri->int_val = atoi(p_variable.get_constValue().c_str());
            SymbolEntry *tem_entry = new SymbolEntry(p_variable.getNameCString(), CONST_KIND, SET_CONST, INT_TYPE, tem_attri);
            if((*tables.top()).isexist(tem_entry)) {
                iserror = 1;
                double_error[tem_entry->sym_name] = 1;
                error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                error_msg += "symbol '";
                error_msg += tem_entry->sym_name;
                error_msg += "' is redeclared\n";
                error2(p_variable.location.line, p_variable.location.col);
            }
            else {
                if(modify_loopvar(tem_entry->sym_name)){ //loop var
                    iserror = 1;
                    double_error[tem_entry->sym_name] = 1;
                    error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                    error_msg += "symbol '";
                    error_msg += tem_entry->sym_name;
                    error_msg += "' is redeclared\n";
                    error2(p_variable.location.line, p_variable.location.col);
                }
                else {
                    var_ref_num[tem_entry->sym_name] = 0;
                    const_var.push(tem_entry->sym_name);
                    double_error[tem_entry->sym_name] = false;
                    (*tables.top()).addSymbol(tem_entry);
                }
            }
        }
        else {
            if(ispara){
                Attribute *tem_attri = new Attribute(SET_SCALAR, INT_TYPE);
                SymbolEntry *tem_entry = new SymbolEntry(p_variable.getNameCString(), PARA_KIND, SET_SCALAR, INT_TYPE, tem_attri);
                if((*tables.top()).isexist(tem_entry)) {
                    iserror = 1;
                    double_error[tem_entry->sym_name] = 1;
                    error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                    error_msg += "symbol '";
                    error_msg += tem_entry->sym_name;
                    error_msg += "' is redeclared\n";
                    error2(p_variable.location.line, p_variable.location.col);
                }
                else {
                    if(modify_loopvar(tem_entry->sym_name)){ //loop var
                        iserror = 1;
                        double_error[tem_entry->sym_name] = 1;
                        error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                        error_msg += "symbol '";
                        error_msg += tem_entry->sym_name;
                        error_msg += "' is redeclared\n";
                        error2(p_variable.location.line, p_variable.location.col);
                    }
                    else {
                        tem_info.push_back("integer");
                        tem_name.push_back(tem_entry->sym_name);
                        var_ref_num[tem_entry->sym_name] = 0;
                        double_error[tem_entry->sym_name] = false;
                        (*tables.top()).addSymbol(tem_entry);
                        
                    }
                }
            }
            else if(isloop && !ispara){
                Attribute *tem_attri = new Attribute(SET_SCALAR, INT_TYPE);
                SymbolEntry *tem_entry = new SymbolEntry(p_variable.getNameCString(), LOOP_VAR_KIND, SET_SCALAR, INT_TYPE, tem_attri);
                if((*tables.top()).isexist(tem_entry)) {
                    iserror = 1;
                    double_error[tem_entry->sym_name] = 1;
                    error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                    error_msg += "symbol '";
                    error_msg += tem_entry->sym_name;
                    error_msg += "' is redeclared\n";
                    error2(p_variable.location.line, p_variable.location.col);
                }
                else {
                    if(modify_loopvar(tem_entry->sym_name)){ //loop var
                        iserror = 1;
                        double_error[tem_entry->sym_name] = 1;
                        error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                        error_msg += "symbol '";
                        error_msg += tem_entry->sym_name;
                        error_msg += "' is redeclared\n";
                        error2(p_variable.location.line, p_variable.location.col);
                    }
                    else {
                        //cout<<"is decline as loop var: "<<tem_entry->sym_name<<endl;
                        loop_var.push(tem_entry->sym_name);
                        double_error[tem_entry->sym_name] = false;
                        (*tables.top()).addSymbol(tem_entry);
                        PType* push_type = new PType(PType::PrimitiveTypeEnum::kIntegerType);
                        expr_stack.push(*push_type);
                    }
                }
            }
            else {
                Attribute *tem_attri = new Attribute(SET_SCALAR, INT_TYPE);
                SymbolEntry *tem_entry = new SymbolEntry(p_variable.getNameCString(), VAR_KIND, SET_SCALAR, INT_TYPE, tem_attri);
                if((*tables.top()).isexist(tem_entry)) {
                    iserror = 1;
                    double_error[tem_entry->sym_name] = 1;
                    error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                    error_msg += "symbol '";
                    error_msg += tem_entry->sym_name;
                    error_msg += "' is redeclared\n";
                    error2(p_variable.location.line, p_variable.location.col);
                }
                else {
                    if(modify_loopvar(tem_entry->sym_name)){ //loop var
                        iserror = 1;
                        double_error[tem_entry->sym_name] = 1;
                        error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                        error_msg += "symbol '";
                        error_msg += tem_entry->sym_name;
                        error_msg += "' is redeclared\n";
                        error2(p_variable.location.line, p_variable.location.col);
                        }
                    else {
                        
                        var_ref_num[tem_entry->sym_name] = 0;
                        double_error[tem_entry->sym_name] = false;
                        (*tables.top()).addSymbol(tem_entry);
                        PType* push_type = new PType(PType::PrimitiveTypeEnum::kIntegerType);
                        expr_stack.push(*push_type);
                    }
                }
            }
        }
    }
    else if(p_variable.get_enumType()=="real"){ //real
        if(p_variable.isconstant()){
            Attribute *tem_attri = new Attribute(SET_CONST, REAL_TYPE);
            tem_attri->dou_val = atof(p_variable.get_constValue().c_str());
            SymbolEntry *tem_entry = new SymbolEntry(p_variable.getNameCString(), CONST_KIND, SET_CONST, REAL_TYPE, tem_attri);
            if((*tables.top()).isexist(tem_entry)) {
                iserror = 1;
                double_error[tem_entry->sym_name] = 1;
                error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                error_msg += "symbol '";
                error_msg += tem_entry->sym_name;
                error_msg += "' is redeclared\n";
                error2(p_variable.location.line, p_variable.location.col);
            }
            else {
                if(modify_loopvar(tem_entry->sym_name)){ //loop var
                    iserror = 1;
                    double_error[tem_entry->sym_name] = 1;
                    error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                    error_msg += "symbol '";
                    error_msg += tem_entry->sym_name;
                    error_msg += "' is redeclared\n";
                    error2(p_variable.location.line, p_variable.location.col);
                }
                else {
                    const_var.push(tem_entry->sym_name);
                    var_ref_num[tem_entry->sym_name] = 0;
                    double_error[tem_entry->sym_name] = false;
                    (*tables.top()).addSymbol(tem_entry);
                }
            }
        }
        else {
            if(ispara){
                Attribute *tem_attri = new Attribute(SET_SCALAR, REAL_TYPE);
                SymbolEntry *tem_entry = new SymbolEntry(p_variable.getNameCString(), PARA_KIND, SET_SCALAR, REAL_TYPE, tem_attri);
                if((*tables.top()).isexist(tem_entry)) {
                    iserror = 1;
                    double_error[tem_entry->sym_name] = 1;
                    error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                    error_msg += "symbol '";
                    error_msg += tem_entry->sym_name;
                    error_msg += "' is redeclared\n";
                    error2(p_variable.location.line, p_variable.location.col);
                }
                else {
                    if(modify_loopvar(tem_entry->sym_name)){ //loop var
                        iserror = 1;
                        double_error[tem_entry->sym_name] = 1;
                        error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                        error_msg += "symbol '";
                        error_msg += tem_entry->sym_name;
                        error_msg += "' is redeclared\n";
                        error2(p_variable.location.line, p_variable.location.col);
                    }
                    else {
                        tem_info.push_back("real");
                        tem_name.push_back(tem_entry->sym_name);
                        var_ref_num[tem_entry->sym_name] = 0;
                        double_error[tem_entry->sym_name] = false;
                        (*tables.top()).addSymbol(tem_entry);
                    }
                }
            }
            else {
                Attribute *tem_attri = new Attribute(SET_SCALAR, REAL_TYPE);
                SymbolEntry *tem_entry = new SymbolEntry(p_variable.getNameCString(), VAR_KIND, SET_SCALAR, REAL_TYPE, tem_attri);
                if((*tables.top()).isexist(tem_entry)) {
                    iserror = 1;
                    double_error[tem_entry->sym_name] = 1;
                    error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                    error_msg += "symbol '";
                    error_msg += tem_entry->sym_name;
                    error_msg += "' is redeclared\n";
                    error2(p_variable.location.line, p_variable.location.col);
                }
                else {
                    if(modify_loopvar(tem_entry->sym_name)){ //loop var
                        iserror = 1;
                        double_error[tem_entry->sym_name] = 1;
                        error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                        error_msg += "symbol '";
                        error_msg += tem_entry->sym_name;
                        error_msg += "' is redeclared\n";
                        error2(p_variable.location.line, p_variable.location.col);
                    }
                    else {
                        var_ref_num[tem_entry->sym_name] = 0;
                        double_error[tem_entry->sym_name] = false;
                        (*tables.top()).addSymbol(tem_entry);
                        PType* push_type = new PType(PType::PrimitiveTypeEnum::kRealType);
                        expr_stack.push(*push_type);
                    }
                }
            }
        }
    }
    else if(p_variable.get_enumType()=="string"){ //string
        if(p_variable.isconstant()){
            Attribute *tem_attri = new Attribute(SET_CONST, STR_TYPE);
            tem_attri->str_val = p_variable.get_constValue();
            SymbolEntry *tem_entry = new SymbolEntry(p_variable.getNameCString(), CONST_KIND, SET_CONST, STR_TYPE, tem_attri);
            if((*tables.top()).isexist(tem_entry)) {
                iserror = 1;
                double_error[tem_entry->sym_name] = 1;
                error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                error_msg += "symbol '";
                error_msg += tem_entry->sym_name;
                error_msg += "' is redeclared\n";
                error2(p_variable.location.line, p_variable.location.col);
            }
            else {
                if(modify_loopvar(tem_entry->sym_name)){ //loop var
                    iserror = 1;
                    double_error[tem_entry->sym_name] = 1;
                    error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                    error_msg += "symbol '";
                    error_msg += tem_entry->sym_name;
                    error_msg += "' is redeclared\n";
                    error2(p_variable.location.line, p_variable.location.col);
                }
                else {
                    const_var.push(tem_entry->sym_name);
                    var_ref_num[tem_entry->sym_name] = 0;
                    double_error[tem_entry->sym_name] = false;
                    (*tables.top()).addSymbol(tem_entry);
                }
            }
        }
        else {
            if(ispara){
                Attribute *tem_attri = new Attribute(SET_SCALAR, STR_TYPE);
                SymbolEntry *tem_entry = new SymbolEntry(p_variable.getNameCString(), PARA_KIND, SET_SCALAR, STR_TYPE, tem_attri);
                if((*tables.top()).isexist(tem_entry)) {
                    iserror = 1;
                    double_error[tem_entry->sym_name] = 1;
                    error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                    error_msg += "symbol '";
                    error_msg += tem_entry->sym_name;
                    error_msg += "' is redeclared\n";
                    error2(p_variable.location.line, p_variable.location.col);
                }
                else {
                    if(modify_loopvar(tem_entry->sym_name)){ //loop var
                        iserror = 1;
                        double_error[tem_entry->sym_name] = 1;
                        error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                        error_msg += "symbol '";
                        error_msg += tem_entry->sym_name;
                        error_msg += "' is redeclared\n";
                        error2(p_variable.location.line, p_variable.location.col);
                    }
                    else {
                        tem_info.push_back("string");
                        tem_name.push_back(tem_entry->sym_name);
                        var_ref_num[tem_entry->sym_name] = 0;
                        double_error[tem_entry->sym_name] = false;
                        (*tables.top()).addSymbol(tem_entry);
                    }
                }
            }
            else {
                Attribute *tem_attri = new Attribute(SET_SCALAR, STR_TYPE);
                SymbolEntry *tem_entry = new SymbolEntry(p_variable.getNameCString(), VAR_KIND, SET_SCALAR, STR_TYPE, tem_attri);
                if((*tables.top()).isexist(tem_entry)) {
                    iserror = 1;
                    double_error[tem_entry->sym_name] = 1;
                    error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                    error_msg += "symbol '";
                    error_msg += tem_entry->sym_name;
                    error_msg += "' is redeclared\n";
                    error2(p_variable.location.line, p_variable.location.col);
                }
                else {
                    if(modify_loopvar(tem_entry->sym_name)){ //loop var
                        iserror = 1;
                        double_error[tem_entry->sym_name] = 1;
                        error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                        error_msg += "symbol '";
                        error_msg += tem_entry->sym_name;
                        error_msg += "' is redeclared\n";
                        error2(p_variable.location.line, p_variable.location.col);
                    }
                    else {
                        //cout<<tem_entry->sym_name<<" "<<p_variable.location.line<<" "<<p_variable.location.col<<endl;
                        var_ref_num[tem_entry->sym_name] = 0;
                        double_error[tem_entry->sym_name] = false;
                        (*tables.top()).addSymbol(tem_entry);
                        PType* push_type = new PType(PType::PrimitiveTypeEnum::kStringType);
                        expr_stack.push(*push_type);
                    }
                }
            }
        }
    }
    else if(p_variable.get_enumType()=="boolean"){ //boolean
        if(p_variable.isconstant()){
            Attribute *tem_attri = new Attribute(SET_CONST, BOOL_TYPE);
            tem_attri->bool_val = (p_variable.get_constValue() == "true") ? "true" : "false";
            SymbolEntry *tem_entry = new SymbolEntry(p_variable.getNameCString(), CONST_KIND, SET_CONST, BOOL_TYPE, tem_attri);
            if((*tables.top()).isexist(tem_entry)) {
                iserror = 1;
                double_error[tem_entry->sym_name] = 1;
                error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                error_msg += "symbol '";
                error_msg += tem_entry->sym_name;
                error_msg += "' is redeclared\n";
                error2(p_variable.location.line, p_variable.location.col);
            }
            else {
                if(modify_loopvar(tem_entry->sym_name)){ //loop var
                    iserror = 1;
                    double_error[tem_entry->sym_name] = 1;
                    error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                    error_msg += "symbol '";
                    error_msg += tem_entry->sym_name;
                    error_msg += "' is redeclared\n";
                    error2(p_variable.location.line, p_variable.location.col);
                }
                else {
                    const_var.push(tem_entry->sym_name);
                    var_ref_num[tem_entry->sym_name] = 0;
                    double_error[tem_entry->sym_name] = false;
                    (*tables.top()).addSymbol(tem_entry);
                }
            }
        }
        else {
            if(ispara){
                Attribute *tem_attri = new Attribute(SET_SCALAR, BOOL_TYPE);
                SymbolEntry *tem_entry = new SymbolEntry(p_variable.getNameCString(), PARA_KIND, SET_SCALAR, BOOL_TYPE, tem_attri);
                if((*tables.top()).isexist(tem_entry)) {
                    iserror = 1;
                    double_error[tem_entry->sym_name] = 1;
                    error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                    error_msg += "symbol '";
                    error_msg += tem_entry->sym_name;
                    error_msg += "' is redeclared\n";
                    error2(p_variable.location.line, p_variable.location.col);
                }
                else {
                    if(modify_loopvar(tem_entry->sym_name)){ //loop var
                        iserror = 1;
                        double_error[tem_entry->sym_name] = 1;
                        error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                        error_msg += "symbol '";
                        error_msg += tem_entry->sym_name;
                        error_msg += "' is redeclared\n";
                        error2(p_variable.location.line, p_variable.location.col);
                    }
                    else {
                        tem_info.push_back("boolean");
                        tem_name.push_back(tem_entry->sym_name);
                        var_ref_num[tem_entry->sym_name] = 0;
                        double_error[tem_entry->sym_name] = false;
                        (*tables.top()).addSymbol(tem_entry);
                    }
                }
            }
            else {
                Attribute *tem_attri = new Attribute(SET_SCALAR, BOOL_TYPE);
                SymbolEntry *tem_entry = new SymbolEntry(p_variable.getNameCString(), VAR_KIND, SET_SCALAR, BOOL_TYPE, tem_attri);
                if((*tables.top()).isexist(tem_entry)) {
                    iserror = 1;
                    double_error[tem_entry->sym_name] = 1;
                    error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                    error_msg += "symbol '";
                    error_msg += tem_entry->sym_name;
                    error_msg += "' is redeclared\n";
                    error2(p_variable.location.line, p_variable.location.col);
                }
                else {
                    if(modify_loopvar(tem_entry->sym_name)){ //loop var
                        iserror = 1;
                        double_error[tem_entry->sym_name] = 1;
                        error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                        error_msg += "symbol '";
                        error_msg += tem_entry->sym_name;
                        error_msg += "' is redeclared\n";
                        error2(p_variable.location.line, p_variable.location.col);
                    }
                    else {
                        var_ref_num[tem_entry->sym_name] = 0;
                        double_error[tem_entry->sym_name] = false;
                        (*tables.top()).addSymbol(tem_entry);
                        PType* push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                        expr_stack.push(*push_type);
                    }
                }
            }
        }
    }
    else { //array type
        if(ispara){
            Attribute *tem_attri = new Attribute(UNKNOWN_SET, VOID_TYPE);
            SymbolEntry *tem_entry = new SymbolEntry(p_variable.getNameCString(), PARA_KIND, SET_SCALAR, p_variable.get_enumType(), tem_attri);
            if((*tables.top()).isexist(tem_entry)) {
                iserror = 1;
                double_error[tem_entry->sym_name] = 1;
                error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                error_msg += "symbol '";
                error_msg += tem_entry->sym_name;
                error_msg += "' is redeclared\n";
                error2(p_variable.location.line, p_variable.location.col);
            }
            else {
                if(modify_loopvar(tem_entry->sym_name)){ //loop var
                    iserror = 1;
                    double_error[tem_entry->sym_name] = 1;
                    error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                    error_msg += "symbol '";
                    error_msg += tem_entry->sym_name;
                    error_msg += "' is redeclared\n";
                    error2(p_variable.location.line, p_variable.location.col);
                }
                else {
                    if(p_variable.array_legal()){
                        tem_info.push_back(p_variable.get_enumType());
                        tem_name.push_back(tem_entry->sym_name);
                        var_ref_num[tem_entry->sym_name] = p_variable.get_child_size();
                        double_error[tem_entry->sym_name] = false;
                        (*tables.top()).addSymbol(tem_entry);
                    }   
                    else {
                        (*tables.top()).addSymbol(tem_entry);
                        iserror = 1;
                        double_error[tem_entry->sym_name] = 1;
                        iserrored.push_back(tem_entry->sym_name);
                        error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                        error_msg += "'";
                        error_msg += tem_entry->sym_name;
                        error_msg += "' declared as an array with an index that is not greater than 0\n";
                        error2(p_variable.location.line, p_variable.location.col);
                    }
                }
            }
        }
        else {
            Attribute *tem_attri = new Attribute(UNKNOWN_SET, VOID_TYPE);
            SymbolEntry *tem_entry = new SymbolEntry(p_variable.getNameCString(), VAR_KIND, SET_SCALAR, p_variable.get_enumType(), tem_attri);
            if((*tables.top()).isexist(tem_entry)) {
                iserror = 1;
                double_error[tem_entry->sym_name] = 1;
                error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                error_msg += "symbol '";
                error_msg += tem_entry->sym_name;
                error_msg += "' is redeclared\n";
                error2(p_variable.location.line, p_variable.location.col);
            }
            else {
                if(modify_loopvar(tem_entry->sym_name)){ //loop var
                    iserror = 1;
                    double_error[tem_entry->sym_name] = 1;
                    error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                    error_msg += "symbol '";
                    error_msg += tem_entry->sym_name;
                    error_msg += "' is redeclared\n";
                    error2(p_variable.location.line, p_variable.location.col);
                }
                else {
                    if(p_variable.array_legal()){
                        var_ref_num[tem_entry->sym_name] = p_variable.get_child_size();
                        double_error[tem_entry->sym_name] = false;
                        (*tables.top()).addSymbol(tem_entry);
                    }
                    else {
                        (*tables.top()).addSymbol(tem_entry);
                        iserror = 1;
                        double_error[tem_entry->sym_name] = 1;
                        iserrored.push_back(tem_entry->sym_name);
                        error1(to_string(p_variable.location.line), to_string(p_variable.location.col));
                        error_msg += "'";
                        error_msg += tem_entry->sym_name;
                        error_msg += "' declared as an array with an index that is not greater than 0\n";
                        error2(p_variable.location.line, p_variable.location.col);
                    }
                }
            }
        }
        
    }
    p_variable.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {  
    if(isloop && !isloopassign){
        int tem = p_constant_value.get_intval();
        for_val.push_back(tem);
    } 
    expr_stack.push(*(p_constant_value.getTypePtr()));
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
    if(p_function.get_enumType()=="integer"){
        func_type = "integer";
        Attribute *func_attri = new Attribute(SET_SCALAR, INT_TYPE);
        if(p_function.get_paraType().size()!=0){
            func_attri->func_arg = p_function.get_paraType()[0];
            for(int i=1;i<p_function.get_paraType().size();i++){
                func_attri->func_arg += ", ";
                func_attri->func_arg += p_function.get_paraType()[i];
            }
        }
        SymbolEntry *func_entry = new SymbolEntry(p_function.getNameCString(), FUNCTION_KIND, SET_SCALAR, INT_TYPE, func_attri);
        if(check_exist(func_entry->sym_name)) {
            iserror = 1;
            error1(to_string(p_function.location.line), to_string(p_function.location.col));
            error_msg += "symbol '";
            error_msg += func_entry->sym_name;
            error_msg += "' is redeclared\n";
            error2(p_function.location.line, p_function.location.col);
        }
        else {
            double_error[func_entry->sym_name] = false;
            func_var.push(func_entry->sym_name);
            func_rettype.push_back("integer");
            (*tables.top()).addSymbol(func_entry);
        }
    }
    else if(p_function.get_enumType()=="real"){
        func_type = "real";
        Attribute *func_attri = new Attribute(SET_SCALAR, REAL_TYPE);
        if(p_function.get_paraType().size()!=0){
            func_attri->func_arg = p_function.get_paraType()[0];
            for(int i=1;i<p_function.get_paraType().size();i++){
                func_attri->func_arg += ", ";
                func_attri->func_arg += p_function.get_paraType()[i];
            }
        }
        SymbolEntry *func_entry = new SymbolEntry(p_function.getNameCString(), FUNCTION_KIND, SET_SCALAR, REAL_TYPE, func_attri);
        if(check_exist(func_entry->sym_name)) {
            iserror = 1;
            error1(to_string(p_function.location.line), to_string(p_function.location.col));
            error_msg += "symbol '";
            error_msg += func_entry->sym_name;
            error_msg += "' is redeclared\n";
            error2(p_function.location.line, p_function.location.col);
        }
        else {
            double_error[func_entry->sym_name] = false;
            func_var.push(func_entry->sym_name);
            func_rettype.push_back("real");
            (*tables.top()).addSymbol(func_entry);
        }
    }
    else if(p_function.get_enumType()=="string"){
        func_type = "string";
        Attribute *func_attri = new Attribute(SET_SCALAR, STR_TYPE);
        if(p_function.get_paraType().size()!=0){
            func_attri->func_arg = p_function.get_paraType()[0];
            for(int i=1;i<p_function.get_paraType().size();i++){
                func_attri->func_arg += ", ";
                func_attri->func_arg += p_function.get_paraType()[i];
            }
        }
        SymbolEntry *func_entry = new SymbolEntry(p_function.getNameCString(), FUNCTION_KIND, SET_SCALAR, STR_TYPE, func_attri);
        if(check_exist(func_entry->sym_name)) {
            iserror = 1;
            error1(to_string(p_function.location.line), to_string(p_function.location.col));
            error_msg += "symbol '";
            error_msg += func_entry->sym_name;
            error_msg += "' is redeclared\n";
            error2(p_function.location.line, p_function.location.col);
        }
        else {
            double_error[func_entry->sym_name] = false;
            func_var.push(func_entry->sym_name);
            func_rettype.push_back("string");
            (*tables.top()).addSymbol(func_entry);
        }
    }
    else if(p_function.get_enumType()=="boolean"){
        func_type = "boolean";
        Attribute *func_attri = new Attribute(SET_SCALAR, BOOL_TYPE);
        if(p_function.get_paraType().size()!=0){
            func_attri->func_arg = p_function.get_paraType()[0];
            for(int i=1;i<p_function.get_paraType().size();i++){
                func_attri->func_arg += ", ";
                func_attri->func_arg += p_function.get_paraType()[i];
            }
        }
        SymbolEntry *func_entry = new SymbolEntry(p_function.getNameCString(), FUNCTION_KIND, SET_SCALAR, BOOL_TYPE, func_attri);
        if(check_exist(func_entry->sym_name)) {
            iserror = 1;
            error1(to_string(p_function.location.line), to_string(p_function.location.col));
            error_msg += "symbol '";
            error_msg += func_entry->sym_name;
            error_msg += "' is redeclared\n";
            error2(p_function.location.line, p_function.location.col);
        }
        else {
            double_error[func_entry->sym_name] = false;
            func_var.push(func_entry->sym_name);
            func_rettype.push_back("boolean");
            (*tables.top()).addSymbol(func_entry);
        }
    }
    else if(p_function.get_enumType()=="void"){
        func_type = "void";
        Attribute *func_attri = new Attribute(SET_SCALAR, VOID_TYPE);
        if(p_function.get_paraType().size()!=0){
            func_attri->func_arg = p_function.get_paraType()[0];
            for(int i=1;i<p_function.get_paraType().size();i++){
                func_attri->func_arg += ", ";
                func_attri->func_arg += p_function.get_paraType()[i];
            }
        }
        SymbolEntry *func_entry = new SymbolEntry(p_function.getNameCString(), FUNCTION_KIND, SET_SCALAR, VOID_TYPE, func_attri);
        if(check_exist(func_entry->sym_name)) { 
            iserror = 1;
            error1(to_string(p_function.location.line), to_string(p_function.location.col));
            error_msg += "symbol '";
            error_msg += func_entry->sym_name;
            error_msg += "' is redeclared\n";
            error2(p_function.location.line, p_function.location.col);
        }
        else {
            double_error[func_entry->sym_name] = false;
            func_var.push(func_entry->sym_name);
            func_rettype.push_back("void");
            (*tables.top()).addSymbol(func_entry);
        }
    }
    else {
        cout<<"error2"<<endl;
    }
    level++;
    SymbolTable* comp_table = new SymbolTable(level);
    this->pushScope(comp_table);

    ispara = 1;
    ifinfunc = 1;
    p_function.visitChildNodes(*this);
    para_info.push_back(tem_info);
    para_name.push_back(tem_name);
    tem_info.resize(0);
    tem_name.resize(0);
    this->print_table();
    func_type = "";
    this->popScope();
    level--;
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
    ispara = 0;
    if(isloop)
        isloopassign = 1;
    
    if(ifinfunc) {
        ifinfunc = 0;
        p_compound_statement.visitChildNodes(*this);
    }
    else {
        level++;
        SymbolTable* comp_table = new SymbolTable(level);
        this->pushScope(comp_table);
        p_compound_statement.visitChildNodes(*this);
        this->print_table();
        this->popScope();
        level--;
    }
    /*if(isdoubleloop)
        isdoubleloop = 0;*/
    //else{
        isloop = 0;
        isloopassign = 0;
    //}
    
}

void SemanticAnalyzer::visit(PrintNode &p_print) {
    p_print.visitChildNodes(*this);
    
    if(!expr_stack.empty()){
        string expr = expr_stack.top().getPTypeCString();
        PType p_expr = expr_stack.top();
        if(p_expr.get_dimension_size()!=0 || expr == "void"){
            if(expr != "error"){
                iserror = 1;
                error1(to_string(p_print.location.line), to_string(p_print.location.col+6));
                error_msg += "expression of print statement must be scalar type\n";
                error2(p_print.location.line, p_print.location.col+6);
                PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                expr_stack.push(*err);
            }
            else {
                expr_stack.push(p_expr);
            }
        }
    }
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    p_bin_op.visitChildNodes(*this);
    switch(p_bin_op.get_enum()){
        case Operator::kPlusOp:
            if(!expr_stack.empty()){
                string right = expr_stack.top().getPTypeCString();
                PType p_right = expr_stack.top();
                expr_stack.pop();
                string left = expr_stack.top().getPTypeCString();
                PType p_left = expr_stack.top();
                expr_stack.pop();
                if(right == left && (right == "integer" || right == "real" || right == "string")) {
                    expr_stack.push(p_right);
                }
                else if(right == "integer" && left == "real") {
                    expr_stack.push(p_left);
                }
                else if(right == "real" && left == "integer") {
                    expr_stack.push(p_right);
                }
                else {
                    if(right != "error" && left != "error"){
                        iserror = 1;
                        error1(to_string(p_bin_op.location.line), to_string(p_bin_op.location.col));
                        error_msg += "invalid operands to binary operator '+' ('";
                        error_msg += left;
                        error_msg += "' and '";
                        error_msg += right;
                        error_msg += "')\n";
                        error2(p_bin_op.location.line, p_bin_op.location.col);
                        PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                        expr_stack.push(*err);
                    }
                    
                }
            }
            break;
        case Operator::kMinusOp:
            if(!expr_stack.empty()){
                string right = expr_stack.top().getPTypeCString();
                PType p_right = expr_stack.top();
                expr_stack.pop();
                string left = expr_stack.top().getPTypeCString();
                PType p_left = expr_stack.top();
                expr_stack.pop();
                if(right == left && (right == "integer" || right == "real")) {
                    expr_stack.push(p_right);
                }
                else if(right == "integer" && left == "real") {
                    expr_stack.push(p_left);
                }
                else if(right == "real" && left == "integer") {
                    expr_stack.push(p_right);
                }
                else {
                    if(right != "error" && left != "error"){
                        iserror = 1;
                        error1(to_string(p_bin_op.location.line), to_string(p_bin_op.location.col));
                        error_msg += "invalid operands to binary operator '-' ('";
                        error_msg += left;
                        error_msg += "' and '";
                        error_msg += right;
                        error_msg += "')\n";
                        error2(p_bin_op.location.line, p_bin_op.location.col);
                        PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                        expr_stack.push(*err);
                    }
                    
                }
            }
            break;
        case Operator::kMultiplyOp:
            if(!expr_stack.empty()){
                string right = expr_stack.top().getPTypeCString();
                PType p_right = expr_stack.top();
                expr_stack.pop();
                string left = expr_stack.top().getPTypeCString();
                PType p_left = expr_stack.top();
                expr_stack.pop();
                if(right == left && (right == "integer" || right == "real")) {
                    expr_stack.push(p_right);
                }
                else if(right == "integer" && left == "real") {
                    expr_stack.push(p_left);
                }
                else if(right == "real" && left == "integer") {
                    expr_stack.push(p_right);
                }
                else {
                    if(right != "error" && left != "error"){
                        iserror = 1;
                        error1(to_string(p_bin_op.location.line), to_string(p_bin_op.location.col));
                        error_msg += "invalid operands to binary operator '*' ('";
                        error_msg += left;
                        error_msg += "' and '";
                        error_msg += right;
                        error_msg += "')\n";
                        error2(p_bin_op.location.line, p_bin_op.location.col);
                        PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                        expr_stack.push(*err);
                    }
                    
                }
            }
            break;
        case Operator::kDivideOp:
            if(!expr_stack.empty()){
                string right = expr_stack.top().getPTypeCString();
                PType p_right = expr_stack.top();
                expr_stack.pop();
                string left = expr_stack.top().getPTypeCString();
                PType p_left = expr_stack.top();
                expr_stack.pop();
                if(right == left && (right == "integer" || right == "real")) {
                    expr_stack.push(p_right);
                }
                else if(right == "integer" && left == "real") {
                    expr_stack.push(p_left);
                }
                else if(right == "real" && left == "integer") {
                    expr_stack.push(p_right);
                }
                else {
                    if(right != "error" && left != "error"){
                        iserror = 1;
                        error1(to_string(p_bin_op.location.line), to_string(p_bin_op.location.col));
                        error_msg += "invalid operands to binary operator '/' ('";
                        error_msg += left;
                        error_msg += "' and '";
                        error_msg += right;
                        error_msg += "')\n";
                        error2(p_bin_op.location.line, p_bin_op.location.col);
                        PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                        expr_stack.push(*err);
                    }
                }
            }
            break;
        case Operator::kModOp:
            if(!expr_stack.empty()){
                string right = expr_stack.top().getPTypeCString();
                PType p_right = expr_stack.top();
                expr_stack.pop();
                string left = expr_stack.top().getPTypeCString();
                PType p_left = expr_stack.top();
                expr_stack.pop();
                if(right == left && right == "integer") {
                    expr_stack.push(p_right);
                }
                else {
                    if(right != "error" && left != "error"){
                        iserror = 1;
                        error1(to_string(p_bin_op.location.line), to_string(p_bin_op.location.col));
                        error_msg += "invalid operands to binary operator 'mod' ('";
                        error_msg += left;
                        error_msg += "' and '";
                        error_msg += right;
                        error_msg += "')\n";
                        error2(p_bin_op.location.line, p_bin_op.location.col);
                        PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                        expr_stack.push(*err);
                    }
                    
                }
            }
            break;
        case Operator::kLessOp:
            if(!expr_stack.empty()){
                string right = expr_stack.top().getPTypeCString();
                PType p_right = expr_stack.top();
                expr_stack.pop();
                string left = expr_stack.top().getPTypeCString();
                PType p_left = expr_stack.top();
                expr_stack.pop();
                PType *push_type;
                if(right == left && (right == "integer" || right == "real")) {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else if(right == "integer" && left == "real") {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else if(right == "real" && left == "integer") {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else {
                    if(right != "error" && left != "error"){
                        iserror = 1;
                        error1(to_string(p_bin_op.location.line), to_string(p_bin_op.location.col));
                        error_msg += "invalid operands to binary operator '<' ('";
                        error_msg += left;
                        error_msg += "' and '";
                        error_msg += right;
                        error_msg += "')\n";
                        error2(p_bin_op.location.line, p_bin_op.location.col);
                        PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                        expr_stack.push(*err);
                    }
                }
            }
            break;
        case Operator::kLessOrEqualOp:
            if(!expr_stack.empty()){
                string right = expr_stack.top().getPTypeCString();
                PType p_right = expr_stack.top();
                expr_stack.pop();
                string left = expr_stack.top().getPTypeCString();
                PType p_left = expr_stack.top();
                expr_stack.pop();
                PType *push_type;
                if(right == left && (right == "integer" || right == "real")) {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else if(right == "integer" && left == "real") {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else if(right == "real" && left == "integer") {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else {
                    if(right != "error" && left != "error"){
                        iserror = 1;
                        error1(to_string(p_bin_op.location.line), to_string(p_bin_op.location.col));
                        error_msg += "invalid operands to binary operator '<=' ('";
                        error_msg += left;
                        error_msg += "' and '";
                        error_msg += right;
                        error_msg += "')\n";
                        error2(p_bin_op.location.line, p_bin_op.location.col);
                        PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                        expr_stack.push(*err);
                    }
                }
            }
            break;
        case Operator::kEqualOp:
            if(!expr_stack.empty()){
                string right = expr_stack.top().getPTypeCString();
                PType p_right = expr_stack.top();
                expr_stack.pop();
                string left = expr_stack.top().getPTypeCString();
                PType p_left = expr_stack.top();
                expr_stack.pop();
                PType *push_type;
                if(right == left && (right == "integer" || right == "real")) {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else if(right == "integer" && left == "real") {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else if(right == "real" && left == "integer") {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else {
                    if(right != "error" && left != "error"){
                        iserror = 1;
                        error1(to_string(p_bin_op.location.line), to_string(p_bin_op.location.col));
                        error_msg += "invalid operands to binary operator '=' ('";
                        error_msg += left;
                        error_msg += "' and '";
                        error_msg += right;
                        error_msg += "')\n";
                        error2(p_bin_op.location.line, p_bin_op.location.col);
                        PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                        expr_stack.push(*err);
                    }
                }
            }
            break;
        case Operator::kGreaterOrEqualOp:
            if(!expr_stack.empty()){
                string right = expr_stack.top().getPTypeCString();
                PType p_right = expr_stack.top();
                expr_stack.pop();
                string left = expr_stack.top().getPTypeCString();
                PType p_left = expr_stack.top();
                expr_stack.pop();
                PType *push_type;
                if(right == left && (right == "integer" || right == "real")) {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else if(right == "integer" && left == "real") {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else if(right == "real" && left == "integer") {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else {
                    if(right != "error" && left != "error"){
                        iserror = 1;
                        error1(to_string(p_bin_op.location.line), to_string(p_bin_op.location.col));
                        error_msg += "invalid operands to binary operator '>=' ('";
                        error_msg += left;
                        error_msg += "' and '";
                        error_msg += right;
                        error_msg += "')\n";
                        error2(p_bin_op.location.line, p_bin_op.location.col);
                        PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                        expr_stack.push(*err);
                    }
                }
            }
            break;
        case Operator::kGreaterOp:
            if(!expr_stack.empty()){
                string right = expr_stack.top().getPTypeCString();
                PType p_right = expr_stack.top();
                expr_stack.pop();
                string left = expr_stack.top().getPTypeCString();
                PType p_left = expr_stack.top();
                expr_stack.pop();
                PType *push_type;
                if(right == left && (right == "integer" || right == "real")) {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else if(right == "integer" && left == "real") {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else if(right == "real" && left == "integer") {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else {
                    if(right != "error" && left != "error"){
                        iserror = 1;
                        error1(to_string(p_bin_op.location.line), to_string(p_bin_op.location.col));
                        error_msg += "invalid operands to binary operator '>' ('";
                        error_msg += left;
                        error_msg += "' and '";
                        error_msg += right;
                        error_msg += "')\n";
                        error2(p_bin_op.location.line, p_bin_op.location.col);
                        PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                        expr_stack.push(*err);
                    }
                }
            }
            break;
        case Operator::kNotEqualOp:
            if(!expr_stack.empty()){
                string right = expr_stack.top().getPTypeCString();
                PType p_right = expr_stack.top();
                expr_stack.pop();
                string left = expr_stack.top().getPTypeCString();
                PType p_left = expr_stack.top();
                expr_stack.pop();
                PType *push_type;
                if(right == left && (right == "integer" || right == "real")) {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else if(right == "integer" && left == "real") {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else if(right == "real" && left == "integer") {
                    push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else {
                    if(right != "error" && left != "error"){
                        iserror = 1;
                        error1(to_string(p_bin_op.location.line), to_string(p_bin_op.location.col));
                        error_msg += "invalid operands to binary operator '<>' ('";
                        error_msg += left;
                        error_msg += "' and '";
                        error_msg += right;
                        error_msg += "')\n";
                        error2(p_bin_op.location.line, p_bin_op.location.col);
                        PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                        expr_stack.push(*err);
                    }
                }
            }
            break;
        case Operator::kAndOp:
            if(!expr_stack.empty()){
                string right = expr_stack.top().getPTypeCString();
                PType p_right = expr_stack.top();
                expr_stack.pop();
                string left = expr_stack.top().getPTypeCString();
                PType p_left = expr_stack.top();
                expr_stack.pop();
                //cout<<left<<" "<<right<<" "<<p_bin_op.location.line<<" "<<p_bin_op.location.col<<endl;
                if(right == left && right == "boolean") {
                    expr_stack.push(p_right);
                }
                else if(right == "integer" && left == "real") {
                    expr_stack.push(p_left);
                }
                else if(right == "real" && left == "integer") {
                    expr_stack.push(p_right);
                }
                else {
                    if(right != "error" ){
                        if(left == "error") {
                            iserror = 1;
                            error1(to_string(p_bin_op.location.line), to_string(p_bin_op.location.col));
                            error_msg += "invalid operands to binary operator 'and' ('";
                            error_msg += "integer";
                            error_msg += "' and '";
                            error_msg += right;
                            error_msg += "')\n";
                            error2(p_bin_op.location.line, p_bin_op.location.col);
                            PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                            expr_stack.push(*err);
                        }
                        else {
                            iserror = 1;
                            error1(to_string(p_bin_op.location.line), to_string(p_bin_op.location.col));
                            error_msg += "invalid operands to binary operator 'and' ('";
                            error_msg += left;
                            error_msg += "' and '";
                            error_msg += right;
                            error_msg += "')\n";
                            error2(p_bin_op.location.line, p_bin_op.location.col);
                            PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                            expr_stack.push(*err);
                        }   
                    }
                }
            }
            break;
        case Operator::kOrOp:
            if(!expr_stack.empty()){
                string right = expr_stack.top().getPTypeCString();
                PType p_right = expr_stack.top();
                expr_stack.pop();
                string left = expr_stack.top().getPTypeCString();
                PType p_left = expr_stack.top();
                expr_stack.pop();
                if(right == left && right == "boolean") { 
                    expr_stack.push(p_right);
                }
                else if(right == "integer" && left == "real") {
                    expr_stack.push(p_left);
                }
                else if(right == "real" && left == "integer") {
                    expr_stack.push(p_right);
                }
                else {
                    if(right != "error" && left != "error"){
                        iserror = 1;
                        error1(to_string(p_bin_op.location.line), to_string(p_bin_op.location.col));
                        error_msg += "invalid operands to binary operator 'or' ('";
                        error_msg += left;
                        error_msg += "' and '";
                        error_msg += right;
                        error_msg += "')\n";
                        error2(p_bin_op.location.line, p_bin_op.location.col);
                        PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                        expr_stack.push(*err);
                    }
                }
            }
            break;
        default: cout<<"error in binary operand"<<endl;;
    }
    
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
    p_un_op.visitChildNodes(*this);
    switch(p_un_op.get_enum()){
        case Operator::kNegOp:
            if(!expr_stack.empty()){
                string left = expr_stack.top().getPTypeCString();
                PType p_left = expr_stack.top();
                expr_stack.pop();
                if(left == "integer" || left == "real"){
                    expr_stack.push(p_left);
                }
                else {
                    if(left != "error"){
                        iserror = 1;
                        error1(to_string(p_un_op.location.line), to_string(p_un_op.location.col));
                        error_msg += "invalid operand to unary operator '-' ('";
                        error_msg += left;
                        error_msg += "')\n";
                        error2(p_un_op.location.line, p_un_op.location.col);
                        PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                        expr_stack.push(*err);
                    }
                    else expr_stack.push(p_left);
                }
            }
            break;
        case Operator::kNotOp:
            if(!expr_stack.empty()){
                string left = expr_stack.top().getPTypeCString();
                PType p_left = expr_stack.top();
                expr_stack.pop();
                if(left == "boolean"){
                    expr_stack.push(p_left);
                }
                else {
                    if(left != "error"){
                        iserror = 1;
                        error1(to_string(p_un_op.location.line), to_string(p_un_op.location.col));
                        error_msg += "invalid operand to unary operator 'not' ('";
                        error_msg += left;
                        error_msg += "')\n";
                        error2(p_un_op.location.line, p_un_op.location.col);
                        PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                        expr_stack.push(*err);
                    }
                    else expr_stack.push(p_left);
                }
            }
            break;
        default: cout<<"error in unary operand"<<endl;
    }
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    p_func_invocation.visitChildNodes(*this);
    string func_inv = p_func_invocation.getNameCString();
    vector <string> func_inv_para;
    if(!expr_stack.empty()){
        for(int i=0;i<p_func_invocation.arguments.size();i++){
            string tem = expr_stack.top().getPTypeCString();
            func_inv_para.insert(func_inv_para.begin(),tem);
            expr_stack.pop();
        }
    }
    
    if(!check_exist(func_inv) && !modify_funcvar(func_inv)){
        iserror = 1;
        error1(to_string(p_func_invocation.location.line), to_string(p_func_invocation.location.col));
        error_msg += "use of undeclared symbol '";
        error_msg += func_inv;
        error_msg += "'\n";
        error2(p_func_invocation.location.line, p_func_invocation.location.col);
        PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
        expr_stack.push(*err);
    }
    else if(!modify_funcvar(func_inv)){
        iserror = 1;
        error1(to_string(p_func_invocation.location.line), to_string(p_func_invocation.location.col));
        error_msg += "call of non-function symbol '";
        error_msg += func_inv;
        error_msg += "'\n";
        error2(p_func_invocation.location.line, p_func_invocation.location.col);
        PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
        expr_stack.push(*err);
    }
    else {
        int idx = func_index(func_inv);
        if(para_info[idx].size()!=func_inv_para.size()){
            iserror = 1;
            error1(to_string(p_func_invocation.location.line), to_string(p_func_invocation.location.col));
            error_msg += "too few/much arguments provided for function '";
            error_msg += func_inv;
            error_msg += "'\n";
            error2(p_func_invocation.location.line, p_func_invocation.location.col);
            PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
            expr_stack.push(*err);
        }
        else {
            bool isparaerr = 0;
            int wrong_pos;
            for(int i=0;i<para_info[idx].size();i++){
                if(para_info[idx][i]!=func_inv_para[i]){
                    isparaerr = 1;
                    wrong_pos = i;
                }
            }
            if(isparaerr){
                iserror = 1;
                error1(to_string(p_func_invocation.arguments[wrong_pos]->getLocation().line), to_string(p_func_invocation.arguments[wrong_pos]->getLocation().col));
                error_msg += "incompatible type passing '";
                error_msg += func_inv_para[wrong_pos];
                error_msg += "' to parameter of type '";
                error_msg += para_info[idx][wrong_pos];
                error_msg += "'\n";
                error2(p_func_invocation.arguments[wrong_pos]->getLocation().line, p_func_invocation.arguments[wrong_pos]->getLocation().col);
                PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                expr_stack.push(*err);
            }
            else {
                if(func_rettype[idx]=="integer"){
                    PType* push_type = new PType(PType::PrimitiveTypeEnum::kIntegerType);
                    expr_stack.push(*push_type);
                }
                else if(func_rettype[idx]=="real"){
                    PType* push_type = new PType(PType::PrimitiveTypeEnum::kRealType);
                    expr_stack.push(*push_type);
                }
                else if(func_rettype[idx]=="string"){
                    PType* push_type = new PType(PType::PrimitiveTypeEnum::kStringType);
                    expr_stack.push(*push_type);
                }
                else if(func_rettype[idx]=="boolean"){
                    PType* push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                    expr_stack.push(*push_type);
                }
                else if(func_rettype[idx]=="void"){
                    PType* push_type = new PType(PType::PrimitiveTypeEnum::kVoidType);
                    expr_stack.push(*push_type);
                }
                
            }
        }
    }
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    string var_ref_name = p_variable_ref.getNameCString();
    p_variable_ref.visitChildNodes(*this);
    string true_type = get_shorttype(var_ref_name);
    int true_size = get_arrsize(var_ref_name);
    int now_size = p_variable_ref.get_child_size();
    bool error_happened = 0;
    bool notinteger;
    int happen_loc;
    PType* push_type;
    if(get_shorttype(var_ref_name)=="integer"){
        push_type = new PType(PType::PrimitiveTypeEnum::kIntegerType);
    }
    else if(get_shorttype(var_ref_name)=="real"){
        push_type = new PType(PType::PrimitiveTypeEnum::kRealType);
    }
    else if(get_shorttype(var_ref_name)=="string"){
        push_type = new PType(PType::PrimitiveTypeEnum::kStringType);
    }
    else if(get_shorttype(var_ref_name)=="boolean"){
        push_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
    }

    if(!expr_stack.empty()){
        notinteger = 0;
        vector <string> child_type;
        for(int i=0;i<now_size;i++){
            string tem = expr_stack.top().getPTypeCString();
            if(tem == "error") error_happened = 1;
            if(tem != "integer") {
                notinteger = 1;
                happen_loc = i;
            } 
            child_type.push_back(tem);
            expr_stack.pop();
        } 
        if(now_size!=-1){
            if(now_size==true_size && true_size == 0){
                vector <uint64_t> empt;
                push_type->setDimensions(empt);
                expr_stack.push(*push_type);
            }
            else if(now_size>true_size){
                PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                expr_stack.push(*err);
            }
            else {
                int diff = true_size - now_size;
                vector <string> child = get_arr(var_ref_name);
                vector <uint64_t> now_child;
                for(int i = true_size-diff;i<true_size;i++){
                    uint64_t tem = atoi(child[i].c_str());
                    now_child.push_back(tem);
                }
                push_type->setDimensions(now_child);
                expr_stack.push(*push_type);
            }
        }
        bool ispara_exist = 0;
        for(int i=0;i<para_name.size();i++){
            for(int j=0;j<para_name[i].size();j++){
                if(var_ref_name==para_name[i][j]) {
                    ispara_exist = 1;
                }
            }
        }
        if(error_happened){}
        else if(!double_error[var_ref_name]&& !modify_loopvar(var_ref_name)){
            true_size = get_arrsize(var_ref_name);
            if(modify_funcvar(var_ref_name)){
                iserror = 1;
                error1(to_string(p_variable_ref.location.line), to_string(p_variable_ref.location.col));
                error_msg += "use of non-variable symbol '";
                error_msg += var_ref_name;
                error_msg += "'\n";
                error2(p_variable_ref.location.line, p_variable_ref.location.col);
            }
            else if(get_arrsize(var_ref_name) == -1 && !check_exist(var_ref_name) && !ispara_exist){ // 
                iserror = 1;
                error1(to_string(p_variable_ref.location.line), to_string(p_variable_ref.location.col));
                error_msg += "use of undeclared symbol '";
                error_msg += var_ref_name;
                error_msg += "'\n";
                error2(p_variable_ref.location.line, p_variable_ref.location.col);
            }
            else if(notinteger){
                iserror = 1;
                error1(to_string(p_variable_ref.indices[happen_loc]->getLocation().line), to_string(p_variable_ref.indices[happen_loc]->getLocation().col));
                error_msg += "index of array reference must be an integer\n";
                error2(p_variable_ref.indices[happen_loc]->getLocation().line, p_variable_ref.indices[happen_loc]->getLocation().col);
            }
            else if(now_size>get_arrsize(var_ref_name)&& !ispara_exist){
                iserror = 1;
                error1(to_string(p_variable_ref.location.line), to_string(p_variable_ref.location.col));
                error_msg += "there is an over array subscript on '";
                error_msg += var_ref_name;
                error_msg += "'\n";
                error2(p_variable_ref.location.line, p_variable_ref.location.col);
            }
            
        }

        true_size = get_arrsize(var_ref_name);
    }
    else {
        if(get_arrsize(var_ref_name) == -1 && !check_exist(var_ref_name) && !modify_loopvar(var_ref_name)){
            iserror = 1;
            error1(to_string(p_variable_ref.location.line), to_string(p_variable_ref.location.col));
            error_msg += "use of undeclared symbol '";
            error_msg += var_ref_name;
            error_msg += "'\n";
            error2(p_variable_ref.location.line, p_variable_ref.location.col);
        }
        
    } 

    error_happened = 0;
    notinteger = 0;
    
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
    p_assignment.visitChildNodes(*this);
    if(!expr_stack.empty()){
        string right = expr_stack.top().getPTypeCString();
        PType p_right = expr_stack.top();
        expr_stack.pop();
        string left = expr_stack.top().getPTypeCString();
        PType p_left = expr_stack.top();
        expr_stack.pop();
        //cout<<p_assignment.lvalue->getNameCString()<<" "<<modify_loopvar(p_assignment.lvalue->getNameCString())<<endl;
        if(p_left.get_dimension_size()!=0){
            iserror = 1;
            error1(to_string(p_assignment.lvalue->location.line), to_string(p_assignment.lvalue->location.col));
            error_msg += "array assignment is not allowed\n";
            error2(p_assignment.lvalue->location.line, p_assignment.lvalue->location.col);
            PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
            expr_stack.push(*err);
        }
        else if(modify_const(p_assignment.lvalue->getNameCString())){
            iserror = 1;
            error1(to_string(p_assignment.lvalue->location.line), to_string(p_assignment.lvalue->location.col));
            error_msg += "cannot assign to variable '";
            error_msg += p_assignment.lvalue->getNameCString();
            error_msg += "' which is a constant\n";
            error2(p_assignment.lvalue->location.line, p_assignment.lvalue->location.col);
            PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
            expr_stack.push(*err);
        }
        else if(p_right.get_dimension_size()!=0){
            iserror = 1;
            error1(to_string(p_assignment.expr->location.line), to_string(p_assignment.expr->location.col));
            error_msg += "array assignment is not allowed\n";
            error2(p_assignment.expr->location.line, p_assignment.expr->location.col);
            PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
            expr_stack.push(*err);
        }
        
        else if(modify_loopvar(p_assignment.lvalue->getNameCString()) && isloopassign && !double_error[p_assignment.lvalue->getNameCString()] && !p_assignment.isfromloop){//&& !isdoubleloop
            //cout<<"here"<<endl;
            iserror = 1;
            error1(to_string(p_assignment.lvalue->location.line), to_string(p_assignment.lvalue->location.col));
            error_msg += "the value of loop variable cannot be modified inside the loop body\n";
            error2(p_assignment.lvalue->location.line, p_assignment.lvalue->location.col);
            PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
            expr_stack.push(*err);
        }
        else if(right!=left && ((right!="real" || left!="integer") && (right != "integer" || left != "real")) && right!="error" && left!="error"){
            iserror = 1;
            error1(to_string(p_assignment.location.line), to_string(p_assignment.location.col));
            error_msg += "assigning to '";
            error_msg += left;
            error_msg += "' from incompatible type '";
            error_msg += right;
            error_msg += "'\n";
            error2(p_assignment.location.line, p_assignment.location.col);
            PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
            expr_stack.push(*err);
        }
        if(p_assignment.isfromloop)
            p_assignment.isfromloop = 0;
    }
}

void SemanticAnalyzer::visit(ReadNode &p_read) {
    string readname = p_read.target->getNameCString();
    p_read.visitChildNodes(*this);
    if(!expr_stack.empty()){
        string expr = expr_stack.top().getPTypeCString();
        PType p_expr = expr_stack.top();
        //expr_stack.pop();

        if(p_expr.get_dimension_size()!=0){
            if(expr != "error"){
                iserror = 1;
                error1(to_string(p_read.location.line), to_string(p_read.location.col+5));
                error_msg += "variable reference of read statement must be scalar type\n";
                error2(p_read.location.line, p_read.location.col+5);
                PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                expr_stack.push(*err);
            }
            else {
                expr_stack.push(p_expr);
            }
        }
        else if(modify_loopvar(readname)){
            if(expr != "error"){
                iserror = 1;
                error1(to_string(p_read.location.line), to_string(p_read.location.col+5));
                error_msg += "variable reference of read statement cannot be a constant or loop variable\n";
                error2(p_read.location.line, p_read.location.col+5);
                PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                expr_stack.push(*err);
            }
            else {
                iserror = 1;
                error1(to_string(p_read.location.line), to_string(p_read.location.col+5));
                error_msg += "variable reference of read statement cannot be a constant or loop variable\n";
                error2(p_read.location.line, p_read.location.col+5);
                PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                expr_stack.push(p_expr);
            }
        }
        else if(modify_const(readname)){
            if(expr != "error"){
                iserror = 1;
                error1(to_string(p_read.location.line), to_string(p_read.location.col+5));
                error_msg += "variable reference of read statement cannot be a constant or loop variable\n";
                error2(p_read.location.line, p_read.location.col+5);
                PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                expr_stack.push(*err);
            }
        }
    }

}

void SemanticAnalyzer::visit(IfNode &p_if) {
    p_if.visitChildNodes(*this);
    if(!expr_stack.empty()){
        string expr = expr_stack.top().getPTypeCString();
        PType p_expr = expr_stack.top();
        if(expr != "error"){
            if(expr != "boolean"){
                iserror = 1;
                error1(to_string(p_if.condition->getLocation().line), to_string(p_if.condition->getLocation().col));
                error_msg += "the expression of condition must be boolean type\n";
                error2(p_if.condition->getLocation().line, p_if.condition->getLocation().col);
                PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                expr_stack.push(*err);
            }
        }
    }

}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    p_while.visitChildNodes(*this);
    if(!expr_stack.empty()){
        string expr = expr_stack.top().getPTypeCString();
        PType p_expr = expr_stack.top();
        if(expr != "error"){
            if(expr != "boolean"){
                iserror = 1;
                error1(to_string(p_while.condition->getLocation().line), to_string(p_while.condition->getLocation().col));
                error_msg += "the expression of condition must be boolean type\n";
                error2(p_while.condition->getLocation().line, p_while.condition->getLocation().col);
                PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                expr_stack.push(*err);
            }
        }
    }
}

void SemanticAnalyzer::visit(ForNode &p_for) {
    level++;
    SymbolTable* comp_table = new SymbolTable(level);
    this->pushScope(comp_table);
    isloop = 1;
    p_for.set_assignbool();
    p_for.visitChildNodes(*this);
    
    if(for_val[0]>for_val[1]){
        iserror = 1;
        error1(to_string(p_for.location.line), to_string(p_for.location.col));
        error_msg += "the lower bound and upper bound of iteration count must be in the incremental order\n";
        error2(p_for.location.line, p_for.location.col);
        PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
        expr_stack.push(*err);
    }

    for_val.resize(0);
    this->print_table();
    if(!loop_var.empty())
        loop_var.pop();
    this->popScope();
    level--;
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    p_return.visitChildNodes(*this);
    if(!expr_stack.empty()){
        string expr = expr_stack.top().getPTypeCString();
        PType p_expr = expr_stack.top();

        if(expr!="error") {
            if(func_type == ""){
                iserror = 1;
                error1(to_string(p_return.location.line), to_string(p_return.location.col));
                error_msg += "program/procedure should not return a value\n";
                error2(p_return.location.line, p_return.location.col);
                PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                expr_stack.push(*err);
            }
            else if(expr!=func_type){
                iserror = 1;
                error1(to_string(p_return.location.line), to_string(p_return.location.col+7));
                error_msg += "return '";
                error_msg += expr;
                error_msg += "' from a function with return type '";
                error_msg += func_type;
                error_msg += "'\n";
                error2(p_return.location.line, p_return.location.col+7);
                PType *err = new PType(PType::PrimitiveTypeEnum::ERROR);
                expr_stack.push(*err);
            }
        }
        
    }
    
}
