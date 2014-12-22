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
	
	fprintf(out, "%%include \"io.inc\"\n");
	fprintf(out, "sys_exit\tequ\t1\nsys_write\tequ\t4\nstdout\tequ\t1\nsys_read\tequ\t3\n");
	fprintf(out, "global	main\n");
	fprintf(out, "section .data\n");
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
			//TODO только однозначные выводит
				/*fprintf(out, "\tmov\tecx,\t[%s]\n",temp->name);
			    fprintf(out, "\tadd\tecx,\t0x30\n");
				fprintf(out, "\tmov  [outputBuffer], ecx\n");
				fprintf(out, "\tmov  ecx, outputBuffer\n");

				fprintf(out, "\tmov  eax, sys_write\n");
				fprintf(out, "\tmov  ebx, stdout\n");
				fprintf(out, "\tmov  edx, 1\n");
				fprintf(out, "\tint  0x80\n");*/
			fprintf(out, "PRINT_DEC 1, %s\nNEWLINE\n", temp->name);		
		}else if(op_name[(int) code[i].op]=="in_int"){
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			fprintf(out, "GET_DEC 4, [%s]\n", temp->name);
		}else if(op_name[(int) code[i].op]=="in_float"){
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			fprintf(out, "GET_DEC 4, [%s]\n", temp->name);
		}else if(op_name[(int) code[i].op]=="out_int"){
			symrec *temp = sym_table;
			int j=0;
			while(j<(data_offset-2)-(int)code[i].arg){
				temp = temp->next;
				j++;
			}
			
			fprintf(out, "PRINT_DEC 4, %s\nNEWLINE\n", temp->name);
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
		}else if(op_name[(int) code[i].op]=="mul" || op_name[(int) code[i].op]=="div"){
			fprintf(out, "\ti%s  eax, ebx\n", op_name[(int) code[i].op]);
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
