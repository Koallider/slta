%option yylineno
%{
	#include <string.h>
	#include "Simple.tab.h"
	int snum;
	extern void yyerror(char *);
%}

DIGIT	[0-9]
ID		[a-z][a-z0-9]*

%x COMM

%%

":="	 	{ snum += yyleng; return ASSGNOP;						}
[+\-]?{DIGIT}+"."{DIGIT}+ { snum += yyleng; yylval.floatval = atof(yytext);
                                     return FLOAT_NUMBER;           }
[+\-]?{DIGIT}+ 	{ snum += yyleng; yylval.intval = atoi(yytext);
				return INT_NUMBER;						            }
do			{ snum += yyleng; return DO;							}
else		{ snum += yyleng; return ELSE;							}
end			{ snum += yyleng; return END;						    }
fi			{ snum += yyleng; return FI;							}
if			{ snum += yyleng; return IF;							}
begin		{ snum += yyleng; return BEGIN_T;						}
integer		{ snum += yyleng; return INTEGER;					    }
float       { snum += yyleng; return FLOAT;                         }
var			{ snum += yyleng; return VAR;							}
read		{ snum += yyleng; return READ;							}
skip		{ snum += yyleng; return SKIP;							}
then		{ snum += yyleng; return THEN;							}
while		{ snum += yyleng; return WHILE;						    }
write		{ snum += yyleng; return WRITE;						    }
{ID}		{ snum += yyleng; yylval.id = (char *) strdup(yytext);
			  return IDENTIFIER;                					                  }
"/*"                { snum += yyleng;  BEGIN COMM;                                                     }
<COMM>.    { snum += yyleng;  continue;                                                               }
<COMM>\n   { snum += yyleng;  continue;                                                             }
<COMM>"*/"   { snum += yyleng;  BEGIN INITIAL;                                              }
<COMM><<EOF>>   { snum += yyleng;  
            yyerror("Unterminated comment");  BEGIN INITIAL;                            }
[ \t\n\r]+	
"<"                        { snum += yyleng;   return yytext[0];                                            }
">"                        { snum += yyleng;   return yytext[0];                                            }
"="                        { snum += yyleng;   return yytext[0];                                            }
"+"                        { snum += yyleng;   return yytext[0];                                            }
"-"                        { snum += yyleng;   return yytext[0];                                            }
"*"                        { snum += yyleng;   return yytext[0];                                            }
"/"                        { snum += yyleng;   return yytext[0];                                            }
"^"                        { snum += yyleng;   return yytext[0];                                            }
";"                        { snum += yyleng;   return yytext[0];                                            }
","                        { snum += yyleng;   return yytext[0];                                            }
"("                        { snum += yyleng;   return yytext[0];                                            }
")"                        { snum += yyleng;   return yytext[0];                                            }
.			{ yyerror("Unknown character"); snum += yyleng;     		}

%%

int yywrap(void) {}