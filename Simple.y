%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "LinkedList.h"
	#include "st.h"			// “аблица идентификаторов
	#include "sm.h"			// —тек
	#include "cg.h"			// √енератор кода

	#define YYDEBUG 1
	int errors;				// количество ошибок
	
	extern int yylineno;
	extern int snum;
	extern char *yytext;
	
	struct 
	lbs			// €рлыки дл€ данных, if и while
	{
		int for_goto;
		int for_jmp_false;
	};
	
	// выдел€ет пам€ть дл€ новой записи (€рлыка)
	struct lbs * newlblrec()
	{
		return (struct lbs *)malloc(sizeof(struct lbs));
	}
	
	// добавление нового идентификатора и проверка, не был ли этот идентификатор
	// уже объ€влен
	install(char *sym_name, enum types t)
	{
		symrec *s;
		s = getsym(sym_name);
		if(s == 0)
			s = putsym(sym_name, t);
		else
		{
			errors++;
			printf("%s is already defined\n", sym_name);
		}
	}
	
	// если идентификатор объ€влен, сгенерировать код
	context_check(enum code_ops operation, char *sym_name)
	{
		symrec *identifier;
		identifier = getsym(sym_name);
		if(identifier == 0)
		{
			errors++;
			printf("%s", sym_name);
			printf("%s\n", " is an undeclared identifier");
		}
		else gen_code(operation, identifier->offset);
	}
	
	enum types get_type(char *sym_name)
	{
	   symrec *identifier;
		identifier = getsym(sym_name);
		if(identifier == 0)
		{
			errors++;
			printf("%s", sym_name);
			printf("%s\n", " is an undeclared identifier");
			return ERROR_T;
		}
		return identifier->type;
	}
	

	
	void add_name_to_buf(char* name)
	{
	       if(list != NULL)
	       {
	           list = ListAddFront(list, name, 0);
	       }
	       else
	       {
	           list = ListCreateNode(name, 0);
	       }
	}
	
	// очищает буфер, вставл€€ в таблицу переменных все переменные указанного типа
	void flush_to_table(enum types t)
	{
	       struct ListNode* prev_node;
	       while(list != NULL)
	       {
	           install(list->data, t);
	           prev_node = list;
	           list = list->next;
	           free(prev_node);
	       }
	}
%}

%union
{
	int intval;			// целочисленные значени€
	float floatval;
	char *id;			// идентификаторы
	struct lbs *lbls;	// дл€ исправлений
}

%start program
%token <intval> INT_NUMBER
%token <floatval> FLOAT_NUMBER
%token <id> 	IDENTIFIER
%token <lbls>	IF WHILE
%token SKIP THEN ELSE FI DO END
%token INTEGER FLOAT READ WRITE VAR BEGIN_T
%token ASSGNOP

%right '<' '>' '='
%left '-' '+'
%left '*' '/'
%right '^'

%%
program : VAR              
			declarations 			 
		  BEGIN_T 				{ gen_code(DATA, data_location() - 1 );          					}
			commands 
		  END			 { gen_code(HALT, 0); YYACCEPT; 					            		}
;
declarations: /*empty*/
	| declarations INTEGER id_seq IDENTIFIER';'{ add_name_to_buf($4); flush_to_table(INT_T); }
	| declarations FLOAT id_seq IDENTIFIER';'       { add_name_to_buf($4); flush_to_table(FLOAT_T); }
	| declarations error;
;
id_seq : /*empty*/
	| id_seq IDENTIFIER ','			 { add_name_to_buf($2);											}
;
commands : /*empty*/
	| commands command ';'
	| error';'
;
command : SKIP
	| READ IDENTIFIER 				{ enum types t = get_type($2);  if(t == INT_T) {context_check(READ_INT, $2); 	} 
	                                                                               else  if(t == FLOAT_T) { context_check(READ_FLOAT, $2);  } 		}
	| WRITE IDENTIFIER					{ enum types t = get_type($2);  if(t == INT_T) {context_check(WRITE_INT, $2); 	} 
	                                                                               else  if(t == FLOAT_T) { context_check(WRITE_FLOAT, $2);   } }
	| IDENTIFIER ASSGNOP exp 		{ enum types t = get_type($1);  if(t == INT_T) {context_check(STORE_INT, $1); 	} 
	                                                                               else  if(t == FLOAT_T) { context_check(STORE_FLOAT, $1);   }	}
	
	| IF exp 						{ $1 = (struct lbs *) newlblrec();
										$1->for_jmp_false = reserve_loc(); 						}
	 THEN commands 					{ $1->for_goto = reserve_loc();							}
	 ELSE							{ back_patch($1->for_jmp_false, JMP_FALSE, gen_label() );	         }
	 commands FI 					{ back_patch($1->for_goto, GOTO, gen_label()); 				          }
	 
	| WHILE 						{ $1 = (struct lbs *) newlblrec();
										$1->for_goto = gen_label(); 							}
	 exp 							{ $1->for_jmp_false = reserve_loc(); 						        }
	DO
	 commands
	END 							{ gen_code(GOTO, $1->for_goto);
										back_patch($1->for_jmp_false, JMP_FALSE, gen_label());	}
;
exp :  INT_NUMBER				{ gen_code(LD_INT, $1); 									}
         | FLOAT_NUMBER                             { gen_code(LD_FLOAT, $1);                                                                               } 
	| IDENTIFIER 					{ context_check(LD_VAR, $1);								}
	| exp '<' exp					{ gen_code(LT, 0);											}
	| exp '=' exp					{ gen_code(EQ, 0);											}
	| exp '>' exp					{ gen_code(GT, 0);											}
	| exp '+' exp 					{ gen_code(ADD, 0);										}
	| exp '-' exp					{ gen_code(SUB, 0);										}
	| exp '*' exp					{ gen_code(MULT, 0);										}
	| exp '/' exp 					{ gen_code(DIV, 0);											}
	| exp '^' exp 					{ gen_code(PWR, 0);										}
	| '(' exp ')'
;
%%

main(int argc, char *argv[])
{
	extern FILE *yyin;
	++argv; --argc;
	yyin = fopen(argv[0], "r");
	errors = 0;
	yyparse();
	printf("Parse Completed\n");
	if(errors == 0)
	{
		fprint_code("program.asm");
	}
	else
	{
	   printf("%d errors\n", errors);
	}
}

yyerror(char* s)
{
	errors++;
	printf("%s (%d, %d): %s\n", s, yylineno, snum, yytext);
}
