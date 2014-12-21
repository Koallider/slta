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
	char isFirstReg=1;
	printf("sys_exit\tequ\t1\nsys_write\tequ\t4\nstdout\tequ\t1\nsys_read\tequ\t3\n");
	printf("global	_start\n");
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
	
	printf("section .bss\n");
	printf("\toutputBuffer\tresb\t4\n");
	printf("\tinputBuffer\tresb\t4\n");
	printf("section .text\n");

	printf("_start:\n");
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
		}else if(op_name[(int) code[i].op]=="out_float"){
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			//TODO только однозначные выводит
				printf("\tmov\tecx,\t[%s]\n",temp->name);
			    printf("\tadd\tecx,\t0x30\n");
				printf("\tmov  [outputBuffer], ecx\n");
				printf("\tmov  ecx, outputBuffer\n");

				printf("\tmov  eax, sys_write\n");
				printf("\tmov  ebx, stdout\n");
				printf("\tmov  edx, 1\n");
				printf("\tint  0x80\n");
		}else if(op_name[(int) code[i].op]=="in_int"){
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			//TODO не работает
				printf("\tmov  eax, sys_read\n");
				printf("\tmov  ebx, 0\n");
				printf("\tmov  edx, 1\n");
				printf("\tmov  ecx, inputBuffer\n");
				printf("\tint  0x80\n");
				printf("\tmov\teax, inputBuffer\n");
				printf("\tmov\t[%s], eax\n",temp->name);
		}else if(op_name[(int) code[i].op]=="ld_var"){
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			if(isFirstReg==1){
				printf("\tmov  eax, [%s]\n",temp->name);
				isFirstReg = 0;
			}else{
				printf("\tmov  ebx, [%s]\n",temp->name);
				isFirstReg = 1;
			}
		}else if(op_name[(int) code[i].op]=="add" || op_name[(int) code[i].op]=="mul" || op_name[(int) code[i].op]=="div" || op_name[(int) code[i].op]=="sub" ){
			printf("\t%s  eax, ebx\n", op_name[(int) code[i].op]);
		}else if(op_name[(int) code[i].op]=="store_int"){
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			printf("\tmov [%s], eax\n",temp->name);
		}	
		i++;
	}
	printf("\tmov eax, 0x1\n");
	printf("\tmov ebx, 0xa\n");
	printf("\tint 0x80\n");
}
/************************** End Code Generator **************************/
