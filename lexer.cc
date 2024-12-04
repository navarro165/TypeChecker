#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <set>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = {
    "END_OF_FILE",
    "INT", "REAL", "BOOL", "TRUE", "FALSE",
    "IF", "WHILE", "SWITCH", "CASE",
    "NOT", "PLUS", "MINUS", "MULT", "DIV",
    "GREATER", "LESS", "GTEQ", "LTEQ", "EQUAL", "NOTEQUAL",
    "LPAREN", "RPAREN",
    "NUM", "REALNUM",
    "COLON", "COMMA", "SEMICOLON",
    "LBRACE", "RBRACE",
    "ID", "ERROR"};

string keyword[] = {
    "int", "real", "bool", "true", "false",
    "if", "while", "switch", "case"};

TokenType keywordTokens[] = {
    INT, REAL, BOOL, TRUE, FALSE,
    IF, WHILE, SWITCH, CASE};

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int)this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    while (!input.EndOfInput() && isspace(c))
    {
        space_encountered = true;
        if (c == '\n')
        {
            line_no++;
        }
        input.GetChar(c);
    }

    if (!input.EndOfInput())
    {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < 9; i++)
    {
        if (s == keyword[i])
        {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < 9; i++)
    {
        if (s == keyword[i])
        {
            return keywordTokens[i];
        }
    }
    return ID;
}

Token LexicalAnalyzer::ScanNumber()
{
    char c;
    input.GetChar(c);

    if (isdigit(c))
    {
        if (c == '0')
        {
            tmp.lexeme = "0";
            tmp.token_type = NUM;
            tmp.line_no = line_no;

            input.GetChar(c);
            if (c == '.')
            {
                tmp.lexeme += c;
                input.GetChar(c);
                if (isdigit(c))
                {
                    tmp.lexeme += c;
                    while (true)
                    {
                        input.GetChar(c);
                        if (isdigit(c))
                        {
                            tmp.lexeme += c;
                        }
                        else
                        {
                            if (c == '\n')
                            {
                                line_no++;
                            }
                            if (!input.EndOfInput())
                            {
                                input.UngetChar(c);
                            }
                            break;
                        }
                    }
                    tmp.token_type = REALNUM;
                    tmp.line_no = line_no;
                }
                else
                {
                    // expects digit after dot
                    if (!input.EndOfInput())
                    {
                        input.UngetChar(c);
                    }
                    tmp.token_type = ERROR;
                    tmp.line_no = line_no;
                }
            }
            else
            {
                if (!input.EndOfInput())
                {
                    input.UngetChar(c);
                }
            }
            return tmp;
        }
        else
        {
            tmp.lexeme = "";
            tmp.lexeme += c;
            bool is_real = false;

            while (true)
            {
                input.GetChar(c);
                if (isdigit(c))
                {
                    tmp.lexeme += c;
                }
                else if (c == '.')
                {
                    if (is_real)
                    {
                        // means there are 2 dots in number
                        tmp.token_type = ERROR;
                        tmp.line_no = line_no;
                        return tmp;
                    }
                    else
                    {
                        is_real = true;
                        tmp.lexeme += c;
                        input.GetChar(c);
                        if (isdigit(c))
                        {
                            tmp.lexeme += c;
                            while (true)
                            {
                                input.GetChar(c);
                                if (isdigit(c))
                                {
                                    tmp.lexeme += c;
                                }
                                else
                                {
                                    if (c == '\n')
                                    {
                                        line_no++;
                                    }
                                    if (!input.EndOfInput())
                                    {
                                        input.UngetChar(c);
                                    }
                                    break;
                                }
                            }
                        }
                        else
                        {
                            // expects digit after dot
                            if (c == '\n')
                            {
                                line_no++;
                            }
                            if (!input.EndOfInput())
                            {
                                input.UngetChar(c);
                            }
                            tmp.token_type = ERROR;
                            tmp.line_no = line_no;
                            return tmp;
                        }
                    }
                }
                else
                {
                    if (c == '\n')
                    {
                        line_no++;
                    }
                    if (!input.EndOfInput())
                    {
                        input.UngetChar(c);
                    }
                    break;
                }
            }

            tmp.token_type = is_real ? REALNUM : NUM;
            tmp.line_no = line_no;
            return tmp;
        }
    }
    else
    {
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c))
    {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c))
        {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (c == '\n')
        {
            line_no++;
        }
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
        return tmp;
    }
    else
    {
        if (c == '\n')
        {
            line_no++;
        }
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::GetToken()
{
    char c;
    SkipSpace();

    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);

    if (c == '\n')
    {
        line_no++;
    }

    if (isdigit(c))
    {
        input.UngetChar(c);
        return ScanNumber();
    }
    else if (isalpha(c))
    {
        input.UngetChar(c);
        return ScanIdOrKeyword();
    }
    else if (c == '>')
    {
        char c2;
        input.GetChar(c2);
        if (c2 == '=')
        {
            tmp.lexeme = ">=";
            tmp.token_type = GTEQ;
        }
        else
        {
            if (c2 == '\n')
            {
                line_no++;
            }
            if (!input.EndOfInput())
            {
                input.UngetChar(c2);
            }
            tmp.lexeme = ">";
            tmp.token_type = GREATER;
        }
        tmp.line_no = line_no;
        return tmp;
    }
    else if (c == '<')
    {
        char c2;
        input.GetChar(c2);
        if (c2 == '=')
        {
            tmp.lexeme = "<=";
            tmp.token_type = LTEQ;
        }
        else if (c2 == '>')
        {
            tmp.lexeme = "<>";
            tmp.token_type = NOTEQUAL;
        }
        else
        {
            if (c2 == '\n')
            {
                line_no++;
            }
            if (!input.EndOfInput())
            {
                input.UngetChar(c2);
            }
            tmp.lexeme = "<";
            tmp.token_type = LESS;
        }
        tmp.line_no = line_no;
        return tmp;
    }
    else if (c == '=')
    {
        tmp.lexeme = "=";
        tmp.token_type = EQUAL;
        tmp.line_no = line_no;
        return tmp;
    }
    else if (c == ':')
    {
        tmp.lexeme = ":";
        tmp.token_type = COLON;
        tmp.line_no = line_no;
        return tmp;
    }
    else if (c == ',')
    {
        tmp.lexeme = ",";
        tmp.token_type = COMMA;
        tmp.line_no = line_no;
        return tmp;
    }
    else if (c == ';')
    {
        tmp.lexeme = ";";
        tmp.token_type = SEMICOLON;
        tmp.line_no = line_no;
        return tmp;
    }
    else if (c == '{')
    {
        tmp.lexeme = "{";
        tmp.token_type = LBRACE;
        tmp.line_no = line_no;
        return tmp;
    }
    else if (c == '}')
    {
        tmp.lexeme = "}";
        tmp.token_type = RBRACE;
        tmp.line_no = line_no;
        return tmp;
    }
    else if (c == '(')
    {
        tmp.lexeme = "(";
        tmp.token_type = LPAREN;
        tmp.line_no = line_no;
        return tmp;
    }
    else if (c == ')')
    {
        tmp.lexeme = ")";
        tmp.token_type = RPAREN;
        tmp.line_no = line_no;
        return tmp;
    }
    else if (c == '+')
    {
        tmp.lexeme = "+";
        tmp.token_type = PLUS;
        tmp.line_no = line_no;
        return tmp;
    }
    else if (c == '-')
    {
        tmp.lexeme = "-";
        tmp.token_type = MINUS;
        tmp.line_no = line_no;
        return tmp;
    }
    else if (c == '*')
    {
        tmp.lexeme = "*";
        tmp.token_type = MULT;
        tmp.line_no = line_no;
        return tmp;
    }
    else if (c == '/')
    {
        tmp.lexeme = "/";
        tmp.token_type = DIV;
        tmp.line_no = line_no;
        return tmp;
    }
    else if (c == '!')
    {
        tmp.lexeme = "!";
        tmp.token_type = NOT;
        tmp.line_no = line_no;
        return tmp;
    }
    else if (c == EOF || input.EndOfInput())
    {
        tmp.token_type = END_OF_FILE;
        tmp.line_no = line_no;
        return tmp;
    }
    else
    {
        if (c == '\n')
        {
            line_no++;
        }
        tmp.lexeme = c;
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

// new parser and type checker logic
// *********************************

enum SimpleType
{
    TYPE_INT,
    TYPE_REAL,
    TYPE_BOOL,
    TYPE_UNKNOWN
};

struct TypeInfo
{
    SimpleType type;
    set<string> equalVars;
};

map<string, TypeInfo> symbolTable;
vector<string> varOrder;

class Parser
{
public:
    void ParseProgram();

private:
    LexicalAnalyzer lexer;
    Token token;
    bool typeError = false;
    int error_line_no;
    string error_constraint;

    // parser helpers
    void ParseGlobalVars();
    void ParseVarDeclList();
    void ParseVarDecl();
    void ParseVarList(vector<string> &varNames);
    void ParseTypeName(SimpleType &varType);
    void ParseBody();
    void ParseStmtList();
    void ParseStmt();
    void ParseAssignmentStmt();
    void ParseIfStmt();
    void ParseWhileStmt();
    void ParseSwitchStmt();
    void ParseCaseList();
    void ParseCase();
    SimpleType ParseExpression(string &varName, int &expr_line_no);
    SimpleType ParsePrimary(string &varName, int &primary_line_no);

    void EnforceType(SimpleType leftType, SimpleType rightType, int line_no, string constraint, string leftVar, string rightVar);

    // for type checking
    void EnforceType(SimpleType leftType, SimpleType rightType, int line_no, string constraint);
    void UnifyTypes(string var1, string var2);
    void UpdateType(string varName, SimpleType type);

    // more helpers
    void SyntaxError();
    void Expect(TokenType expected);
    void ReportTypeError(int line_no, string constraint);
};

void Parser::SyntaxError()
{
    cout << "Syntax Error" << endl;
    exit(1);
}

void Parser::Expect(TokenType expected)
{
    if (token.token_type != expected)
    {
        SyntaxError();
    }
    token = lexer.GetToken();
}

void Parser::ReportTypeError(int line_no, string constraint)
{
    if (!typeError)
    {
        typeError = true;
        error_line_no = line_no;
        error_constraint = constraint;
    }
}

void Parser::ParseProgram()
{
    token = lexer.GetToken();
    ParseGlobalVars();
    ParseBody();
    if (token.token_type != END_OF_FILE)
    {
        SyntaxError();
    }

    if (typeError)
    {
        cout << "TYPE MISMATCH " << error_line_no << " " << error_constraint << endl;
    }
    else
    {
        // output variable types
        set<string> printed;
        for (string varName : varOrder)
        {
            if (printed.find(varName) == printed.end())
            {
                TypeInfo &info = symbolTable[varName];
                if (info.type == TYPE_INT)
                {
                    cout << varName << ": int #" << endl;
                    printed.insert(varName);
                }
                else if (info.type == TYPE_REAL)
                {
                    cout << varName << ": real #" << endl;
                    printed.insert(varName);
                }
                else if (info.type == TYPE_BOOL)
                {
                    cout << varName << ": bool #" << endl;
                    printed.insert(varName);
                }
                else
                {
                    // group by type
                    vector<string> vars;
                    for (string v : info.equalVars)
                    {
                        if (printed.find(v) == printed.end())
                        {
                            vars.push_back(v);
                            printed.insert(v);
                        }
                    }
                    for (size_t i = 0; i < vars.size(); ++i)
                    {
                        cout << vars[i];
                        if (i != vars.size() - 1)
                        {
                            cout << ", ";
                        }
                    }
                    cout << ": ? #" << endl;
                }
            }
        }
    }
}

void Parser::ParseGlobalVars()
{
    if (token.token_type == ID)
    {
        ParseVarDeclList();
    }
}

void Parser::ParseVarDeclList()
{
    ParseVarDecl();
    if (token.token_type == ID)
    {
        ParseVarDeclList();
    }
}

void Parser::ParseVarDecl()
{
    vector<string> varNames;
    ParseVarList(varNames);
    Expect(COLON);
    SimpleType varType;
    ParseTypeName(varType);
    Expect(SEMICOLON);

    // add to symbol table
    for (string varName : varNames)
    {
        symbolTable[varName].type = varType;
        symbolTable[varName].equalVars.insert(varName);
        varOrder.push_back(varName);
    }
}

void Parser::ParseVarList(vector<string> &varNames)
{
    if (token.token_type == ID)
    {
        varNames.push_back(token.lexeme);
        token = lexer.GetToken();
        if (token.token_type == COMMA)
        {
            token = lexer.GetToken();
            ParseVarList(varNames);
        }
    }
    else
    {
        SyntaxError();
    }
}

void Parser::ParseTypeName(SimpleType &varType)
{
    if (token.token_type == INT)
    {
        varType = TYPE_INT;
    }
    else if (token.token_type == REAL)
    {
        varType = TYPE_REAL;
    }
    else if (token.token_type == BOOL)
    {
        varType = TYPE_BOOL;
    }
    else
    {
        SyntaxError();
    }
    token = lexer.GetToken();
}

void Parser::ParseBody()
{
    Expect(LBRACE);
    ParseStmtList();
    Expect(RBRACE);
}

void Parser::ParseStmtList()
{
    ParseStmt();
    if (token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH)
    {
        ParseStmtList();
    }
}

void Parser::ParseStmt()
{
    if (token.token_type == ID)
    {
        ParseAssignmentStmt();
    }
    else if (token.token_type == IF)
    {
        ParseIfStmt();
    }
    else if (token.token_type == WHILE)
    {
        ParseWhileStmt();
    }
    else if (token.token_type == SWITCH)
    {
        ParseSwitchStmt();
    }
    else
    {
        SyntaxError();
    }
}

void Parser::ParseAssignmentStmt()
{
    if (token.token_type == ID)
    {
        string lhsVar = token.lexeme;
        if (symbolTable.find(lhsVar) == symbolTable.end())
        {
            symbolTable[lhsVar].type = TYPE_UNKNOWN;
            symbolTable[lhsVar].equalVars.insert(lhsVar);
            varOrder.push_back(lhsVar);
        }
        token = lexer.GetToken();
        Expect(EQUAL);
        string exprVar;
        int expr_line_no;
        SimpleType exprType = ParseExpression(exprVar, expr_line_no);
        Expect(SEMICOLON);

        // C1
        SimpleType lhsType = symbolTable[lhsVar].type;
        EnforceType(lhsType, exprType, expr_line_no, "C1", lhsVar, exprVar);
        UnifyTypes(lhsVar, exprVar);
    }
    else
    {
        SyntaxError();
    }
}

void Parser::ParseIfStmt()
{
    Expect(IF);
    Expect(LPAREN);
    string exprVar;
    int expr_line_no;
    SimpleType exprType = ParseExpression(exprVar, expr_line_no);
    Expect(RPAREN);

    // C4
    if (exprType != TYPE_BOOL && exprType != TYPE_UNKNOWN)
    {
        ReportTypeError(expr_line_no, "C4");
    }
    else if (exprType == TYPE_UNKNOWN && !exprVar.empty())
    {
        UpdateType(exprVar, TYPE_BOOL);
    }

    ParseBody();
}

void Parser::ParseWhileStmt()
{
    Expect(WHILE);
    Expect(LPAREN);
    string exprVar;
    int expr_line_no;
    SimpleType exprType = ParseExpression(exprVar, expr_line_no);
    Expect(RPAREN);

    // C4
    if (exprType != TYPE_BOOL)
    {
        ReportTypeError(expr_line_no, "C4");
    }

    ParseBody();
}

void Parser::ParseSwitchStmt()
{
    Expect(SWITCH);
    Expect(LPAREN);
    string exprVar;
    int expr_line_no;
    SimpleType exprType = ParseExpression(exprVar, expr_line_no);
    Expect(RPAREN);

    // C5
    if (exprType != TYPE_INT && exprType != TYPE_UNKNOWN)
    {
        ReportTypeError(expr_line_no, "C5");
    }
    else if (exprType == TYPE_UNKNOWN && !exprVar.empty())
    {
        UpdateType(exprVar, TYPE_INT);
    }

    Expect(LBRACE);
    ParseCaseList();
    Expect(RBRACE);
}

void Parser::ParseCaseList()
{
    ParseCase();
    if (token.token_type == CASE)
    {
        ParseCaseList();
    }
}

void Parser::ParseCase()
{
    Expect(CASE);
    Expect(NUM);
    Expect(COLON);
    ParseBody();
}

SimpleType Parser::ParseExpression(string &varName, int &expr_line_no)
{
    expr_line_no = token.line_no;
    if (token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM ||
        token.token_type == TRUE || token.token_type == FALSE)
    {
        return ParsePrimary(varName, expr_line_no);
    }
    else if (token.token_type == PLUS || token.token_type == MINUS ||
             token.token_type == MULT || token.token_type == DIV ||
             token.token_type == GREATER || token.token_type == LESS ||
             token.token_type == GTEQ || token.token_type == LTEQ ||
             token.token_type == EQUAL || token.token_type == NOTEQUAL)
    {
        TokenType op = token.token_type;
        int op_line_no = token.line_no;
        token = lexer.GetToken();
        string leftVar, rightVar;
        int left_line_no, right_line_no;
        SimpleType leftType = ParseExpression(leftVar, left_line_no);
        SimpleType rightType = ParseExpression(rightVar, right_line_no);

        // C2
        EnforceType(leftType, rightType, op_line_no, "C2", leftVar, rightVar);

        if (!leftVar.empty() && !rightVar.empty())
        {
            UnifyTypes(leftVar, rightVar);
        }

        if (op == PLUS || op == MINUS || op == MULT || op == DIV)
        {
            if (!leftVar.empty())
            {
                varName = leftVar;
            }
            else if (!rightVar.empty())
            {
                varName = rightVar;
            }
            else
            {
                varName = "";
            }

            if (!varName.empty())
            {
                if (!leftVar.empty())
                    UnifyTypes(varName, leftVar);
                if (!rightVar.empty())
                    UnifyTypes(varName, rightVar);
            }

            return leftType != TYPE_UNKNOWN ? leftType : rightType;
        }
        else
        {
            varName = "";
            return TYPE_BOOL;
        }
    }
    else if (token.token_type == NOT)
    {
        int op_line_no = token.line_no;
        token = lexer.GetToken();
        string exprVar;
        int expr_line_no;
        SimpleType exprType = ParseExpression(exprVar, expr_line_no);

        // C3
        if (exprType != TYPE_BOOL && exprType != TYPE_UNKNOWN)
        {
            ReportTypeError(op_line_no, "C3");
        }
        else if (exprType == TYPE_UNKNOWN && !exprVar.empty())
        {
            UpdateType(exprVar, TYPE_BOOL);
        }
        varName = "";
        return TYPE_BOOL;
    }
    else
    {
        SyntaxError();
        return TYPE_UNKNOWN;
    }
}

SimpleType Parser::ParsePrimary(string &varName, int &primary_line_no)
{
    primary_line_no = token.line_no;
    if (token.token_type == ID)
    {
        varName = token.lexeme;
        if (symbolTable.find(varName) == symbolTable.end())
        {
            symbolTable[varName].type = TYPE_UNKNOWN;
            symbolTable[varName].equalVars.insert(varName);
            varOrder.push_back(varName);
        }
        token = lexer.GetToken();
        return symbolTable[varName].type;
    }
    else if (token.token_type == NUM)
    {
        varName = "";
        token = lexer.GetToken();
        return TYPE_INT;
    }
    else if (token.token_type == REALNUM)
    {
        varName = "";
        token = lexer.GetToken();
        return TYPE_REAL;
    }
    else if (token.token_type == TRUE || token.token_type == FALSE)
    {
        varName = "";
        token = lexer.GetToken();
        return TYPE_BOOL;
    }
    else
    {
        SyntaxError();
        return TYPE_UNKNOWN;
    }
}

void Parser::EnforceType(SimpleType leftType, SimpleType rightType, int line_no, string constraint, string leftVar, string rightVar) {
    if (leftType != TYPE_UNKNOWN && rightType != TYPE_UNKNOWN && leftType != rightType) {
        ReportTypeError(line_no, constraint);
    } else if (leftType == TYPE_UNKNOWN && rightType != TYPE_UNKNOWN && !leftVar.empty()) {
        UpdateType(leftVar, rightType);
    } else if (rightType == TYPE_UNKNOWN && leftType != TYPE_UNKNOWN && !rightVar.empty()) {
        UpdateType(rightVar, leftType);
    }
}

void Parser::UnifyTypes(string var1, string var2)
{
    if (var1.empty() || var2.empty())
    {
        return;
    }

    // handle sets
    set<string> &vars1 = symbolTable[var1].equalVars;
    set<string> &vars2 = symbolTable[var2].equalVars;
    vars1.insert(vars2.begin(), vars2.end());

    // update equalVars
    for (const string &var : vars1)
    {
        symbolTable[var].equalVars = vars1;
    }

    // handle types
    SimpleType type1 = symbolTable[var1].type;
    SimpleType type2 = symbolTable[var2].type;

    if (type1 == TYPE_UNKNOWN && type2 != TYPE_UNKNOWN)
    {
        UpdateType(var1, type2);
    }
    else if (type2 == TYPE_UNKNOWN && type1 != TYPE_UNKNOWN)
    {
        UpdateType(var2, type1);
    }
    else if (type1 != TYPE_UNKNOWN && type2 != TYPE_UNKNOWN && type1 != type2)
    {
        // types conflicts
    }
}

void Parser::UpdateType(string varName, SimpleType type)
{
    if (symbolTable[varName].type == type)
    {
        return;
    }
    symbolTable[varName].type = type;
    for (string v : symbolTable[varName].equalVars)
    {
        if (symbolTable[v].type != type)
        {
            symbolTable[v].type = type;
            UpdateType(v, type);
        }
    }
}

int main()
{
    Parser parser;
    parser.ParseProgram();
    return 0;
}
