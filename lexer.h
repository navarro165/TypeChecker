#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>
#include "inputbuf.h"

// ------- token types -------------------

typedef enum
{
  END_OF_FILE = 0,
  BOOL,
  CASE,
  COLON,
  COMMA,
  DIV,
  EQUAL,
  ERROR,
  FALSE,
  GREATER,
  GTEQ,
  ID,
  IF,
  INT,
  LBRACE,
  LESS,
  LPAREN,
  LTEQ,
  MINUS,
  MULT,
  NOT,
  NOTEQUAL,
  NUM,
  PLUS,
  PRIVATE,
  PUBLIC,
  RBRACE,
  REAL,
  REALNUM,
  RPAREN,
  SEMICOLON,
  SWITCH,
  TRUE,
  WHILE,
} TokenType;

class Token
{
public:
  TokenType token_type;
  int line_no;
  std::string lexeme;
  void Print();
};

class LexicalAnalyzer
{
public:
  Token GetToken();
  TokenType UngetToken(Token);
  LexicalAnalyzer();

private:
  std::vector<Token> tokens;
  int line_no;
  Token tmp;
  InputBuffer input;

  bool SkipSpace();
  bool IsKeyword(std::string);
  TokenType FindKeywordIndex(std::string);
  Token ScanNumber();
  Token ScanIdOrKeyword();
};

#endif //__LEXER__H__
