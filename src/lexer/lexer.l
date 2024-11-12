%{
#include "tokens.h"

  /* Keep track of current position of lex for error messages, i.e.
     the position just *after* the last token read */
  int line_nr = 1;
  int col_nr = 1;

%}

%%
"println!(\"{".+"}\")"    { return PRINTVAR; }
"println!(\""[^\"]*"\")"    { return PRINTSTRING; }
[ \t\n]+        { /* Ignore whitespace */ }
"fn"            { return FN; }
"let"           { return LET; }
"mut"           { return MUT; }
"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"true"          { return TRUE; }
"false"         { return FALSE; }
[a-zA-Z_][a-zA-Z0-9_]*  { return IDENTIFIER; }
[0-9]+          { return DEC_LITERAL; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
"->"            { return ARROW; }
";"             { return SEMICOLON; }
":"             { return COLON; }
"="             { return EQ; }
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return STAR; }
"/"             { return SLASH; }
"+="            { return PLUSEQ; }
"-="            { return MINUSEQ; }
"&&"            { return ANDAND; }
"||"            { return OROR; }
"!"             { return NOT; }
">"             { return GT; }
">="            { return GE; }
"<"             { return LT; }
"<="            { return LE; }
"=="            { return EQEQ; }
"!="            { return NE; }
"&"             { return AMPERSAND; }
","             { return COMMA; }
.      {
  if (yytext[0] < ' '){ /* non-printable char */
    /*yyerror*/ fprintf(stderr,"illegal character: ^%c",yytext[0] + '@');
  }
  else {
    if (yytext[0] > '~') {
      /* non-printable char printed as octal int padded with zeros, eg \012*/
      /*yyerror(*/fprintf(stderr,"illegal character: \\%03o", (int) yytext[0]);
    }
    else {
      /*yyerror(*/fprintf(stderr,"illegal character: %s",yytext);
    }
  }
  /* lex read exactly one char; the illegal one */
  fprintf(stderr," at line %d column %d\n", line_nr, (col_nr-1));
			       }
%%

/* Function called by (f)lex when EOF is read. If yywrap returns a
   true (non-zero) (f)lex will terminate and continue otherwise.*/
int yywrap(){
  return (1);
}
