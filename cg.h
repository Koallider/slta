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
Print Code to stdio
-------------------------------------------------------------------------*/
void print_code()
{
	int i = 0;
	printf("section .data\n");
	symrec *temp = sym_table;
	while(temp!=NULL){
		if(temp->type == INT_T){
			printf("\t%s:\tdd\t%d\n", temp->name, 0);
		}else if(temp->type == FLOAT_T){
			printf("\t%s:\tdq\t%f\n", temp->name, 0.0);
		}
		temp = temp->next;
	}
	printf("section .text\n");
	printf("global	main\n");
	printf("main:\n");
	while (i < code_offset) {
		
		if(op_name[(int) code[i].op]=="ld_int"){
			printf("\tmov\teax,\t%d\n",(int)code[i].arg);
		}else if(op_name[(int) code[i].op]=="ld_float"){
			printf("\tfld\tqword\t%f\n",code[i].arg);
		}else if(op_name[(int) code[i].op]=="store_int"){
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			printf("\tmov\t[%s],\teax\n",temp->name);
		}else if(op_name[(int) code[i].op]=="store_float"){
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			//TODO сделать для флоат
			//printf("\tmov\t[%s],\teax\n",temp->name);
		}
		
		//printf("%3d: %-10s%4d\n",i,op_name[(int) code[i].op], (int)code[i].arg );
		i++;
	}
}
/************************** End Code Generator **************************/
