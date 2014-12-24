/* OPERATIONS: Internal Representation */
enum code_ops { HALT, STORE_INT, STORE_FLOAT, JMP_FALSE, GOTO,
	DATA, LD_INT, LD_FLOAT, LD_VAR,
	READ_INT, WRITE_INT, READ_FLOAT, WRITE_FLOAT,
	LT, EQ, GT, ADD, SUB, MULT, DIV, PWR };
/* OPERATIONS: External Representation */
char *op_name[] = {"halt", "store_int", "store_float", "jmp_false", "goto",
	"data", "ld_int", "ld_float", "ld_var",
	"in_int", "out_int", "in_float", "out_float",
	"lt", "eq", "gt", "add", "sub", "mul", "div", "pwr" };

struct arg
{
	char* name;
	float value;
	char isConst;
};	
	
struct instruction
{
	enum code_ops op;
	float arg;
};
/* CODE Array */
struct instruction code[999];
/* RUN-TIME Stack */
float stack[999];