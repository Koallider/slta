#include "rbtree.h"
#include "lgen.h"

/***************************************************************************
Code Generator
***************************************************************************/
/*-------------------------------------------------------------------------
Data Segment
-------------------------------------------------------------------------*/
struct ListNode* list;
int data_offset = 0;
/* Initial offset*/
int data_location()
/* Reserves a data location*/
{
	return data_offset++;
}
/*-------------------------------------------------------------------------
Code Segment
-------------------------------------------------------------------------*/
int code_offset = 0;
/* Initial offset*/
int gen_label()
{
	return code_offset;
} /* Returns current offset */
int reserve_loc()
{
	return code_offset++;
} /* Reserves a code location */
/* Generates code at current location*/
void gen_code( enum code_ops operation, float arg )
{ 
	code[code_offset].op = operation;
	code[code_offset++].arg = arg;
}
/* Generates code at a reserved location
*/
void back_patch( int addr, enum code_ops operation, float arg )
{
	code[addr].op = operation;
	code[addr].arg = arg;
}
/*-------------------------------------------------------------------------
Print Code to file
-------------------------------------------------------------------------*/
void fprint_code(const char* filename)
{
	int i = 0;
	char* label_name;	// var to store generated label name for jmp/goto
	char* prev_op;		// using to determine, which operation (>, < or =) were in while condition, when we finding jmp_false cmd
	struct rbtree* jmp_list = NULL, *found_node = NULL;	// in jmp_list: key - line, where jmp will be, value - label name
	FILE* out = fopen(filename, "w");
	
	if(out == NULL)
	{
	  perror("Cannot open file");
	  return;
	}
	
	for(i = 0; i < code_offset; i++)
	{
	    if(op_name[(int) code[i].op]=="goto" || op_name[(int) code[i].op]=="jmp_false")
	    {
	      label_name = calloc(15, sizeof(char));
	      get_next_label_name(label_name);
	      jmp_list = rbtree_add(jmp_list, (int)code[i].arg, label_name);
	    }
	    printf("%3d: %-10s%4d\n",i,op_name[(int) code[i].op], (int)code[i].arg );
	}
	i = 0;
	
	fprintf(out, "extern printf\n");
	fprintf(out, "extern scanf\n");
	fprintf(out, "global main\n");
	fprintf(out, "section .data\n");
	fprintf(out, "\tinp_int_msg:\tdb\t\"%%d\", 0\n");
	fprintf(out, "\tinp_flt_msg:\tdb\t\"%%lf\", 0\n");
	fprintf(out, "\tout_int_msg:\tdb\t\"%%d\", 10, 0\n");
	fprintf(out, "\tout_flt_msg:\tdb\t\"%%lf\", 10, 0\n");
	
	symrec *temp = sym_table;
	while(temp!=NULL){
		if(temp->type == INT_T){
			fprintf(out, "\t%s:\tdd\t%d\n", temp->name, 0);
		}else if(temp->type == FLOAT_T){
			fprintf(out, "\t%s:\tdq\t%f\n", temp->name, 0.0);
		}
		temp = temp->next;
	}
	
	fprintf(out, "section .bss\n");
	fprintf(out, "\toutputBuffer\tresb\t4\n");
	fprintf(out, "\tinputBuffer\tresb\t4\n");
	fprintf(out, "section .text\n");

	fprintf(out, "main:\n");
	while (i < code_offset) {
		fprintf(out, ";%3d: %-10s%4d\n",i,op_name[(int) code[i].op], (int)code[i].arg );
		
		if((found_node = rbtree_lookup(jmp_list, i)) != NULL && i != 0)	// NullNode in tree has key 0, it is forbidden key
		{								// but zero line is always data,and it is correct
		  fprintf(out, "%s:\t", found_node->value);
		}
		
		if(op_name[(int) code[i].op]=="ld_int"){
			fprintf(out, "\tpush\t%d\n",(int)code[i].arg);
		}else if(op_name[(int) code[i].op]=="ld_float"){
			fprintf(out, "\tfld\tqword\t%f\n",code[i].arg);
			//TODO don't know, may be all right
		}else if(op_name[(int) code[i].op]=="store_int"){
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			fprintf(out, "\tpop\teax\n");
			fprintf(out, "\tmov\t[%s],\teax\n",temp->name);
		}else if(op_name[(int) code[i].op]=="store_float"){		
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			//TODO сделать для флоат
			//fprintf("\tmov\t[%s],\teax\n",temp->name);
		}else if(op_name[(int) code[i].op]=="out_float"){
			int j = 0;
			symrec *temp = sym_table;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			fprintf(out, "\tpush dword [%s + 4]\n", temp->name);
			fprintf(out, "\tpush dword [%s]\n", temp->name);
			fprintf(out, "\tpush out_flt_msg\n");
			fprintf(out, "\tcall printf\n");
			fprintf(out, "\tadd esp, 12\n");
		}else if(op_name[(int) code[i].op]=="in_int"){
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			fprintf(out, "\tpush %s\n", temp->name);
			fprintf(out, "\tpush inp_int_msg\n");
			fprintf(out, "\tcall scanf\n");
			fprintf(out, "\tadd esp, 8\n");
		}else if(op_name[(int) code[i].op]=="in_float"){	// does not work
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			fprintf(out, "\tpush\tebp\n");
			fprintf(out, "\tmov\tebp, esp\n");
			fprintf(out, "\tsub\tesp, 8\n");
			fprintf(out, "\tlea\teax, [ebp-8]\n");
			fprintf(out, "\tpush\teax\n");
			fprintf(out, "\tpush\tinp_flt_msg\n");
			fprintf(out, "\tcall\tscanf\n");
			fprintf(out, "\tadd\tesp, 8\n");
			fprintf(out, "\tfld\tqword [ebp-8]\n");
			fprintf(out, "\tfstp\tqword [%s]\n", temp->name);
			fprintf(out, "\tmov\tesp, ebp\n");
			fprintf(out, "\tpop\tebp\n");
		}else if(op_name[(int) code[i].op]=="out_int"){
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			fprintf(out, "\tpush dword [%s]\n", temp->name);
			fprintf(out, "\tpush out_int_msg\n");
			fprintf(out, "\tcall printf\n");
			fprintf(out, "\tadd esp, 8\n");
		}else if(op_name[(int) code[i].op]=="ld_var"){
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			if(temp->type == INT_T)
			{
			  fprintf(out, "\tpush dword [%s]\n",temp->name);
			}
			else
			{
			  fprintf(out, "\tpush dword [%s]\n",temp->name);
			}
		}else if(op_name[(int) code[i].op]=="add" || op_name[(int) code[i].op]=="sub" ){
			fprintf(out, "\tpop\tebx\n");
			fprintf(out, "\tpop\teax\n");
			fprintf(out, "\t%s  eax, ebx\n", op_name[(int) code[i].op]);
			fprintf(out, "\tpush\teax\n");
		}else if(op_name[(int) code[i].op]=="mul"){
			fprintf(out, "\tpop\tebx\n");
			fprintf(out, "\tpop\teax\n");
			fprintf(out, "\ti%s  eax, ebx\n", op_name[(int) code[i].op]);
			fprintf(out, "\tpush\teax\n");
		}else if(op_name[(int) code[i].op]=="div"){
			fprintf(out, "\tpop\tebx\n");
			fprintf(out, "\tpop\teax\n");
			fprintf(out, "\txor edx, edx\n\tdiv ebx\n");
			fprintf(out, "\tpush\teax\n");
		}else if(op_name[(int) code[i].op]=="lt" || op_name[(int) code[i].op]=="eq" || // while loop implementation
		    op_name[(int) code[i].op]=="gt"){		
			fprintf(out, "\tpop\tebx\n");
			fprintf(out, "\tpop\teax\n");
			fprintf(out, "\tcmp\teax, ebx\n");
			prev_op = op_name[(int) code[i].op];
		}else if(op_name[(int) code[i].op] == "jmp_false"){
			found_node = rbtree_lookup(jmp_list, (int)code[i].arg);
			if(prev_op == "lt")
			{
			  fprintf(out, "\tjae\t%s\n", found_node->value);
			}
			else if(prev_op == "eq")
			{
			  fprintf(out, "\tjne\t%s\n", found_node->value);
			}
			else if(prev_op == "gt")
			{
			  fprintf(out, "\tjle\t%s\n", found_node->value);
			}
		}else if(op_name[(int) code[i].op] == "goto"){
			found_node = rbtree_lookup(jmp_list, (int)code[i].arg);
			fprintf(out, "\tjmp\t%s\n", found_node->value);
		}
		i++;
	}
	fprintf(out, "\tmov eax, 0x1\n");
	fprintf(out, "\tmov ebx, 0xa\n");
	fprintf(out, "\tint 0x80\n");
	fclose(out);
}
/************************** End Code Generator **************************/
