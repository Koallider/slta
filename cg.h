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
void fprint_code(const char* filename)
{
	int i = 0;
	char isFirstReg=1;
	FILE* out = fopen(filename, "w");
	if(out == NULL)
	{
	  perror("Cannot open file");
	  return;
	}
	
	for(i = 0; i < code_offset; i++)
	{
	    printf("%3d: %-10s%4d\n",i,op_name[(int) code[i].op], (int)code[i].arg );
	}
	i = 0;
	
	fprintf(out, "extern printf\n");
	fprintf(out, "extern scanf\n");
	fprintf(out, "global main\n");
	fprintf(out, "section .data\n");
	fprintf(out, "\tinp_int_msg:\tdb\t\"%%d\", 0\n");
	fprintf(out, "\tinp_flt_msg:\tdb\t\"%%f\", 0\n");
	fprintf(out, "\tout_int_msg:\tdb\t\"%%d\", 10, 0\n");
	fprintf(out, "\tout_flt_msg:\tdb\t\"%%f\", 10, 0\n");
	
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
		if(op_name[(int) code[i].op]=="ld_int"){
		        if(isFirstReg==1){
				fprintf(out, "\tmov\teax,\t%d\n",(int)code[i].arg);
				isFirstReg = 0;
			}else{
				fprintf(out, "\tmov\tebx,\t%d\n",(int)code[i].arg);
				isFirstReg = 1;
			}
		}else if(op_name[(int) code[i].op]=="ld_float"){
			fprintf(out, "\tfld\tqword\t%f\n",code[i].arg);		// ?
		}else if(op_name[(int) code[i].op]=="store_int"){
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
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
			symrec *temp = sym_table;
			/*while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}*/
			int j = 0;
			while(j < code[i].arg)
			{
			  temp = temp->next;
			  j++;
			}
			fprintf(out, "\tpush dword [%s]\n", temp->name);
			fprintf(out, "\tpush out_flt_msg\n");
			fprintf(out, "\tcall printf\n");
			fprintf(out, "\tadd esp, 8\n");		// float's size is not 4
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
		}else if(op_name[(int) code[i].op]=="in_float"){
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			fprintf(out, "\tpush %s\n", temp->name);
			fprintf(out, "\tpush inp_float_msg\n");
			fprintf(out, "\tcall scanf\n");
			fprintf(out, "\tadd esp, 8\n");		// float's size is not 4
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
			if(isFirstReg==1){
				fprintf(out, "\tmov  eax, [%s]\n",temp->name);
				isFirstReg = 0;
			}else{
				fprintf(out, "\tmov  ebx, [%s]\n",temp->name);
				isFirstReg = 1;
			}
		}else if(op_name[(int) code[i].op]=="add" || op_name[(int) code[i].op]=="sub" ){
			fprintf(out, "\t%s  eax, ebx\n", op_name[(int) code[i].op]);
		}else if(op_name[(int) code[i].op]=="mul"){
			fprintf(out, "\ti%s  eax, ebx\n", op_name[(int) code[i].op]);
		}else if(op_name[(int) code[i].op]=="div"){
			fprintf(out, "\txor edx, edx\n\tdiv ebx\n");
		}else if(op_name[(int) code[i].op]=="store_int"){
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			fprintf(out, "\tmov [%s], eax\n",temp->name);
		}	
		i++;
	}
	fprintf(out, "\tmov eax, 0x1\n");
	fprintf(out, "\tmov ebx, 0xa\n");
	fprintf(out, "\tint 0x80\n");
	fclose(out);
}
/************************** End Code Generator **************************/
