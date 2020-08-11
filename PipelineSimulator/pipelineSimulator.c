#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define NUMMEMORY 65536 /* maximum number of data words in memory */
#define NUMREGS 8 /* number of machine registers */
#define ADD 0
#define NAND 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5 /* JALR – not implemented in this project */
#define HALT 6
#define NOOP 7
#define NOOPINSTRUCTION 0x1c00000

typedef struct IFIDstruct{
	int instr;
	int pcplus1;
} IFIDType;

typedef struct IDEXstruct{
	int instr;
	int pcplus1;
	int readregA;
	int readregB;
	int offset;
} IDEXType;

typedef struct EXMEMstruct{
	int instr;
	int branchtarget;
	int aluresult;
	int readreg;
} EXMEMType;

typedef struct MEMWBstruct{
	int instr;
	int writedata;
} MEMWBType;

typedef struct WBENDstruct{
	int instr;
	int writedata;
} WBENDType;

typedef struct statestruct{
	int pc;
	int instrmem[NUMMEMORY];
	int datamem[NUMMEMORY];
	int reg[NUMREGS];
	int numMemory;
	IFIDType IFID;
	IDEXType IDEX;
	EXMEMType EXMEM;
	MEMWBType MEMWB;
	WBENDType WBEND;
	int cycles; /* Number of cycles run so far */
	int fetched; /* Total number of instructions fetched */
	int retired; /* Total number of completed instructions */
	int branches; /* Total number of branches executed */
	int mispreds; /* Number of branch mispredictions*/
} statetype;

void ifStage(statetype* state, statetype* newstate);
void idStage(statetype* state, statetype* newstate);
void exStage(statetype* state, statetype* newstate);
void memStage(statetype* state, statetype* newstate);
void wbStage(statetype* state, statetype* newstate);
int signExtend(int num);
void printstate(statetype *stateptr);
void printinstruction(int instr);
int halt_index;

int field0(int instruction){
	return( (instruction>>19) & 0x7);
}

int field1(int instruction){
	return( (instruction>>16) & 0x7);
}

int field2(int instruction){
	return(instruction & 0xFFFF);
}

int opcode(int instruction){
	return(instruction>>22);
}



int main(int argc, char** argv)
{
	char* fname;
	int cin = 0;
	opterr = 0;

	while((cin = getopt(argc, argv, "i:")) != -1){
	//Reads the command
		switch(cin)
		{
			case 'i':
				fname=(char*)malloc(strlen(optarg));
				fname[0] = '\0';

				strncpy(fname, optarg, strlen(optarg)+1);
				printf("FILE: %s\n", fname);
				break;
			case '?':
				if(optopt == 'i'){
					printf("Option -%c requires an argument.\n", optopt);
				}
				else if(isprint(optopt)){
					printf("Unknown option `-%c'.\n", optopt);
				}
				else{
					printf("Unknown option character `\\x%x'.\n", optopt);
					return 1;
				}
				break;
			default:
				abort();
		}
	}

	//Reads the file
	FILE *fp = fopen(fname, "r");
	if (fp == NULL) {
		printf("Cannot open file '%s' : %s\n", fname, strerror(errno));
		return -1;
	}

	/* count the number of lines by counting newline characters */
	int line_count = 0;
	int c;
	while (EOF != (c=getc(fp))) {
		if ( c == '\n' ){
			line_count++;
		}
	}
	// reset fp to the beginning of the file
	rewind(fp);

	//Initialize state
	statetype* state = (statetype*)malloc(sizeof(statetype));
	statetype* newstate = (statetype*)malloc(sizeof(statetype));

	state->pc = 0;
	memset(state->instrmem, 0, NUMMEMORY*sizeof(int));
	memset(state->datamem, 0, NUMMEMORY*sizeof(int));
	memset(state->reg, 0, NUMREGS*sizeof(int));

	state->numMemory = line_count;
	state->IFID.instr=NOOPINSTRUCTION;
	state->IDEX.instr=NOOPINSTRUCTION;
	state->EXMEM.instr=NOOPINSTRUCTION;
	state->MEMWB.instr=NOOPINSTRUCTION;
	state->WBEND.instr=NOOPINSTRUCTION;

	//Initialize variables to 0
	state->IFID.pcplus1=0;
	state->IDEX.pcplus1=0;
	state->fetched=0;
	state->retired=-3;
	state->branches=0;
	state->mispreds=0;

	state->IDEX.readregA=state->reg[field0(state->IFID.instr)];
	state->IDEX.readregB=state->reg[field1(state->IFID.instr)];
	state->IDEX.offset=signExtend(field2(state->IFID.instr));

	//Load into instrmem and datamem
	char line[256];
	halt_index = 0;
	int i = 0;
	while (fgets(line, sizeof(line), fp)) {
		/* note that fgets doesn't strip the terminating \n, checking its
		   presence would allow to handle lines longer that sizeof(line) */
		int x = atoi(line);
		state->instrmem[i] = x;
		state->datamem[i] = x;
		if (opcode(x) == HALT)
			halt_index = i;
		i++;
	}
	fclose(fp);
	free(fname);


	/** Run the simulation **/
	while(1){
		printstate(state);

		/* check for halt */
		if(HALT == opcode(state->MEMWB.instr)) {
			printf("machine halted\n");
			printf("total of %d cycles executed\n", state->cycles);
			printf("total of %d instructions fetched\n", state->fetched);
			printf("total of %d instructions retired\n", state->retired);
			printf("total of %d branches executed\n", state->branches);
			printf("total of %d branch mispredictions\n", state->mispreds);
			exit(0);
		}

		*newstate = *state;

		//Increment cycles, fetched, and retired
		newstate->cycles++;
		//newstate->fetched++;
		//newstate->retired++;


		/*------------------ IF stage ----------------- */
		ifStage(state, newstate);

		/*------------------ ID stage ----------------- */
		idStage(state, newstate);

		/*------------------ EX stage ----------------- */
		exStage(state, newstate);

		/*------------------ MEM stage ----------------- */
		memStage(state, newstate);

		/*------------------ WB stage ----------------- */
		wbStage(state, newstate);

		*state = *newstate; /* this is the last statement before the end of the loop.
		It marks the end of the cycle and updates the current
		state with the values calculated in this cycle
		– AKA "Clock Tick". */
		//run(state);
			//free(state);
	}
    free(state);
    free(newstate);
}

void ifStage(statetype* state, statetype* newstate)
{
	//Pass data to the next stage of the pipeline
	newstate->IFID.instr = state->instrmem[state->pc];
	newstate->IFID.pcplus1 = state->pc+1;
	newstate->pc = state->pc+1;
	if(state->pc <= halt_index)
		newstate->fetched++;

	//Insert a noop if the next instruction is a LW

	if(opcode(state->IFID.instr) == LW )
	{
		if (field0(state->IFID.instr) == field0(state->instrmem[state->pc]) || field0(state->IFID.instr) == field1(state->instrmem[state->pc]))
		{
			newstate->IFID.instr = NOOPINSTRUCTION;
			newstate->pc--;
			newstate->fetched--;
			newstate->retired--;
		}
	}
}
void idStage(statetype* state, statetype* newstate)
{
	//Pass data to the next stage of the pipeline
	newstate->IDEX.instr=state->IFID.instr;
	newstate->IDEX.pcplus1=state->IFID.pcplus1;

	//Read from the registers
	newstate->IDEX.readregA = state->reg[field0(state->IFID.instr)];
	newstate->IDEX.readregB = state->reg[field1(state->IFID.instr)];
	newstate->IDEX.offset=signExtend(field2(state->IFID.instr));
}

void exStage(statetype* state, statetype* newstate)
{
	//Pass data to the next stage of the pipeline
	newstate->EXMEM.instr=state->IDEX.instr;
	newstate->EXMEM.branchtarget=state->IDEX.pcplus1+state->IDEX.offset;

	//Set the register to be pointers to the register from state.
	int readregA = (state->IDEX.readregA);
	int readregB = (state->IDEX.readregB);

	//Forward data from WBEND to this register
	if(opcode(state->WBEND.instr) == LW)
	{
		if(field0(state->WBEND.instr) == field0(state->IDEX.instr))
		{
			readregA = state->WBEND.writedata;
		}
		if(field0(state->WBEND.instr) == field1(state->IDEX.instr))
		{
			readregB = state->WBEND.writedata;
		}
	}

	//Forward data from MEMWB to this register

	if(opcode(state->MEMWB.instr) == LW)
	{
		if(field0(state->MEMWB.instr) == field0(state->IDEX.instr))
		{
			readregA = state->MEMWB.writedata;
		}
		if(field0(state->MEMWB.instr) == field1(state->IDEX.instr))
		{
			readregB = state->MEMWB.writedata;
		}
	}
	if(opcode(state->MEMWB.instr) == ADD || opcode(state->MEMWB.instr) == NAND)
		{
			if(field2(state->MEMWB.instr) == field0(state->IDEX.instr))
			{
				readregA = state->MEMWB.writedata;
			}
			if(field2(state->MEMWB.instr) == field1(state->IDEX.instr))
			{
				readregB = state->MEMWB.writedata;
			}
	}
	//Forward data from EXMEM to this register
	if(opcode(state->EXMEM.instr) == ADD || opcode(state->EXMEM.instr) == NAND)
	{
		if(field2(state->EXMEM.instr) == field0(state->IDEX.instr))
		{
			readregA = state->EXMEM.aluresult;
		}
		if(field2(state->EXMEM.instr) == field1(state->IDEX.instr))
		{
			readregB = state->EXMEM.aluresult;
		}
	}
	if(opcode(state->IDEX.instr) == ADD)
	{
		newstate->EXMEM.aluresult = readregA + readregB;
	}

	else if(opcode(state->IDEX.instr) == NAND)
	{
		newstate->EXMEM.aluresult  = ~(readregA & readregB);
	}

	else if(opcode(state->IDEX.instr) == LW)
	{
		newstate->EXMEM.aluresult = readregB + state->IDEX.offset;
		newstate->EXMEM.readreg = readregA;
	}

	else if(opcode(state->IDEX.instr) == BEQ)
	{
		newstate->EXMEM.aluresult = (readregA == readregB);
	}
}

void memStage(statetype* state, statetype* newstate)
{
	//Pass data to the next stage of the pipeline
	newstate->MEMWB.instr = state->EXMEM.instr;
	newstate->MEMWB.writedata = state->EXMEM.aluresult;

	//Load or store from memory
	if (opcode(newstate->MEMWB.instr) == LW)
	{
		newstate->MEMWB.writedata = state->datamem[state->EXMEM.aluresult];
	}
	else if (opcode(newstate->MEMWB.instr) == SW)
	{
		newstate->datamem[state->EXMEM.aluresult] = state->EXMEM.readreg;
	}

	//Flush the registers if we need to branch
	if(opcode(state->EXMEM.instr)==BEQ)
	{
		if( state->EXMEM.aluresult==1)
		{
			newstate->pc = state->EXMEM.branchtarget;
			newstate->IFID.instr=NOOPINSTRUCTION;
			newstate->IDEX.instr=NOOPINSTRUCTION;
			newstate->EXMEM.instr=NOOPINSTRUCTION;
			newstate->mispreds=state->mispreds+1;
			newstate->retired = newstate->retired-3;
		}
		newstate->branches=state->branches+1;
	}
}

void wbStage(statetype* state, statetype* newstate)
{
	//Pass data to the end of the pipeline
	newstate->WBEND.instr = state->MEMWB.instr;
	newstate->WBEND.writedata = state->MEMWB.writedata;

	//Write back to the registers
	if (opcode(newstate->WBEND.instr) == LW) {
		newstate->reg[field0(newstate->WBEND.instr)] = state->MEMWB.writedata;
	}
	else if (opcode(newstate->WBEND.instr) == ADD ||
		opcode(newstate->WBEND.instr) == NAND ){
		newstate->reg[field2(newstate->WBEND.instr)] = state->MEMWB.writedata;
	}
	newstate->retired++;
}

void printinstruction(int instr) {
	char opcodestring[10];
	if (opcode(instr) == ADD) {
		strcpy(opcodestring, "add");
	}

	else if (opcode(instr) == NAND)
	{
		strcpy(opcodestring, "nand");
	}

	else if (opcode(instr) == LW) {
		strcpy(opcodestring, "lw");
	}

	else if (opcode(instr) == SW) {
		strcpy(opcodestring, "sw");
	}

	else if (opcode(instr) == BEQ) {
		strcpy(opcodestring, "beq");
	}

	else if (opcode(instr) == JALR) {
		strcpy(opcodestring, "jalr");
	}

	else if (opcode(instr) == HALT) {
		strcpy(opcodestring, "halt");
	}
	else if (opcode(instr) == NOOP) {
		strcpy(opcodestring, "noop");
	}

	else {
		strcpy(opcodestring, "data");
	}

	if(opcode(instr) == ADD || opcode(instr) == NAND){
		printf("%s %d %d %d\n", opcodestring, field2(instr), field0(instr), field1(instr));
	}

	else if(0 == strcmp(opcodestring, "data")){
		printf("%s %d\n", opcodestring, signExtend(field2(instr)));
	}

	else{
		printf("%s %d %d %d\n", opcodestring, field0(instr), field1(instr),
		signExtend(field2(instr)));
	}
}

void printstate(statetype *stateptr){
	int i;
	printf("\n@@@\nstate before cycle %d starts\n", stateptr->cycles);
	printf("\tpc %d\n", stateptr->pc);
	printf("\tdata memory:\n");
	for (i=0; i<stateptr->numMemory; i++) {
		printf("\t\tdatamem[ %d ] %d\n", i, stateptr->datamem[i]);
	}
	printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
		printf("\t\treg[ %d ] %d\n", i, stateptr->reg[i]);
	}
	printf("\tIFID:\n");
	printf("\t\tinstruction ");
	printinstruction(stateptr->IFID.instr);
	printf("\t\tpcplus1 %d\n", stateptr->IFID.pcplus1);
	printf("\tIDEX:\n");
	printf("\t\tinstruction ");
	printinstruction(stateptr->IDEX.instr);
	printf("\t\tpcplus1 %d\n", stateptr->IDEX.pcplus1);
	printf("\t\treadregA %d\n", stateptr->IDEX.readregA);
	printf("\t\treadregB %d\n", stateptr->IDEX.readregB);
	printf("\t\toffset %d\n", stateptr->IDEX.offset);
	printf("\tEXMEM:\n");
	printf("\t\tinstruction ");
	printinstruction(stateptr->EXMEM.instr);
	printf("\t\tbranchtarget %d\n", stateptr->EXMEM.branchtarget);
	printf("\t\taluresult %d\n", stateptr->EXMEM.aluresult);
	printf("\t\treadreg %d\n", stateptr->EXMEM.readreg);
	printf("\tMEMWB:\n");
	printf("\t\tinstruction ");
	printinstruction(stateptr->MEMWB.instr);
	printf("\t\twritedata %d\n", stateptr->MEMWB.writedata);
	printf("\tWBEND:\n");
	printf("\t\tinstruction ");
	printinstruction(stateptr->WBEND.instr);
	printf("\t\twritedata %d\n", stateptr->WBEND.writedata);
}

int signExtend(int num){
	// convert a 16-bit number into a 32-bit integer
	if (num & (1<<15) ) {
		num -= (1<<16);
	}
	return num;
}
