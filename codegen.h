#include "rbtree.h"
#include "rbtree_fl.h"
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
	struct rbtree_fl* const_float_list = NULL, *found_float_const = NULL;
	struct rbtree* const_int_list = NULL, *found_int_const = NULL;
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
	
	for(i = 0; i < code_offset; i++)
	{
	    if(op_name[(int) code[i].op]=="ld_int")
	    {
	      if((found_int_const = rbtree_lookup(const_int_list, (int)code[i].arg)) == NULL)
	      {
		label_name = calloc(15, sizeof(char));
		get_next_label_name(label_name);
		const_int_list = rbtree_add(const_int_list, (int)code[i].arg, label_name);
		fprintf(out, "\t%s:\tdd\t%d\n", label_name, (int)code[i].arg);
	      }
	    }
	    else if(op_name[(int) code[i].op]=="ld_float")
	    {
	      if((found_float_const = rbtree_lookup_fl(const_float_list, code[i].arg)) == NULL)
	      {
		label_name = calloc(15, sizeof(char));
		get_next_label_name(label_name);
		const_float_list = rbtree_add_fl(const_float_list, code[i].arg, label_name);
		fprintf(out, "\t%s:\tdq\t%f\n", label_name, code[i].arg);
	      }
	    }
	}
	i = 0;
	
	fprintf(out, "section .bss\n");
	fprintf(out, "\tbuf\tresq\t1\n");
	fprintf(out, "section .text\n");

	fprintf(out, "main:\n");
	while (i < code_offset) {
		if((found_node = rbtree_lookup(jmp_list, i)) != NULL && i != 0)	// NullNode in tree has key 0, it is forbidden key
		{								// but zero line is always data,and it is correct
		  fprintf(out, "%s:\t", found_node->value);
		}
		
		if(op_name[(int) code[i].op]=="ld_int"){
			found_int_const = rbtree_lookup(const_int_list, (int)code[i].arg);
			fprintf(out, "\tfild\tdword [%s]\n",found_int_const->value);
		}else if(op_name[(int) code[i].op]=="ld_float"){
			found_float_const = rbtree_lookup_fl(const_float_list, code[i].arg);
			fprintf(out, "\tfld\tqword [%s]\n",found_float_const->value);
		}else if(op_name[(int) code[i].op]=="store_int"){
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			fprintf(out, "\tfistp\tdword [%s]\n",temp->name);
		}else if(op_name[(int) code[i].op]=="store_float"){		
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			fprintf(out, "\tfstp\tqword [%s]\n",temp->name);
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
			  fprintf(out, "\tfild\tdword [%s]\n",temp->name);
			}
			else
			{
			  fprintf(out, "\tfld\tqword [%s]\n",temp->name);
			}
		}else if(op_name[(int) code[i].op]=="add") {
			fprintf(out, "\tfstp\tqword [buf]\n");
			fprintf(out, "\tfadd\tqword [buf]\n");
		}else if(op_name[(int) code[i].op]=="sub"){
			fprintf(out, "\tfstp\tqword [buf]\n");
			fprintf(out, "\tfsub\tqword [buf]\n");
		}else if(op_name[(int) code[i].op]=="mul"){
			fprintf(out, "\tfstp\tqword [buf]\n");
			fprintf(out, "\tfmul\tqword [buf]\n");
		}else if(op_name[(int) code[i].op]=="div"){
			fprintf(out, "\tfstp\tqword [buf]\n");
			fprintf(out, "\tfdiv\tqword [buf]\n");
		}else if(op_name[(int) code[i].op]=="lt" || op_name[(int) code[i].op]=="eq" || // while loop implementation
		    op_name[(int) code[i].op]=="gt"){
			fprintf(out, "\tfstp\tqword [buf]\n");
			fprintf(out, "\tfcomp\tqword [buf]\n");
			fprintf(out, "\tfstp\tqword [buf]\n");
			fprintf(out, "\twait\n");
			fprintf(out, "\tfstsw\tax\n");
			fprintf(out, "\tsahf\n");
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
			  fprintf(out, "\tjbe\t%s\n", found_node->value);
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
