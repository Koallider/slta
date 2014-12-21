/***************************************************************************
Code Generator
***************************************************************************/
/*-------------------------------------------------------------------------
Data Segment
-------------------------------------------------------------------------*/
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
	while (i < code_offset) {
		printf("%3d: %-10s%4d\n",i,op_name[(int) code[i].op], (int)code[i].arg );
		i++;
	}
}
/************************** End Old Code Generator **************************/

/************************** NASM Addition  **************************/

/****************************************************************************
 * bss segment generation with buffer variable
 ***************************************************************************/
void gen_data_segment(char* filename)
{
    FILE* out = fopen(filename, "w");
    fprintf(out, "%%include \"io.inc\"\n");
    fprintf(out, "section .bss\n\nbuf\tresd\t1\n\nsection .text\n");
    fprintf(out, "section .code\n");
    fclose(out);
}


/**************************************************************************
 * Add variables to stack, default value is zero, offset like in symbol 
 * table (symrec)
 *************************************************************************/
void gen_vars_reservation(char* filename)
{
    int count, i;
    FILE* out;
    if(code[0].op == DATA)
    {
      count = code[0].arg;
      out = fopen(filename, "a");
      for(i = 0; i <= count; i++)
      {
	fprintf(out, "push 0\n");
      }
      fprintf(out, "mov eax, 0\n");
      fclose(out);
    }
}