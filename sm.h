/***************************************************************************
Stack Machine
***************************************************************************/
/*=========================================================================
DECLARATIONS
=========================================================================*/
/* OPERATIONS: Internal Representation */
enum code_ops { HALT, STORE_INT, STORE_FLOAT, JMP_FALSE, GOTO,
	DATA, LD_INT, LD_FLOAT, LD_VAR,
	READ_INT, WRITE_INT, READ_FLOAT, WRITE_FLOAT,
	LT, EQ, GT, ADD, SUB, MULT, DIV, PWR };
/* OPERATIONS: External Representation */
char *op_name[] = {"halt", "store_int", "store_float", "jmp_false", "goto",
	"data", "ld_int", "ld_float", "ld_var",
	"in_int", "out_int", "in_float", "out_float",
	"lt", "eq", "gt", "add", "sub", "mult", "div", "pwr" };
struct instruction
{
	enum code_ops op;
	float arg;
};
/* CODE Array */
struct instruction code[999];
/* RUN-TIME Stack */
float stack[999];
/*-------------------------------------------------------------------------
Registers
-------------------------------------------------------------------------*/
int pc = 0;
struct instruction ir;
int ar = 0;
int top = 0;
char ch;
/*=========================================================================
Fetch Execute Cycle
=========================================================================*/
void fetch_execute_cycle()
{
	int read_buf;
	do { /*printf( "PC = %3d IR.arg = %8d AR = %3d Top = %3d,%8d\n",
		pc, ir.arg, ar, top, stack[top]); */
		/* Fetch*/
		ir = code[pc++];
		/* Execute*/
		switch (ir.op) {
			case HALT: 
				printf( "halt\n" );
				break;
			case READ_INT : printf( "Input integer: " );
				scanf( "%d", &read_buf ); 
				stack[ar+(int)ir.arg] = (float)read_buf; break;
			case WRITE_INT : printf( "Output: %d\n", (int)stack[top--] ); break;
			case READ_FLOAT : printf( "Input float: " );
				scanf( "%f", &stack[ar+(int)ir.arg] ); break;
			case WRITE_FLOAT : printf( "Output: %f\n", stack[top--] ); break;
			case STORE_INT: stack[(int)ir.arg] = (int)stack[top--];
				break;
			case STORE_FLOAT: stack[(int)ir.arg] = stack[top--];
				break;
			case JMP_FALSE : if ( (int)stack[top--] == 0 )
				pc = (int)ir.arg;
				break;
			case GOTO: pc = (int)ir.arg;
				break;
			case DATA: top = top + (int)ir.arg;
				break;
			case LD_INT: stack[++top] = (int)ir.arg; break;
			case LD_FLOAT: stack[++top] = ir.arg; break;
			case LD_VAR: stack[++top] = stack[ar+(int)ir.arg];break;
			case LT : 
				if ( stack[top-1] < stack[top] )
					stack[--top] = 1.0;
				else stack[--top] = 0.0;
				break;
			case EQ : 
				if ( stack[top-1] == stack[top] )
					stack[--top] = 1.0;
				else stack[--top] = 0.0;
				break;
			case GT : 
				if ( stack[top-1] > stack[top] )
					stack[--top] = 1.0;
				else stack[--top] = 0.0;
				break;
			case ADD: 
				stack[top-1] = stack[top-1] + stack[top];
				top--;
				break;
			case SUB:
				stack[top-1] = stack[top-1] - stack[top];
				top--;
				break;
			case MULT:
				stack[top-1] = stack[top-1] * stack[top];
				top--;
				break;
			case DIV:
				stack[top-1] = stack[top-1] / stack[top];
				top--;
				break;
			case PWR:
				stack[top-1] = stack[top-1] * stack[top];
				top--;
				break;
			default: printf( "Internal Error: Memory Dump\n" );
				break;
		}
	}
	while (ir.op != HALT);
}
/*************************** End Stack Machine **************************/
