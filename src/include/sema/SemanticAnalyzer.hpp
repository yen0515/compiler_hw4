#ifndef __SEMA_SEMANTIC_ANALYZER_H
#define __SEMA_SEMANTIC_ANALYZER_H

#include "visitor/AstNodeVisitor.hpp"
#include "sema/SymbolTable.hpp"
#include "AST/PType.hpp"
#include <vector>

using namespace std;


class SemanticAnalyzer : public AstNodeVisitor {
  public:
    SemanticAnalyzer(string n_filename, FILE* n_fp) ;
    ~SemanticAnalyzer() = default;
    void can_print();

    void visit(ProgramNode &p_program) override;
    void visit(DeclNode &p_decl) override;
    void visit(VariableNode &p_variable) override;
    void visit(ConstantValueNode &p_constant_value) override; 
    void visit(FunctionNode &p_function) override;
    void visit(CompoundStatementNode &p_compound_statement) override;
    void visit(PrintNode &p_print) override;
    void visit(BinaryOperatorNode &p_bin_op) override;
    void visit(UnaryOperatorNode &p_un_op) override;
    void visit(FunctionInvocationNode &p_func_invocation) override;
    void visit(VariableReferenceNode &p_variable_ref) override;
    void visit(AssignmentNode &p_assignment) override;
    void visit(ReadNode &p_read) override;
    void visit(IfNode &p_if) override;
    void visit(WhileNode &p_while) override;
    void visit(ForNode &p_for) override;
    void visit(ReturnNode &p_return) override;

    void pushScope(SymbolTable *n_scope);
    void popScope();
    void print_table();
    bool modify_loopvar(string n_name);
    bool modify_funcvar(string n_name);
    bool check_exist(string n_name);
    string get_shorttype(string n_name);
    int get_arrsize(string n_name);
    vector <string> get_arr(string n_name);
    bool modify_const(string n_name);

    int func_index(string n_name);

    void error1(string loc_line, string loc_col);
    void error2(int loc_line, int loc_col);

  public:
    // TODO: something like symbol manager (manage symbol tables)
    //       context manager, return type manager
    bool can_print_table = 0;

    bool iserror = 0;
    int level = 0;
    bool ispara = 0;
    bool isloop = 0;
    bool isloopassign = 0;
    bool isdoubleloop = 0;
    bool ifinfunc = 0;
    bool check_assign = 0;
    bool ret_func = 0;
    bool notinteger;
    string func_type = "";
    vector <vector <string> > para_info;
    vector <string> tem_info;
    vector <string> func_rettype;
    vector <int> for_val;
    vector <vector <string> > para_name;
    vector <string> tem_name;

    stack <SymbolTable*> tables;
    stack <string> loop_var;
    stack <string> func_var;
    stack <string> const_var;
    stack <PType> expr_stack;
    vector <string> iserrored;
    map <string, bool> double_error;
    map <string, int> var_ref_num;
    string error_msg = "";
    string filename;
    FILE *fp;


    
    SymbolTable *cur;
};


#endif
